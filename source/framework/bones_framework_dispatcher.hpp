#ifndef __ZEN__BONES__FRAMEWORK_DISPATCHER__HPP
#define __ZEN__BONES__FRAMEWORK_DISPATCHER__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <iostream>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

class zen::bones::framework_dispatcher
{
public:

    framework_dispatcher(
        const framework_dispatcher &
    ) = delete;

    framework_dispatcher &
    operator=(
        framework_dispatcher
    ) = delete;

    framework_dispatcher(
        framework_dispatcher &&
    ) = delete;

    framework_dispatcher &
    operator=(
        framework_dispatcher &&
    ) = delete;

    ~framework_dispatcher();

    explicit framework_dispatcher(
        const std::string &name_
    );

    using enqueued_callback_function =
    const std::function<
        void()
    >;

    using enqueued_callback_function_shared =
    zen::types<
        enqueued_callback_function
    >::shared;

    void
    enqueue(
        const enqueued_callback_function &callback_function
    );


private:

    void
    process(
        std::string name_,
        std::promise<
            void
        > start_thread
    );

    std::thread _processing_thread;
    std::mutex _processing_queue_mutex;
    std::condition_variable _processing_queue_condition_variable;
    std::queue<
        enqueued_callback_function_shared
    > _processing_queue;
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_DISPATCHER__HPP
