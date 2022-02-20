/**
 * 2022.02.19 by xumenger
 *
 * Raft 算法中的角色建模
 *
 */

#ifndef PORTAL_RAFT_ROLE_HPP
#define PORTAL_RAFT_ROLE_HPP

#include <iostream>

#include "RaftNode.h"

using namespace std;


namespace portal
{
namespace raft
{
    /**
    * 角色枚举
    */
    enum RoleName { FOLLOWER, CANDIDATE, LEADER };

    /**
    * 角色状态
    */
    struct RoleState 
    {
        RoleName roleName;      // 角色名称
        int term;               // 任期号
        int votesCount = -1;    // 作为Candidate时，获取多少票数
        NodeId *votedFor;        // 作为Follower时，投票给谁
        NodeId *leaderId;        // 领导者Id
    };


    /**
     * 角色父类
     */
    virtual class AbstractNodeRole 
    {
        private:
            RoleName name;

        protected:
            int term;

        public:
            AbstractNodeRole(RoleName name, int term)
            {
                this.name = name;
                this.term = term;
            }

            virtual ~AbstractNodeRole()
            {
            }

            RoleName getName()
            {
                return this.name;
            }

            int getTerm()
            {
                return this.term;
            }

            // 虚函数：获取LeaderId
            virtual const NodeId &getLeaderId(NodeId &selfId);

            // 虚函数：获取角色状态
            virtual const RoleState &getState() = 0;

            // 虚函数：取消定时任务
            virtual void cancelTimeoutOrTask() = 0;
    };


    /**
    * 领导者
    */
    class LeaderNodeRole : public AbstractNodeRole
    {
        private:
            // 日志复制任务
            // LogReplicationTask logReplicationTask;
            //
        public:
            LeaderNodeRole(int term/*, Logreplicationtask logReplicationtask*/)
                : AbstractNodeRole(LEADER, term)
            {
                //this.logReplicationtask = logReplicationtask;
            }

            virtual ~LeaderNodeRole()
            {
            }

            const NodeId &getLeaderId(NodeId &selfId) override
            {
                return selfId;
            }

            const RoleState &getState() override
            {
                return null;  // TODO
            }

            void cancelTimeoutOrTask() override
            {
                // TODO
            }
    };

    /**
     * 候选者
     */
    class CandidateNodeRole : public AbstractNodeRole
    {
        private:
            int votesCount;
            // 定时选举任务
            // ElectionTimeout electionTimeout;
            
        public:
            CandidateNodeRole(int term/*, ElectionTimeout electionTimeout*/)
            {
                CandidateNodeRole(term, 1);
            }

            CandidateNodeRole(int term, int votesCount/*, Electiontimeout electionTimeout*/)
                :AbstractNodeRole(CANDIDATE, term)
            {
                this.votesCount = votesCount;
                //this.electiontimeout = electiontimeout;
            }

            virtual ~CandidateNodeRole()
            {
            }

            int getVotesCount()
            {
                return this.votesCount;
            }

            const NodeId &getLeaderId(NodeId &selfId) override
            {
                return null;
            }

            const RoleState &getState() override
            {
                return null;
            }

            void cancelTimeoutOrTask() override
            {
                // TODO
            }
    };

    /**
     * 跟随者
     */
    class FollowerNodeRole : public AbstractNodeRole
    {
        private:
            NodeId votedFor;                    // 投过票的节点，可能为空
            NodeId leaderId;                    // 当前leader 节点ID，可能为空
            int preVotesCount;                  //
            // ElectionTimeout electionTimeout; // 选举超时
            long lastHeartbeat;

        public:
            FollowerNodeRole(int term, NodeId)
    };

}
}

#endif

