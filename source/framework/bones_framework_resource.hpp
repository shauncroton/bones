#ifndef __ZEN__BONES__FRAMEWORK_RESOURCE__HPP
#define __ZEN__BONES__FRAMEWORK_RESOURCE__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>
#include <zen/bones/framework/bones_framework_resource.hpp>
#include <zen/bones/framework/bones_framework_event.hpp>
#include <zen/bones/framework/bones_framework_dispatcher.hpp>
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

template<
    typename State
>
class zen::bones::framework_resource
{
public:

    using state_type =
    State;

    framework_resource(
        const framework_resource &
    ) = delete;

    framework_resource &
    operator=(
        framework_resource
    ) = delete;

    framework_resource(
        framework_resource &&
    ) = delete;

    framework_resource &
    operator=(
        framework_resource &&
    ) = delete;

    ~framework_resource() = default;

    template<
        typename ...Args
    >
    framework_resource(
        std::string name_,
        Args ...args
    );

    const auto &
    name() const
    {
        return _name;
    }

    using event_callback_function =
    std::function<
        void(
            const framework_event_shared &
        )
    >;

    void
    enqueue(
        const event_callback_function &callback_,
        const framework_event_shared &event_
    );

    typename zen::types<
        State
    >::shared
    state()
    {
        return _state;
    }

    framework_dispatcher_shared
    dispatcher()
    {
        return _dispatcher;
    }

private:

    const std::string _name;

    typename zen::types<
        State
    >::shared _state;

    zen::bones::framework_dispatcher_shared _dispatcher;
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename State
>
template<
    typename ...Args
>
zen::bones::framework_resource<
    State
>::framework_resource(
    std::string name_,
    Args ...args
)
    : _name(
    std::move(
        name_
    ))
    , _state(
        std::make_shared<
            State
        >(
            std::forward<
                Args
            >(
                args
            )...
        ))
    , _dispatcher(
        std::make_shared<
            framework_dispatcher
        >( _name ))
{
    return;
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename State
>
void
zen::bones::framework_resource<
    State
>::enqueue(
    const event_callback_function &callback_,
    const framework_event_shared &event_
)
{
    auto &state = _state;
    auto enqueued_callback = [
        state,
        event_,
        callback_
    ]()
    {
        callback_(
            event_
        );
    };

    _dispatcher->enqueue(
        enqueued_callback
    );
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_RESOURCE__HPP
