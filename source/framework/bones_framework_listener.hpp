#ifndef __ZEN__BONES__FRAMEWORK_LISTENER__HPP
#define __ZEN__BONES__FRAMEWORK_LISTENER__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>
#include <functional>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <future>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

class zen::bones::framework_listener
{
public: //SCTP

//    using session_factory_function    =
//    std::function<
//        framework_shared(
//            const framework_connection_shared &
//        )
//    >;

public:

    framework_listener(
        const framework_listener &
    ) = delete;

    framework_listener &
    operator=(
        framework_listener &
    ) = delete;

    framework_listener(
        framework_listener &&
    ) = delete;

    framework_listener &
    operator=(
        framework_listener &&
    ) = delete;

    ~framework_listener();

    explicit framework_listener(
        const std::string &address_
    );

private:

    void
    listener(
        std::string address_,
        std::promise<
            void
        > start_thread_
    );

private:

    std::thread _listener_thread;
    int _sock_fd{ -1 };
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_LISTENER__HPP
