/**
 * 2022.02.20 by xumenger
 *
 * Raft日志相关信息封装
 *
 */
#ifndef PORTAL_RAFT_LOG_H
#define PORTAL_RAFT_LOG_H

#include <iostream>

#include "RaftRpc.h"
#include "RaftEntry.h"

using namespace std;


namespace portal
{
namespace raft
{

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

        }

    };

}
}

#endif
