#include "System.hpp"

#include <pico/unique_id.h>
#include <mutex>

char sSystemId[8 + 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1] = "bt-pico-";
std::once_flag sSystemIdFlag;

const char* systemId()
{
    std::call_once(sSystemIdFlag, [](){
        pico_get_unique_board_id_string(sSystemId + 8, sizeof(sSystemId) - 8);
    });
    return sSystemId;
}