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

当收到客户端的PutRequest 请求之后会转发到这里，然后可以看到其实现是调用了`s.kv.Put` 按照Raft 的协议，在具体的实现中肯定是做了转发Follwer、持久化等处理

`server/etcdserver/v3_server.go:` 的相关实现代码如下

```go
func (s *EtcdServer) Put(ctx context.Context, r *pb.PutRequest) (*pb.PutResponse, error) {
	ctx = context.WithValue(ctx, traceutil.StartTimeKey, time.Now())
	// 把PutRequest 封装为InternalRaftRequest 调用raftRequest() 方法
	resp, err := s.raftRequest(ctx, pb.InternalRaftRequest{Put: r})
	if err != nil {
		return nil, err
	}
	return resp.(*pb.PutResponse), nil
}

func (s *EtcdServer) raftRequest(ctx context.Context, r pb.InternalRaftRequest) (proto.Message, error) {
	return s.raftRequestOnce(ctx, r)
}

func (s *EtcdServer) raftRequestOnce(ctx context.Context, r pb.InternalRaftRequest) (proto.Message, error) {
	result, err := s.processInternalRaftRequestOnce(ctx, r)
	if err != nil {
		return nil, err
	}
	if result.Err != nil {
		return nil, result.Err
	}
	if startTime, ok := ctx.Value(traceutil.StartTimeKey).(time.Time); ok && result.Trace != nil {
		applyStart := result.Trace.GetStartTime()
		// The trace object is created in toApply. Here reset the start time to trace
		// the raft request time by the difference between the request start time
		// and toApply start time
		result.Trace.SetStartTime(startTime)
		result.Trace.InsertStep(0, applyStart, "process raft request")
		result.Trace.LogIfLong(traceThreshold)
	}
	return result.Resp, nil
}

// 核心处理方法
func (s *EtcdServer) processInternalRaftRequestOnce(ctx context.Context, r pb.InternalRaftRequest) (*apply2.Result, error) {
	// Raft 相关的一些Index，其具体作用？
	ai := s.getAppliedIndex()
	ci := s.getCommittedIndex()
	if ci > ai+maxGapBetweenApplyAndCommitIndex {
		return nil, errors.ErrTooManyRequests
	}

	// 创建RequestHeader，s.reqIDGen.Next() 的作用是？
	r.Header = &pb.RequestHeader{
		ID: s.reqIDGen.Next(),
	}

	// check authinfo if it is not InternalAuthenticateRequest
	if r.Authenticate == nil {
		authInfo, err := s.AuthInfoFromCtx(ctx)
		if err != nil {
			return nil, err
		}
		if authInfo != nil {
			r.Header.Username = authInfo.Username
			r.Header.AuthRevision = authInfo.Revision
		}
	}

	// 对请求进行序列化处理
	data, err := r.Marshal()
	if err != nil {
		return nil, err
	}

	if len(data) > int(s.Cfg.MaxRequestBytes) {
		return nil, errors.ErrRequestTooLarge
	}

	id := r.ID
	if id == 0 {
		id = r.Header.ID
	}
	ch := s.w.Register(id)

	cctx, cancel := context.WithTimeout(ctx, s.Cfg.ReqTimeout())
	defer cancel()

	start := time.Now()
	// 调用raftNode.Propose() 方法，在这里实现数据同步和持久化处理
	err = s.r.Propose(cctx, data)
	if err != nil {
		proposalsFailed.Inc()
		s.w.Trigger(id, nil) // GC wait
		return nil, err
	}
	proposalsPending.Inc()
	defer proposalsPending.Dec()

	select {
	case x := <-ch:
		return x.(*apply2.Result), nil
	case <-cctx.Done():
		proposalsFailed.Inc()
		s.w.Trigger(id, nil) // GC wait
		return nil, s.parseProposeCtxErr(cctx.Err(), start)
	case <-s.done:
		return nil, errors.ErrStopped
	}
}
```

核心的方法就是`node.go: raftNode.Propose()` 方法

