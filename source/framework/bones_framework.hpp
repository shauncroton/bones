#ifndef __ZEN__BONES__FRAMEWORK__HPP
#define __ZEN__BONES__FRAMEWORK__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>
#include <zen/bones/framework/bones_framework_configuration.hpp>
//#include <zen/bones/framework/bones_framework_link.hpp>
//#include <zen/bones/framework/bones_framework_coupling.hpp>
//#include <zen/bones/framework/bones_framework_connection.hpp>
#include <zen/bones/framework/bones_framework_registry.hpp>
#include <zen/bones/framework/bones_framework_dispatcher.hpp>
#include <zen/bones/framework/bones_framework_coupling.hpp>
#include <memory>
#include <sys/types.h>
#include <unistd.h>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

class zen::bones::framework
{
public:

    framework(
        const framework &
    ) = delete;

    framework &
    operator=(
        framework
    ) = delete;

    framework(
        framework &&
    ) = delete;

    framework &
    operator=(
        framework &&
    ) = delete;

    ~framework() = default;

    framework() = default;

    framework_configuration_shared &
    configuration()
    {
        return _configuration;
    }

    const framework_configuration_shared &
    configuration() const
    {
        return _configuration;
    }

    void
    configuration(
        const framework_configuration_shared &configuration_
    )
    {
        _configuration = configuration_;
    }

    framework_registry_shared &
    registry()
    {
        return _registry;
    }

    const framework_registry_shared &
    registry() const
    {
        return _registry;
    }

    void
    registry(
        const framework_registry_shared &registry_
    )
    {
        _registry = registry_;
    }

    template<
        typename State,
        typename ...Args
    >
    framework_resource_shared<
        State
    >
    make_resource(
        const std::string &name_,
        Args ...args
    ) const;

    template<
        typename Session,
        typename Resource,
        typename ...Args
    >
    framework_service_shared<
        Session,
        Resource
    >
    make_service(
        const std::string &name_,
        const std::shared_ptr<
            Resource
        > &resource_

    ) const;

//    template<
//        typename Session,
//        typename State
//    >
//    framework_connection_shared
//    make_connection(
//        framework_coupling *rear_owner_,
//        const framework_dispatcher_shared &dispatcher_,
//        const typename zen::types<
//            State
//        >::shared &state_,
//        const typename zen::types<
//            Session
//        >::shared &session_,
//        const std::string &service_name_
//    ) const;

    template<
        typename Application
    >
    static int
    main_function(
        int argc,
        char *argv[]
    );

private:

    framework_configuration_shared _configuration;
    framework_registry_shared _registry;

};

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename State,
    typename ...Args
>
zen::bones::framework_resource_shared<
    State
>
zen::bones::framework::make_resource(
    const std::string &name_,
    Args ...args
) const
{
    return std::make_shared<
        framework_resource<
            State
        >
    >(
        name_,
        std::forward<
            Args
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
    typename Session,
    typename Resource,
    typename ...Args
>
zen::bones::framework_service_shared<
    Session,
    Resource
>
zen::bones::framework::make_service(
    const std::string &name_,
    const std::shared_ptr<
        Resource
    > &resource_
) const
{
    auto service = std::make_shared<
        framework_service<
            Session,
            Resource
        >
    >(
        name_,
        resource_
    );

    auto session_factory_function = [ service ](
        const framework_coupling_shared &front_coupling_
    )
    {
        service->factory(
            front_coupling_
        );
    };

    framework_singleton().registry()->insert(
        name_,
        resource_->dispatcher(),
        session_factory_function
    );

    service->start();

    return service;
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

//template<
//    typename Session,
//    typename State
//>
//zen::bones::framework_connection_shared
//zen::bones::framework::make_connection(
//    framework_coupling *rear_owner_,
//    const framework_dispatcher_shared &rear_dispatcher_,
//    const typename zen::types<
//        State
//    >::shared &state_,
//    const typename zen::types<
//        Session
//    >::shared &session_,
//    const std::string &service_name_
//) const
//{
//
//
//    framework_dispatcher_shared
//        front_dispatcher =
//        framework_singleton().depository()->dispatcher(
//            service_name_
//        );
//
//    auto
//        rear_coupling =
//        std::make_shared<
//            framework_coupling
//        >(
//            service_name_,
//            rear_dispatcher_
//        );
//
//    if( rear_owner_ )
//        rear_owner_->ownership(
//            rear_coupling
//        );
//
//    auto
//        rear_connection =
//        std::make_shared<
//            framework_connection
//        >(
//            rear_coupling,
//            state_,
//            session_
//        );
//
//    auto
//        front_coupling =
//        std::make_shared<
//            framework_coupling
//        >(
//            rear_coupling,
//            front_dispatcher
//        );
//
//    rear_coupling->ownership(
//        front_coupling
//    );
//
//    rear_coupling->entangled(
//        front_coupling
//    );
//
//    front_coupling->entangled(
//        rear_coupling
//    );
//
//    front_dispatcher->enqueue(
//        [
//            service_name_,
//            front_coupling
//        ]()
//        {
//
//
//            framework_singleton()
//
//                .depository()->
//
//                factory(
//                service_name_,
//                front_coupling
//
//            );
//        }
//    );
//
//    return rear_connection;
//};

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

template<
    typename Application
>
int
zen::bones::framework::main_function(
    int argc,
    char *argv[]
)
{
    try
    {
        auto &framework = framework_singleton();

        auto configuration = std::make_shared<
            framework_configuration
        >(
            argc,
            argv
        );

        framework.configuration(
            configuration
        );

        auto registry = std::make_shared<
            framework_registry
        >();

        framework.registry(
            registry
        );

        {
            auto application_resource = framework.make_resource<
                int
            >(
                "behaviour"
            );

            framework.make_service<
                Application
            >(
                "application_startup",
                application_resource
            );
        }

        auto rear_coupling = std::make_shared<
            framework_coupling
        >(
            nullptr
        );

        auto front_coupling = std::make_shared<
            framework_coupling
        >(
            nullptr
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

        framework_singleton().registry()->factory(
            "application_startup",
            front_coupling
        );

        while( true )
        {
            std::this_thread::yield();
            std::this_thread::sleep_for(
                std::chrono::seconds(
                    1
                ));
        }

        framework.registry()->clear();
    }
    catch( const std::exception &ex )
    {
        log_error()
        << "exception caught: " << ex.what();
    }
    catch( ... )
    {
        log_fatality()
        << "exception caught: unknown ";

        abort();
    }

    return 1;
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK__HPP
