/**
 * 2022.02.20 by xumenger
 *
 * Raft日志相关信息封装
 *
 */
#ifndef PORTAL_RAFT_LOG_H
#define PORTAL_RAFT_LOG_H

#include <iostream>

using namespace std;


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
        public:
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


    /**
     * No-Op 条目
     */
    struct NoOpEntry : public Entry
    {
        public:
            NoOpEntry(int index, int term)
                : Entry(KIND_NO_OP, index, term)
            {

            }
    };

    
    /**
     * 通用日志条目
     */
    struct GeneralEntry : public Entry
    {
        public:
            GeneralEntry(int index, int term, unsigned char *commandBytes)
                : Entry(KIND_GENERAL, index, term)
            {
                this.commandBytes = commandBytes;
            }
    };

}
}

#endif
