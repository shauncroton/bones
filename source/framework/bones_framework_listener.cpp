#include <zen/bones/bones_framework.hh>
#include <zen/bones/framework/bones_framework_link.hpp>
#include <strings.h>
#include <cerrno>
#include <cstring>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

zen::bones::framework_listener::framework_listener(
    const std::string &address_
)
{
    signal(
        SIGPIPE,
        SIG_IGN);

    std::promise<
        void
    > start_thread;

    auto thread_started = start_thread.get_future();

    _listener_thread = std::thread(
        &framework_listener::listener,
        this,
        address_,
        std::move(
            start_thread
        ));
    thread_started.get();
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

zen::bones::framework_listener::~framework_listener()
{
    if( _sock_fd < 0 )
        return;

    close(
        _sock_fd
    );

    _listener_thread.join();
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_listener::listener(
    std::string address_,
    std::promise<
        void
    > start_thread_
)
{
    int sock_fd = -1;

    for( auto reconnect_delay = 0s; true; ++reconnect_delay )
    {
        log_attempt( address_,
            pthread_self()) << "starting listener";

        std::this_thread::sleep_for(
            reconnect_delay
        );

        if( sock_fd != -1 )
        {
            close(
                _sock_fd
            );

            sock_fd = -1;
        }

        std::string thread_name = "listener_" + address_;

        std::replace(
            std::begin( thread_name ),
            std::end( thread_name ),
            '.',
            '_'
        );

        pthread_setname_np(
            pthread_self(),
            thread_name.c_str());

        auto it = address_.find(
            ':'
        );

        auto host = address_.substr(
            0,
            it
        );

        auto port = address_.substr(
            ++it
        );

        sock_fd = socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

        if( sock_fd < 0 )
        {
            log_error( _sock_fd,
                address_,
                errno,
                std::strerror( errno ))
            << "opening listener socket";

            continue;
        }

        struct sockaddr_in server_address;

        bzero(( char * ) &server_address,
            sizeof( server_address ));

        const auto port_no = atoi(
            port.c_str());

        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(
            port_no
        );

        const auto bind_result = bind(
            sock_fd,
            ( struct sockaddr * ) &server_address,
            sizeof( server_address ));

        if( bind_result < 0 )
        {
            log_error( bind_result,
                sock_fd,
                address_,
                errno,
                std::strerror( errno ))
            << "binding to listener socket";

            continue;
        }

        /// Make a listening listener
        const auto listen_result = listen(
            sock_fd,
            5
        );

        if( listen_result < 0 )
        {
            log_error( listen_result,
                sock_fd,
                address_,
                errno,
                std::strerror( errno ))
            << "listening on listener socket";

            continue;
        }

        _sock_fd = sock_fd;

        /// Accept an incoming link
        start_thread_.set_value();

        auto reaccept_delay = 0s;

        while( true )
        {
            log_attempt() << "waiting on listener socket for next incoming connection request";

            ++reaccept_delay;

            std::this_thread::sleep_for(
                reaccept_delay
            );

            struct sockaddr_in client_address;

            socklen_t client_length = sizeof( client_address );

            auto new_sock_fd = accept(
                _sock_fd,
                ( struct sockaddr * ) &client_address,
                &client_length
            );

            if( new_sock_fd < 0 )
            {
                log_error( new_sock_fd,
                    address_,
                    errno,
                    std::strerror( errno ))
                << "listener failed to accept new connection request";

                continue;
            }

            uint32_t handshake = 0xDEADBEEF;

            const auto send_result = send(
                new_sock_fd,
                &handshake,
                sizeof( handshake ),
                0
            );

            handshake = 0;

            if( send_result < 0 )
            {
                log_error( send_result,
                    address_,
                    handshake,
                    sizeof( handshake ),
                    errno,
                    std::strerror( errno ))
                << "sending handshake from listener to link";

                continue;
            }

            handshake = 0;

            const auto recv_result = recv(
                new_sock_fd,
                &handshake,
                sizeof( handshake ),
                0
            );

            if( recv_result < 0 )
            {
                log_error( recv_result,
                    address_,
                    handshake,
                    sizeof( handshake ),
                    errno,
                    std::strerror( errno ))
                << "receiving handshake from link to listener";

                continue;
            }

            if( handshake != 0xCAFEBABE )
            {
                log_error( recv_result,
                    address_,
                    handshake,
                    sizeof( handshake ),
                    errno,
                    std::strerror( errno ))
                << "handshake from link to listener was bad";

                continue;
            }

            auto rear_link = std::make_shared<
                framework_link
            >(
                address_,
                new_sock_fd
            );

            auto rear_coupling = std::dynamic_pointer_cast<
                framework_coupling
            >(
                rear_link
            );

            framework_dispatcher_shared front_dispatcher = framework_singleton().registry()->dispatcher(
                address_
            );

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
                    address_,
                    rear_link,
                    front_coupling
                ]()
                {
                    framework_singleton().registry()->factory(
                        address_,
                        front_coupling
                    );

                    rear_link->start_dispatcher();
                }
            );

            reaccept_delay = 0s;

            log_accept() << "next incoming connection request has been accepted on listener socket";
        }

        log_accept( address_,
            pthread_self()) << "stopping listener";

        break;
    }
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

