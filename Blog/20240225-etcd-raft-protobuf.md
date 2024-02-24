>etcd 是怎么处理网络消息的？

书接上篇，复盘一下etcdServer 从收到一个KV 存储消息之后的完整处理流程！对于网络编程、Raft 处理流程都以此作为契机进行系统化地梳理！


## etcd 启动流程分析

etcd 的启动方法位于server/main.go，启动流程的调用栈如下所示：

```
server/main.go: func Main(args []string)
↓

server/etcdmain/etcd.go: func startEtcdOrProxyV2(args []string)
↓

// startEtcd runs StartEtcd in addition to hooks needed for standalone etcd.
server/etcdmain/etcd.go: func startEtcd(cfg *embed.Config) (<-chan struct{}, <-chan error, error)
↓

// StartEtcd launches the etcd server and HTTP handlers for client/server communication.
// The returned Etcd.Server is not guaranteed to have joined the cluster. Wait
// on the Etcd.Server.ReadyNotify() channel to know when it completes and is ready for use.
server/embed/etcd.go: func StartEtcd(inCfg *Config) (e *Etcd, err error)
↓

// Start performs any initialization of the Server necessary for it to
// begin serving requests. It must be called before Do or Process.
// Start must be non-blocking; any long-running server functionality
// should be implemented in goroutines.
server/server.go: func (s *EtcdServer) Start()
↓

// start prepares and starts server in a new goroutine. It is no longer safe to
// modify a server's fields after it has been sent to Start.
// This function is just used for testing.
server/server.go: func (s *EtcdServer) start() 
```

在`server/server.go: func (s *EtcdServer) start()` 方法中创建了各个channel 用于通信，以及启动了一个goroutine 执行run() 方法

```go
// start prepares and starts server in a new goroutine. It is no longer safe to
// modify a server's fields after it has been sent to Start.
// This function is just used for testing.
func (s *EtcdServer) start() {
	lg := s.Logger()

	if s.Cfg.SnapshotCount == 0 {
		lg.Info(
			"updating snapshot-count to default",
			zap.Uint64("given-snapshot-count", s.Cfg.SnapshotCount),
			zap.Uint64("updated-snapshot-count", DefaultSnapshotCount),
		)
		s.Cfg.SnapshotCount = DefaultSnapshotCount
	}
	if s.Cfg.SnapshotCatchUpEntries == 0 {
		lg.Info(
			"updating snapshot catch-up entries to default",
			zap.Uint64("given-snapshot-catchup-entries", s.Cfg.SnapshotCatchUpEntries),
			zap.Uint64("updated-snapshot-catchup-entries", DefaultSnapshotCatchUpEntries),
		)
		s.Cfg.SnapshotCatchUpEntries = DefaultSnapshotCatchUpEntries
	}

	s.w = wait.New()
	s.applyWait = wait.NewTimeList()
	s.done = make(chan struct{})
	s.stop = make(chan struct{})
	s.stopping = make(chan struct{}, 1)
	s.ctx, s.cancel = context.WithCancel(context.Background())
	s.readwaitc = make(chan struct{}, 1)
	s.readNotifier = newNotifier()
	s.leaderChanged = notify.NewNotifier()
	if s.ClusterVersion() != nil {
		lg.Info(
			"starting etcd server",
			zap.String("local-member-id", s.MemberId().String()),
			zap.String("local-server-version", version.Version),
			zap.String("cluster-id", s.Cluster().ID().String()),
			zap.String("cluster-version", version.Cluster(s.ClusterVersion().String())),
		)
		membership.ClusterVersionMetrics.With(prometheus.Labels{"cluster_version": version.Cluster(s.ClusterVersion().String())}).Set(1)
	} else {
		lg.Info(
			"starting etcd server",
			zap.String("local-member-id", s.MemberId().String()),
			zap.String("local-server-version", version.Version),
			zap.String("cluster-version", "to_be_decided"),
		)
	}

	// TODO: if this is an empty log, writes all peer infos
	// into the first entry
	go s.run()
}
```

在`server/server.go: func (s *EtcdServer) run()` 方法中很主要的就是对于raft 相关封装的调用


## etcdServer 接收到KV 消息

在etcd v3 版本中是使用gRPC 封装的网络通信，这里针对最常见的Servive KV 进行分析，其proto 定义如下

```proto
service KV {
  // Range gets the keys in the range from the key-value store.
  rpc Range(RangeRequest) returns (RangeResponse) {
      option (google.api.http) = {
        post: "/v3alpha/kv/range"
        body: "*"
    };
  }

  // Put puts the given key into the key-value store.
  // A put request increments the revision of the key-value store
  // and generates one event in the event history.
  rpc Put(PutRequest) returns (PutResponse) {
      option (google.api.http) = {
        post: "/v3alpha/kv/put"
        body: "*"
    };
  }

  // DeleteRange deletes the given range from the key-value store.
  // A delete request increments the revision of the key-value store
  // and generates a delete event in the event history for every deleted key.
  rpc DeleteRange(DeleteRangeRequest) returns (DeleteRangeResponse) {
      option (google.api.http) = {
        post: "/v3alpha/kv/deleterange"
        body: "*"
    };
  }

  // Txn processes multiple requests in a single transaction.
  // A txn request increments the revision of the key-value store
  // and generates events with the same revision for every completed request.
  // It is not allowed to modify the same key several times within one txn.
  rpc Txn(TxnRequest) returns (TxnResponse) {
      option (google.api.http) = {
        post: "/v3alpha/kv/txn"
        body: "*"
    };
  }

  // Compact compacts the event history in the etcd key-value store. The key-value
  // store should be periodically compacted or the event history will continue to grow
  // indefinitely.
  rpc Compact(CompactionRequest) returns (CompactionResponse) {
      option (google.api.http) = {
        post: "/v3alpha/kv/compaction"
        body: "*"
    };
  }
}
```

分析其`rpc Put(PutRequest) returns (PutResponse)` 方法，在etcd 工程中，这个Service 编译为server/etcdserver/api/v3rpc/key.go，其中Put 的代码如下

```go
func (s *kvServer) Put(ctx context.Context, r *pb.PutRequest) (*pb.PutResponse, error) {
	if err := checkPutRequest(r); err != nil {
		return nil, err
	}

	resp, err := s.kv.Put(ctx, r)
	if err != nil {
		return nil, togRPCError(err)
	}

	s.hdr.fill(resp.Header)
	return resp, nil
}
```

当收到客户端的PutRequest 请求之后会转发到这里，然后可以看到其实现是调用了