#pragma once

#include <functional>
#include <chrono>

#include <pico/async_context.h>

class CallbackTimer {

    public:

        typedef std::function<void(CallbackTimer&)> Callback; 

        CallbackTimer();

        void sheduleInMilliseconds(uint32_t milliseconds, Callback callback);

        template< class Rep, class Period >
        void sheduleIn(const std::chrono::duration<Rep, Period>& duration, std::function<void(CallbackTimer&)> callback) {
            sheduleInMilliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() , callback);    
        }

    private:
        Callback mActiveCallback;
        async_at_time_worker_t mAtTimeWorker;

};