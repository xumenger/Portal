/**
 * 2022.02.20 by xumenger
 *
 * Raft日志快照
 *
 */
#ifndef PORTAL_RAFT_SNAPSHOT_H
#define PORTAL_RAFT_SNAPSHOT_H

#include <iostream>
#include <set>
#include <list>

#include "RaftNode.h"

using namespace std;


namespace portal
{
namespace raft
{


enum AppendEntriesStatus { FAILED, SUCCESS };

enum InstallSnapshotStateName { ILLEGAL_INSTALL_SNAPSHOT_RPC, INSTALLING, INSTALLED };

struct InstallSnapshotState
{
    InstallSnapshotStateName stateName;
    set<NodeEndpoint> lastConfig;
};


class SnapshotChunk
{
    public:
        unsigned char *bytes;
        bool lastChunk;

        SnapshotChunk(unsigned char *bytes, bool lastChunk)
        {
            this.bytes = bytes;
            this.lastChunk = lastChunk;
        }

        virtual ~SnapshotChunk()
        {

        }
};


class Snapshot
{
    int getLastIncludedIndex()
    {

    }

    int getLastIncludedTerm()
    {

    }

    // 从指定的偏移offset 开始读区指定长度length 的快照区
    SnapshotChunk *readData(int offset, int length)
    {

    }

    void close()
    {

    }
};


}
}

#endif
