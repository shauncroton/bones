#ifndef __ZEN__BONES__FRAMEWORK__H
#define __ZEN__BONES__FRAMEWORK__H

#include <zen.h>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

namespace zen
{
    namespace bones
    {
        class framework;

        using framework_shared =
        zen::types<
            framework
        >::shared;

        framework &
        framework_singleton();


        class framework_configuration;

        using framework_configuration_shared =
        zen::types<
            framework_configuration
        >::shared;


        class framework_connection;

        using framework_connection_shared =
        typename zen::types<
            framework_connection
        >::shared;


        class framework_coupling;

        using framework_coupling_shared =
        typename zen::types<
            framework_coupling
        >::shared;


        class framework_coupling_owner;

        using framework_coupling_owner_shared =
        typename zen::types<
            framework_coupling_owner
        >::shared;


        class framework_dispatcher;

        using framework_dispatcher_shared =
        zen::types<
            framework_dispatcher
        >::shared;


        class framework_event;

        using framework_event_shared =
        zen::types<
            framework_event
        >::shared;


        class framework_link;

        using framework_link_shared =
        zen::types<
            framework_link
        >::shared;


        class framework_listener;

        using framework_listener_shared =
        zen::types<
            framework_listener
        >::shared;


        class framework_registry;

        using framework_registry_shared =
        zen::types<
            framework_registry
        >::shared;


        template<
            typename State
        >
        class framework_resource;

        template<
            typename State
        > using framework_resource_shared =
        typename zen::types<
            framework_resource<
                State
            >
        >::shared;


        template<
            typename Session,
            typename Resource
        >
        class framework_service;

        template<
            typename Session,
            typename Resource
        > using framework_service_shared =
        typename zen::types<
            framework_service<
                Session,
                Resource
            >
        >::shared;


//        template<
//            typename Resource
//        >
//        framework_coupling_shared<
//            Resource
//        >
//        make_coupling(
//            const std::string &name_,
//            const typename zen::types<
//                Resource
//            >::shared &resource_
//        );

//        framework_link_shared
//        make_link(
//            const std::string &to_name_,
//            const std::string &from_name_
//        );
    }
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK__H
