/**
 * 2022.02.20 by xumenger
 *
 * Raft日志相关信息封装
 *
 */
#ifndef PORTAL_RAFT_LOG_H
#define PORTAL_RAFT_LOG_H

namespace portal
{
namespace raft
{
    /**
     * 日志条目类型
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
    };
}
}

#endif
