#include "../include/util.h"

namespace jyl
{
    pid_t getThreadID() { return syscall(SYS_gettid); }

    uint32_t getFiberID() { return 0; }
}