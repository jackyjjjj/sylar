#pragma once

#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>

namespace jyl
{
    pid_t getThreadID();
    uint32_t getFiberID();
}