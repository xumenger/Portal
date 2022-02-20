/**
 * 2022.02.20 by xumenger
 *
 * Raft RPC 通信数据结构定义
 */
#ifndef PORTAL_RAFT_RPC_H
#define PORTAL_RAFT_RPC_H

#include <iostream>

#include "RaftNode.h"
#include "RaftEntry.h"

using namespace std;

namespace portal
{
namespace raft
{
    struct PreVoteRpc
    {
        int term;
        int lastLogIndex = 0;
        int lastLogTerm = 0;
    };
    struct PreVoteResult
    {
        int term;
        bool voteGranted;
    };


    struct RequestVoteRpc
    {
        int term;
        NodeId *candidateId;
        int lastLogIndex;
        int lastLogTerm;
    };
    struct RequestVoteResult
    {
        int term;
        bool voteGranted;
    };


    struct AppendEntriesRpc
    {
        string messageId;
        int term;
        NodeId *leaderId;
        int prevLogIndex;
    }
}
}

#endif
