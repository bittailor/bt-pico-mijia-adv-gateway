#include "CallbackTimer.hpp"

#include <pico/cyw43_arch.h>

CallbackTimer::CallbackTimer()
: mAtTimeWorker{
    .do_work = [](async_context_t *context, struct async_work_on_timeout *timeout) {
        CallbackTimer* self = reinterpret_cast<CallbackTimer*>(timeout->user_data);
        if(self->mActiveCallback) {
            auto callback = self->mActiveCallback;
            self->mActiveCallback = nullptr;
            callback(*self);
        }
    },
    .user_data = this
}
{
}

void CallbackTimer::sheduleInMilliseconds(uint32_t milliseconds, std::function<void(CallbackTimer &)> callback)
{
    mActiveCallback = callback;
    async_context_add_at_time_worker_in_ms(cyw43_arch_async_context(), &mAtTimeWorker, milliseconds);
}