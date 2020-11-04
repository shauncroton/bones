#ifndef __ZEN__BONES__FRAMEWORK_COUPLING__HPP
#define __ZEN__BONES__FRAMEWORK_COUPLING__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>
#include <functional>
#include <map>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

class zen::bones::framework_coupling
{
    using framework_coupling_weak =
    typename zen::types<
        framework_coupling
    >::weak;

    using callback_type =
    std::function<
        void(
            const framework_event_shared &,
            const framework_connection_shared &
        )
    >;

    using callbacks_type =
    std::unordered_map<
        std::string,
        callback_type
    >;

public:

    framework_coupling(
        const framework_coupling &
    ) = delete;

    framework_coupling &
    operator=(
        framework_coupling
    ) = delete;

    framework_coupling(
        framework_coupling &&
    ) = delete;

    framework_coupling &
    operator=(
        framework_coupling &&
    ) = delete;

    ~framework_coupling()
    {
        return;
    };

    explicit framework_coupling(
        framework_dispatcher_shared dispatcher_
    );

    void
    entangled(
        framework_coupling_weak entangled_
    );

    void
    dispatch(
        const std::string &tag_,
        const std::string &payload_
    );

    virtual void
    deliver(
        const std::string &tag_,
        const std::string &payload_
    );

    void
    callback(
        const std::string &name_,
        callback_type callback_
    );

    void
    callback(
        const std::string &name_
    );

    void
    ownership(
        const framework_coupling_shared &owned_
    );

    void
    ownership(
        framework_coupling *owned_
    );

    void
    ownership(
        framework_connection_shared owned_connection_
    )
    {
        _owned_connection = owned_connection_;
    }

    void
    owner(
        framework_coupling *owner_
    );

    framework_coupling *
    owner()
    {
        return _owner;
    }

    framework_dispatcher_shared
    dispatcher()
    {
        return _dispatcher;
    }

private:

    framework_dispatcher_shared _dispatcher;

    zen::types< callbacks_type >::shared _callbacks;

    framework_coupling_weak _entangled;

    framework_coupling *_owner{ nullptr };

    std::map<
        framework_coupling *,
        framework_coupling_shared
    > _ownerships;

    framework_connection_shared _owned_connection;

};

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_COUPLING__HPP