```go
// Propose proposes that data be appended to the log. Note that proposals can be lost without
// notice, therefore it is user's job to ensure proposal retries.
func (n *node) Propose(ctx context.Context, data []byte) error {
	// 封装为pb.Message，类型为MsgProp
	// 消息的定义参考raft.proto
	return n.stepWait(ctx, pb.Message{Type: pb.MsgProp, Entries: []pb.Entry{{Data: data}}})
}

func (n *node) stepWait(ctx context.Context, m pb.Message) error {
	return n.stepWithWaitOption(ctx, m, true)
}

// Step advances the state machine using msgs. The ctx.Err() will be returned,
// if any.
func (n *node) stepWithWaitOption(ctx context.Context, m pb.Message, wait bool) error {
	if m.Type != pb.MsgProp {
		select {
		case n.recvc <- m:
			return nil
		case <-ctx.Done():
			return ctx.Err()
		case <-n.done:
			return ErrStopped
		}
	}
	ch := n.propc
	pm := msgWithResult{m: m}
	if wait {
		pm.result = make(chan error, 1)
	}
	select {
	case ch <- pm:
		if !wait {
			return nil
		}
	case <-ctx.Done():
		return ctx.Err()
	case <-n.done:
		return ErrStopped
	}
	select {
	case err := <-pm.result:
		if err != nil {
			return err
		}
	case <-ctx.Done():
		return ctx.Err()
	case <-n.done:
		return ErrStopped
	}
	return nil
}
```

这里实现了把消息写到propc 这个Channel 里面，接下来就找从propc channel 获取消息的代码，找到了`node.go: func (n *node) run()` 方法，其中有这样的处理逻辑：

```go
...

		select {
		// TODO: maybe buffer the config propose if there exists one (the way
		// described in raft dissertation)
		// Currently it is dropped in Step silently.
		case pm := <-propc:
			m := pm.m
			m.From = r.id
			err := r.Step(m)
			if pm.result != nil {
				pm.result <- err
				close(pm.result)
			}

...
```

进一步调用到`raft.go: func (r *raft) Step(m pb.Message) error`，这里面有针对Raft 协议处理的核心逻辑，我们先只挑出发送MsgProp 类型消息的处理，该方法中没有处理逻辑，实际调用的是step() 方法，对于Leader 就是`raft.go: func stepLeader(r *raft, m pb.Message) error`

```go
...

	case pb.MsgProp:
		if len(m.Entries) == 0 {
			r.logger.Panicf("%x stepped empty MsgProp", r.id)
		}
		if r.prs.Progress[r.id] == nil {
			// If we are not currently a member of the range (i.e. this node
			// was removed from the configuration while serving as leader),
			// drop any new proposals.
			return ErrProposalDropped
		}
		if r.leadTransferee != None {
			r.logger.Debugf("%x [term %d] transfer leadership to %x is in progress; dropping proposal", r.id, r.Term, r.leadTransferee)
			return ErrProposalDropped
		}

		for i := range m.Entries {
			e := &m.Entries[i]
			var cc pb.ConfChangeI
			if e.Type == pb.EntryConfChange {
				var ccc pb.ConfChange
				if err := ccc.Unmarshal(e.Data); err != nil {
					panic(err)
				}
				cc = ccc
			} else if e.Type == pb.EntryConfChangeV2 {
				var ccc pb.ConfChangeV2
				if err := ccc.Unmarshal(e.Data); err != nil {
					panic(err)
				}
				cc = ccc
			}
			if cc != nil {
				alreadyPending := r.pendingConfIndex > r.raftLog.applied
				alreadyJoint := len(r.prs.Config.Voters[1]) > 0
				wantsLeaveJoint := len(cc.AsV2().Changes) == 0

				var refused string
				if alreadyPending {
					refused = fmt.Sprintf("possible unapplied conf change at index %d (applied to %d)", r.pendingConfIndex, r.raftLog.applied)
				} else if alreadyJoint && !wantsLeaveJoint {
					refused = "must transition out of joint config first"
				} else if !alreadyJoint && wantsLeaveJoint {
					refused = "not in joint state; refusing empty conf change"
				}

				if refused != "" {
					r.logger.Infof("%x ignoring conf change %v at config %s: %s", r.id, cc, r.prs.Config, refused)
					m.Entries[i] = pb.Entry{Type: pb.EntryNormal}
				} else {
					r.pendingConfIndex = r.raftLog.lastIndex() + uint64(i) + 1
				}
			}
		}

		// 先本地追加，这里面是关于raftLog 的处理
		if !r.appendEntry(m.Entries...) {
			return ErrProposalDropped
		}
		// 在广播到集群中其他节点，这里是发送的逻辑
		r.bcastAppend()
		return nil

...
```

