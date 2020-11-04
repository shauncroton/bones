#ifndef __ZEN__BONES__FRAMEWORK_CONNECTION__HPP
#define __ZEN__BONES__FRAMEWORK_CONNECTION__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>
#include <zen/bones/framework/bones_framework.hpp>
#include <zen/bones/framework/bones_framework_coupling.hpp>
#include <zen/bones/framework/bones_framework_link.hpp>
#include <zen/bones/framework/bones_framework_dispatcher.hpp>
#include <functional>
#include <map>
#include <utility>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

class zen::bones::framework_connection
{
    using framework_coupling_weak =
    typename zen::types<
        framework_coupling
    >::weak;

    template<
        typename State,
        typename Session
    > using handler_type =
    std::function<
        void(
            Session &,
            const std::shared_ptr<
                State
            > &,
            const framework_event_shared &,
            const framework_connection_shared &
        )
    >;

public:

    framework_connection(
        const framework_connection &
    ) = delete;

    framework_connection &
    operator=(
        framework_connection &
    ) = delete;

    framework_connection(
        framework_connection &&
    ) = delete;

    framework_connection &
    operator=(
        framework_connection &&
    ) = delete;

    ~framework_connection();

    template<
        typename State,
        typename Session
    >
    framework_connection(
        std::string service_name_,
        const framework_coupling_shared &coupling_,
        std::weak_ptr<
            State
        > state_,
        std::weak_ptr<
            Session
        > session_
    );

    void
    dispatch(
        const std::string &tag_,
        const std::string &payload_
    ) const;

    template<
        typename ...Args
    >
    void
    dispatch(
        const std::string &tag_,
        const std::string &payload_,
        const Args ...args
    ) const;

    template<
        typename State,
        typename Session
    >
    void
    callback(
        const std::string &name_,
        void (Session::*handler_)(
            const std::shared_ptr<
                State
            > &,
            const framework_event_shared &,
            const framework_connection_shared &
        ));

    void
    callback(
        const std::string &name_
    ) const;

    framework_connection_shared
    spawn(
        const std::string &service_name_
    ) const;

    static auto &
    framework()
    {
        return framework_singleton();
    }

    const std::string &
    service_name()
    {
        return _service_name;
    }

private:

    template<
        typename State,
        typename Session
    >
    void
    callback_impl(
        const std::string &name_,
        std::weak_ptr<
            State
        > state_,
        std::weak_ptr<
            Session
        > session_,
        handler_type<
            State,
            Session
        > *handler_
    );

    template<
        typename State,
        typename Session
    >
    framework_connection_shared
    spawn_impl(
        const std::string &service_name_,
        const std::shared_ptr<
            State
        > &state_,
        const std::shared_ptr<
            Session
        > &session_
    ) const;

private:

    const std::string _service_name;

    const framework_coupling_weak _coupling;

    std::function<
        void(
            const std::string &name_,
            void *callback_
        )
    > _callback_function;

    std::function<
        framework_connection_shared(
            const std::string &service_name_
        )
    > _spawn_function;
};


///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///


template<
    typename... Args
>
void
zen::bones::framework_connection::dispatch(
    const std::string &tag_,
    const std::string &payload_,
    const Args ...args
) const
{
    dispatch(
        tag_,
        payload_

    );

    dispatch(
        tag_,
        std::forward<
            const Args
        >(
            args
        )...
    );
}


///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename State,
    typename Session
