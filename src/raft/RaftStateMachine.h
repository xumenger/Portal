/**
 * 2022.02.20 by xumenger
 *
 * Raft状态机
 *
 */
#ifndef PORTAL_RAFT_STATEMACHINE_H
#define PORTAL_RAFT_STATEMACHINE_H

#include <iostream>
#include <set>

#include "RaftNode.h"
#include "RaftSnapshot.h"

using namespace std;


namespace portal
{
namespace raft
{
    class RaftStateMachine
    {
        public:
            int getLastApplied()
            {

            }

            void applyLog(int index, 
                          int term, 
                          unsigned char *commangBytes, 
                          int firstLogIndex, 
                          set<NodeEndpoint> lastGroupConfig)
            {

            }

            void advanceLastApplied(int index)
            {

            }

            void onReadIndexReached(string requestId, int readIndex)
            {

            }

            void generateSnapshot()
            {

            }

            void applySnapshot(Snapshot snapshot)
            {

            }

            void shutdown()
            {

            }
    };
}
}

#endif