继续在raft.go 中看相关源码：

```go
// bcastAppend sends RPC, with entries to all peers that are not up-to-date
// according to the progress recorded in r.prs.
func (r *raft) bcastAppend() {
	r.prs.Visit(func(id uint64, _ *tracker.Progress) {
		if id == r.id {
			return
		}
		r.sendAppend(id)
	})
}

// sendAppend sends an append RPC with new entries (if any) and the
// current commit index to the given peer.
func (r *raft) sendAppend(to uint64) {
	r.maybeSendAppend(to, true)
}

// maybeSendAppend sends an append RPC with new entries to the given peer,
// if necessary. Returns true if a message was sent. The sendIfEmpty
// argument controls whether messages with no entries will be sent
// ("empty" messages are useful to convey updated Commit indexes, but
// are undesirable when we're sending multiple messages in a batch).
func (r *raft) maybeSendAppend(to uint64, sendIfEmpty bool) bool {
	...
	...

	// Send the actual MsgApp otherwise, and update the progress accordingly.
	next := pr.Next // save Next for later, as the progress update can change it
	if err := pr.UpdateOnEntriesSend(len(ents), payloadsSize(ents), next); err != nil {
		r.logger.Panicf("%x: %v", r.id, err)
	}
	r.send(pb.Message{
		To:      to,
		Type:    pb.MsgApp,
		Index:   next - 1,
		LogTerm: term,
		Entries: ents,
		Commit:  r.raftLog.committed,
	})
	return true
}
```

调用到`progress.go: func (pr *Progress) UpdateOnEntriesSend(entries int, bytes, nextIndex uint64) error` 完成发送

```
// UpdateOnEntriesSend updates the progress on the given number of consecutive
// entries being sent in a MsgApp, with the given total bytes size, appended at
// and after the given log index.
func (pr *Progress) UpdateOnEntriesSend(entries int, bytes, nextIndex uint64) error {
	switch pr.State {
	case StateReplicate:
		if entries > 0 {
			last := nextIndex + uint64(entries) - 1
			pr.OptimisticUpdate(last)
			pr.Inflights.Add(last, bytes)
		}
		// If this message overflows the in-flights tracker, or it was already full,
		// consider this message being a probe, so that the flow is paused.
		pr.MsgAppFlowPaused = pr.Inflights.Full()
	case StateProbe:
		// TODO(pavelkalinnikov): this condition captures the previous behaviour,
		// but we should set MsgAppFlowPaused unconditionally for simplicity, because any
		// MsgApp in StateProbe is a probe, not only non-empty ones.
		if entries > 0 {
			pr.MsgAppFlowPaused = true
		}
	default:
		return fmt.Errorf("sending append in unhandled state %s", pr.State)
	}
	return nil
}
```

这里面还是没有发送，只是将数据添加到Inflights 中：`pr.Inflights.Add(last, bytes)`

有几个问题：

1. 到这里还是没有找到具体是哪个代码调用Socket 的send 方法把消息发送出去的！
2. etcdServer 应该有两个端口，一个是面向客户端的，一个是面向集群其他节点的，这里是不是一个切入点？
3. etcd 对外的服务基于gRPC 实现，etcd 内部集群间的通信是怎么实现的？


## Follower 从接收请求到最终的处理流程