>
zen::bones::framework_connection::framework_connection(
    std::string service_name_,
    const framework_coupling_shared &coupling_,
    std::weak_ptr<
        State
    > state_,
    std::weak_ptr<
        Session
    > session_
)
    : _service_name( service_name_ )
    , _coupling(
        coupling_
    )
    , _callback_function(
        [
            this,
            state_,
            session_
        ](
            const std::string &name_,
            void *handler_
        )
        {
            using handler_type =
            handler_type<
                State,
                Session
            >;

            auto handler = static_cast<
                handler_type * >(
                handler_
            );

            this->callback_impl(
                name_,
                state_,
                session_,
                handler
            );
        }
    )
    , _spawn_function(
        [
            this,
            state_,
            session_
        ](
            const std::string &service_name_
        ) -> auto
        {
            using state_shared_type =
            typename zen::types<
                State
            >::shared;

            state_shared_type state = state_.lock();

            using session_shared_type =
            typename zen::types<
                Session
            >::shared;

            session_shared_type session = session_.lock();

            return this->spawn_impl(
                service_name_,
                state,
                session
            );
        }
    )
{
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename State,
    typename Session
>
void
zen::bones::framework_connection::callback(
    const std::string &name_,
    void(
    Session::*handler_ )(
        const std::shared_ptr<
            State
        > &,
        const framework_event_shared &,
        const framework_connection_shared &
    ))
{
    using handler_type =
    handler_type<
        State,
        Session
    >;

    handler_type handler = handler_;

    _callback_function(
        name_,
        static_cast<void *>(&handler));
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename State,
    typename Session
>
void
zen::bones::framework_connection::callback_impl(
    const std::string &name_,
    std::weak_ptr<
        State
    > state_,
    std::weak_ptr<
        Session
    > session_,
    handler_type<
        State,
        Session
    > *handler_
)
{
    auto handler = *handler_;

    std::shared_ptr<
        State
    > state = state_.lock();

    if( !state )
        return;

    std::shared_ptr<
        Session
    > session = session_.lock();

    if( !session )
        return;

    auto callback = [
        this,
        state,
        session,
        handler
    ](
        const framework_event_shared &event_,
        const framework_connection_shared &connection_
    ) -> void
    {
        handler(
            *session,
            state,
            event_,
            connection_
        );
    };

    auto coupling = _coupling.lock();

    if( !coupling )
        return;

    coupling->callback(
        name_,
        callback
    );
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename State,
    typename Session
>
zen::bones::framework_connection_shared
zen::bones::framework_connection::spawn_impl(
    const std::string &service_name_,
    const std::shared_ptr<
        State
    > &state_,
    const std::shared_ptr<
        Session
    > &session_
) const
{
    using state_weak_type =
    typename zen::types<
        State
    >::weak;

    using session_weak_type =
    typename zen::types<
        Session
    >::weak;

    auto coupling = _coupling.lock();

    if( !coupling )
        return nullptr;

    auto rear_owner = coupling->owner();

    auto rear_dispatcher = coupling->dispatcher();

    auto rear_coupling = std::make_shared<
        framework_coupling
    >(
        rear_dispatcher
    );

    if( rear_owner )
        rear_owner->ownership(
            rear_coupling
        );

    auto rear_connection = std::make_shared<
        framework_connection
    >(
        service_name_,
        rear_coupling,
        state_weak_type(
            state_
        ),
        session_weak_type(
            session_
        ));

    rear_coupling->ownership(
        rear_connection
    );

    if( service_name_.find(
        ":"
    ) == std::string::npos )
    {
        framework_dispatcher_shared front_dispatcher = framework_singleton().registry()->dispatcher(
            service_name_
        );

        if( !front_dispatcher )
            return nullptr;

        auto front_coupling = std::make_shared<
            framework_coupling
        >(
            front_dispatcher
        );

        rear_coupling->ownership(
            front_coupling
        );

        rear_coupling->entangled(
            front_coupling
        );

        front_coupling->entangled(
            rear_coupling
        );

        front_dispatcher->enqueue(
            [
                service_name_,
                front_coupling
            ]()
            {
                framework_singleton().registry()->factory(
                    service_name_,
                    front_coupling
                );
            }
        );
    }
    else
    {
        auto front_link = std::make_shared<
            framework_link
        >(
            service_name_
        );

        auto front_coupling = std::dynamic_pointer_cast<
            framework_coupling
        >(
            front_link
        );

        rear_coupling->ownership(
            front_coupling
        );

        rear_coupling->entangled(
            front_coupling
        );

        front_coupling->entangled(
            rear_coupling
        );

        front_link->start_dispatcher();
    }

    return rear_connection;
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_CONNECTION__HPP
