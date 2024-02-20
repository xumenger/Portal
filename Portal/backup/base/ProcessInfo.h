/**
 * 2022.03.16
 * 进程信息
 *
 */

#ifndef PORTAL_BASE_PROCESSINFO_H
#define PORTAL_BASE_PROCESSINFO_H

#include "./Types.h"
#include "./Timestamp.h"

#include <vector>
#include <sys/types.h>


namespace portal
{
namespace ProcessInfo
{

    pid_t pid();
    string pidString();
    uid_t uid();
    string username();
    uid_t euid();
    Timestamp startTime();
    int clockTicksPerSecond();
    int pageSize();
    bool isDebugBuild();

    string hostname();
    string procname();
    
    // read /proc/self/status
    string procStatus();

    // read /proc/self/stat
    string procStat();

    // read /proc/self/task/tid/stat
    string threadStat();

    // readlink /proc/self/exe
    string exePath();

    int openedFiles();
    int maxOpenFiles();

    struct CpuTime
    {
        double userSeconds;
        double systemSeconds;

        CpuTime(): userSeconds(0.0), systemSeconds(0.0) {  }
    };
    CpuTime cpuTime();

    int numThreads();
    std::vector<pid_t> threads();

};
};


#endif

