#ifndef __ZEN__BONES__FRAMEWORK_LINK__HPP
#define __ZEN__BONES__FRAMEWORK_LINK__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>
#include <zen/bones/framework/bones_framework_coupling.hpp>
#include <zen/bones/framework/bones_framework_resource.hpp>
#include <zen/bones/framework/bones_framework_registry.hpp>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <sys/socket.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <string>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

class zen::bones::framework_link
    : public framework_coupling
        , public std::enable_shared_from_this<
        framework_link
    >
{
public:

    framework_link(
        const framework_link &
    ) = delete;

    framework_link &
    operator=(
        framework_link
    ) = delete;

    framework_link(
        framework_link &&
    ) = delete;

    framework_link &
    operator=(
        framework_link &&
    ) = delete;

    ~framework_link()
    {
        close(
            _sock_fd
        );
    }

    framework_link(
        std::string address_,
        int sock_fd_
    );

    explicit framework_link(
        std::string address_
    );

    void
    start_dispatcher();

    void
    deliver(
        const std::string &tag_,
        const std::string &payload_
    );

private:

    void
    dispatcher(
        framework_link_shared keep_me_alive_
    );

private:

    const std::string _address;

    int _sock_fd{ -1 };
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

//auto
//zen::bones::make_link(
//    const ::std::string &from_name_,
//    const ::std::string &to_name_
//)
//{
//
//};

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_LINK__HPP
