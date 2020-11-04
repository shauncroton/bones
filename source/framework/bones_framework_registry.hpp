#ifndef __ZEN__BONES__FRAMEWORK_REGISTRY__HPP
#define __ZEN__BONES__FRAMEWORK_REGISTRY__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <functional>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

class zen::bones::framework_registry
{
    using factory_function_type =
    std::function<
        void(
            const framework_coupling_shared &coupling_
        )
    >;

    using service_map_type =
    std::unordered_map<
        std::string,
        std::pair<
            framework_dispatcher_shared,
            factory_function_type
        >
    >;

public:

    framework_registry(
        const framework_registry &
    ) = delete;

    framework_registry &
    operator=(
        framework_registry
    ) = delete;

    framework_registry(
        framework_registry &&
    ) = delete;

    framework_registry &
    operator=(
        framework_registry &&
    ) = delete;

    ~framework_registry() = default;

    framework_registry() = default;

    void
    insert(
        const std::string &name_,
        const framework_dispatcher_shared &dispatcher_,
        const factory_function_type &factory
    );

    void
    remove(
        const std::string &name_
    );

    void
    clear();

    framework_dispatcher_shared
    dispatcher(
        const std::string &name_
    );

    void
    factory(
        const std::string &service_name_,
        const framework_coupling_shared &front_coupling_
    );

private:

    std::mutex _service_maps_mutex;

    service_map_type _services;
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_REGISTRY__HPP
