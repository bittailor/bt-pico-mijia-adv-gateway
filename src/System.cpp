#include "System.hpp"

#include <mutex>
#include <malloc.h>

#include <hardware/flash.h>
#include <pico/unique_id.h>

extern "C" char __flash_binary_end;
extern "C" char __StackLimit;
extern "C" char __bss_end__;

namespace System {
namespace {
    char sSystemId[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];
    std::once_flag sSystemIdFlag;
}

const char* id()
{
    std::call_once(sSystemIdFlag, [](){
        pico_get_unique_board_id_string(sSystemId, sizeof(sSystemId));
    });
    return sSystemId;
}

size_t codeSize()
{
    return ((size_t)(&__flash_binary_end) - XIP_BASE);
}

size_t freeHeap()
{
    return totalHeap() - usedHeap();
}
size_t usedHeap()
{
    return mallinfo().uordblks;
}

size_t totalHeap()
{
    return &__StackLimit  - &__bss_end__;
}

}