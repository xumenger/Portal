/**
 * 2022.02.20 by xumenger
 *
 * Raft日志相关信息封装
 *
 */
#ifndef PORTAL_RAFT_LOG_H
#define PORTAL_RAFT_LOG_H

#include <iostream>
#include <set>
#include <list>

#include "RaftRpc.h"
#include "RaftEntry.h"
#include "RaftSnapshot.h"
#include "RaftStateMachine.h"

using namespace std;


namespace portal
{
namespace raft
{

    enum AppendEntriesStatus { FAILED, SUCCESS };


    /**
     * 文件日志
     */
    class FileLog
    {
        static string FILE_NAME_SNAPSHOT = "service.ss";
        static string FILE_NAME_ENTRIES = "entries.bin";
        static string FILE_NAME_ENTRY_OFFSET_INDEX = "entries.idx";

        static string DIR_NAME_GENERATING = "generating";
        static string DIR_NAME_INSTALLING = "installing";

        // 获取日志序列中最后一个日志条目的元信息
        EntryMeta *getLastEntryMeta()
        {
            return NULL;
        }

        // 创建AppendEntriesRpc 请求
        AppendEntriesRpc *createAppendEntriesRpc(int term, NodeId *selfId, int nextIndex, int maxEntries)
        {
            return NULL;
        }

        // 创建InstallSnapshotRpc 请求
        InstallSnapshotRpc *createInstallSnapshotRpc(int term, NodeId *selfId, int offset, int length)
        {
            return NULL;
        }

        InstallSnapshotRpc *createInstallSnapshotRpc(int term, int lastIncludedIndex, NodeId *selfId, int offset, int length)
        {
            return NULL;
        }

        // 获取集群节点信息
        set<NodeEndpoint> getLastGroup()
        {
            // TODO
        }


        int getNextIndex()
        {
            return 0;
        }

        int getCommitIndex()
        {
            return 0;
        }

        bool isNewerThan(int lastLogIndex, int lastLogTerm)
        {

        }


        // 生成追加No-Op 条目
        LogEntry *appendEntry(int term)
        {

        }

        // 生成追加General 条目
        LogEntry *appendEntry(int term, unsigned char *command)
        {

        }

        // 生成追加AddNode 条目
        LogEntry *appendEntry(int term, set<NodeEndpoint> nodeEndpoints, NodeEndpoint nodeToAdd)
        {

        }

        // 生成追加RemoveNode 条目
        LogEntry *appendEntry(int term, set<NodeEndpoint> nodeEndpoints, NodeEndpoint nodeToRemove)
        {

        }


        // 向日志序列中追加来自Leader 的日志条目
        AppendEntriesStatus appendEntriesFromLeader(int prevLogIndex, int prevLogTerm, List)
        {

        }

        // 安装快照
        InstallSnapshotState *installSnapshot(InstallSnapshotRpc *rpc)
        {

        }

        // 生成快照
        void snapshotGenerated(int lastIncludedIndex)
        {

        }


        // 设置状态机
        void setStateMachine(RaftStateMachine *stateMachine)
        {

        }

        // 获取状态机
        RaftStateMachine *getStateMachine()
        {

        }

        // 关闭文件日志
        void close()
        {

        }

    };

}
}

#endif
