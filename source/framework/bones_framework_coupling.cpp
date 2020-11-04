#include <zen/bones/framework/bones_framework_coupling.hpp>
#include <zen/bones/bones_framework.hh>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///


zen::bones::framework_coupling::framework_coupling(
    framework_dispatcher_shared dispatcher_
)
    : _dispatcher(
    std::move(
        dispatcher_
    ))
    , _callbacks(
        std::make_shared< callbacks_type >())
{
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_coupling::entangled(
    framework_coupling_weak entangled_
)
{
    _entangled = std::move(
        entangled_
    );
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_coupling::dispatch(
    const std::string &tag_,
    const std::string &payload_
)
{
    std::this_thread::yield();

    auto entangled_coupling = _entangled.lock();
    if( !entangled_coupling )
    {
        log_attempt()
        << "entangled not available";
        return;
    }
    entangled_coupling->deliver(
        tag_,
        payload_
    );
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

inline void
zen::bones::framework_coupling::deliver(
    const std::string &tag_,
    const std::string &payload_
)
{
    auto &connection = _owned_connection;

    auto &callbacks = _callbacks;

    auto call_execute_function = [
        tag_,
        payload_,
        connection,
        callbacks,
        this
    ]()
    {
        auto it = callbacks->find(
            tag_
        );

        if( it == callbacks->end())
        {
            log_attempt()
            << "###### No callback connected for event \"" << tag_ << "\"\n" << payload_ << "\n";

            return;
        }

        auto session_callback = it->second;

        auto event = std::make_shared<
            framework_event
        >(
            tag_,
            payload_
        );

        session_callback(
            event,
            _owned_connection
        );
    };

    _dispatcher->enqueue(
        call_execute_function
    );
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_coupling::callback(
    const std::string &name_,
    callback_type callback_
)
{
    callback(
        name_
    );

    _callbacks->emplace(
        name_,
        callback_
    );
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_coupling::callback(
    const std::string &name_
)
{
    const auto it = _callbacks->find(
        name_
    );

    if( it == _callbacks->end())
        return;

    _callbacks->erase(
        it
    );
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_coupling::ownership(
    const framework_coupling_shared &owned_
)
{
    const auto raw = owned_.get();

    _ownerships.emplace(
        raw,
        owned_
    );

    owned_->owner(
        this
    );
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_coupling::ownership(
    framework_coupling *owned_
)
{
    auto it = _ownerships.find(
        owned_
    );

    if( it == _ownerships.end())
        return;

    _ownerships.erase(
        it
    );
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_coupling::owner(
    framework_coupling *owner_
)
{
    if( _owner )
        _owner->ownership(
            _owner
        );

    _owner = owner_;
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////
