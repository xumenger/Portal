/**
 * 2022.02.20 by xumenger
 *
 * Raft日志相关信息封装
 *
 */
#ifndef PORTAL_RAFT_ENTRY_H
#define PORTAL_RAFT_ENTRY_H

#include <iostream>

#include "RaftRpc.h"

using namespace std;


namespace portal
{
namespace raft
{
    /**
     * 日志条目类型：
     * 1. 心跳
     * 2. 通用日志
     * 3. 新增集群节点
     * 4. 移除集群节点
     */
    enum EntryKind { KIND_NO_OP = 0, KIND_GENERAL, KIND_ADD_NODE, KIND_REMOVE_NODE };


    /**
     * 日志条目元信息
     */
    struct EntryMeta
    {
        EntryKind kind;
        int index;
        int term;
    };


    /**
     * 日志条目
     */
    struct Entry
    {
        EntryMeta meta;                 // 日志条目元信息
        unsigned char *commandBytes;    // C++ 中没有byte 类型

        Entry(EntryKind kind, int index, int term)
        {
            this.meta.kind = kind;
            this.meta.index = index;
            this.meta.term = term;

            this.commandBytes = NULL;
        }

        const EntryMeta &getEntryMeta()
        {
            return this.meta;
        }

        const unsigned char *getCommandBytes()
        {
            return this.commandBytes;
        }
    };

}
}

#endif
