#include <zen/bones/framework/bones_framework_dispatcher.hpp>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

zen::bones::framework_dispatcher::framework_dispatcher(
    const std::string &name_
)
{
    std::promise<
        void
    > start_thread;

    auto thread_started = start_thread.get_future();

    _processing_thread = std::thread(
        &zen::bones::framework_dispatcher::process,
        this,
        name_,
        std::move(
            start_thread
        ));

    std::this_thread::yield();

    thread_started.get();
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

zen::bones::framework_dispatcher::~framework_dispatcher()
{
    {
        std::lock_guard<
            std::mutex
        > g(
            _processing_queue_mutex
        );

        _processing_queue.emplace(
            nullptr
        );
    }
    _processing_queue_condition_variable.notify_one();

    _processing_thread.join();
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_dispatcher::enqueue(
    const enqueued_callback_function &callback_function
)
{
    auto callback_function_shared = std::make_shared<
        const enqueued_callback_function
    >(
        callback_function
    );

    {
        std::lock_guard<
            std::mutex
        > g(
            _processing_queue_mutex
        );

        _processing_queue.emplace(
            callback_function_shared
        );
    }

    _processing_queue_condition_variable.notify_one();

    std::this_thread::sleep_for(
        1ms
    );
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_dispatcher::process(
    std::string name_,
    std::promise<
        void
    > start_thread
)
{
    log_attempt( name_,
        pthread_self()) << "starting dispatcher";

    pthread_setname_np(
        pthread_self(),
        name_.c_str());

    start_thread.set_value();

    while( true )
    {
        std::unique_lock<
            std::mutex
        > lock(
            _processing_queue_mutex
        );

        if( _processing_queue.empty())
            _processing_queue_condition_variable.wait(
                lock
            );

        if( _processing_queue.empty())
            continue;

        auto enqueued_callback = _processing_queue.front();

        _processing_queue.pop();

        lock.unlock();

        if( !enqueued_callback )
            break;

        ( *enqueued_callback )();
    };

    log_accept( name_,
        pthread_self()) << "stopping dispatcher";
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

