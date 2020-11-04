#ifndef __ZEN__BONES__FRAMEWORK_SERVICE__HPP
#define __ZEN__BONES__FRAMEWORK_SERVICE__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.hh>
#include <zen/bones/framework/bones_framework_link.hpp>
#include <zen/bones/framework/bones_framework_coupling.hpp>
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
    typename Session,
    typename Resource
>
class zen::bones::framework_service
{
    using resource_shared_type =
    typename zen::types<
        Resource
    >::shared;

    using ownership_token_type =
    std::function<
        void()
    >;

public:

    framework_service(
        const framework_service &
    ) = delete;

    framework_service &
    operator=(
        framework_service
    ) = delete;

    framework_service(
        framework_service &&
    ) = delete;

    framework_service &
    operator=(
        framework_service &&
    ) = delete;

    ~framework_service() = default;

    framework_service(
        std::string name_,
        resource_shared_type resource_
    );

    const auto &
    name() const
    {
        return _name;
    }

    void
    start();

    void
    stop();

    void
    factory(
        const framework_coupling_shared &front_coupling_
    );

private:

    const std::string _name;

    typename zen::types<
        Resource
    >::shared _resource;

    framework_listener_shared _listener;

};

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename Session,
    typename Resource
>
zen::bones::framework_service<
    Session,
    Resource
>::framework_service(
    std::string name_,
    resource_shared_type resource_
)
    : _name(
    std::move(
        name_
    ))
    , _resource(
        std::move(
            resource_
        ))
{
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename Session,
    typename Resource
>
void
zen::bones::framework_service<
    Session,
    Resource
>::start()
{
    if( _name.find(
        ':'
    ) != std::string::npos )
    {
        _listener = std::make_shared<
            framework_listener
        >(
            _name
        );
    }
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename Session,
    typename Resource
>
void
zen::bones::framework_service<
    Session,
    Resource
>::stop()
{
    _listener.reset();

    framework_singleton().registry()->remove(
        _name
    );
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename Session,
    typename Resource
>
void
zen::bones::framework_service<
    Session,
    Resource
>::factory(
    const framework_coupling_shared &front_coupling_
)
{
    using state_weak_type =
    typename zen::types<
        typename Resource::state_type
    >::weak;

    using session_weak_type =
    typename zen::types<
        Session
    >::weak;

    auto state = _resource->state();

    auto session = std::make_shared<
        Session
    >();

    auto weak_state = state_weak_type(
        state
    );

    auto weak_session = session_weak_type(
        session
    );

    auto front_connection = std::make_shared<
        framework_connection
    >(
        std::string(), // TODO:
        front_coupling_,
        weak_state,
        weak_session
    );

    front_coupling_->ownership(
        front_connection
    );

    session->on_event_session_initialize(
        state,
        front_connection
    );
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_SERVICE__HPP
