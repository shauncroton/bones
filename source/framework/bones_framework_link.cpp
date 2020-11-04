#include <zen/bones/framework/bones_framework_link.hpp>
#include <zen/bones/bones_framework.hh>
#include <zen/parts/utility/string/parts_utility_string_functions.hpp>
#include <strings.h>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

zen::bones::framework_link::framework_link(
    std::string address_,
    int sock_fd_
)
    : framework_coupling(
    nullptr
)
    , _address( std::move( address_ ))
    , _sock_fd( sock_fd_ )
{
    signal(
        SIGPIPE,
        SIG_IGN);
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

zen::bones::framework_link::framework_link(
    std::string address_
)
    : framework_coupling( nullptr )
    , _address( std::move( address_ ))
{
    int sock_fd = -1;

    for( auto reconnect_delay = 0s; true; ++reconnect_delay )
    {
        log_attempt( _address ) << "connecting link to remote host";

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

        const auto host = utility::network_connection_string(
            _address
        ).host();

        const auto port = utility::network_connection_string(
            _address
        ).port();

        sock_fd = socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

        if( sock_fd < 0 )
        {
            log_error( sock_fd,
                address_,
                errno,
                std::strerror( errno ))
            << "opening link socket";

            continue;
        }

        struct hostent *server = gethostbyname(
            host.c_str());

        if( server == nullptr )
        {
            log_error( _address,
                errno,
                std::strerror( errno ))
            << "no such host found for link";

            continue;
        }

        struct sockaddr_in serv_addr;

        bzero(( char * ) &serv_addr,
            sizeof( serv_addr ));

        serv_addr.sin_family = AF_INET;

        bcopy(
            server->h_addr,
            ( char * ) &serv_addr.sin_addr.s_addr,
            server->h_length
        );

        serv_addr.sin_port = htons(
            port
        );

        const auto connect_result = connect(
            sock_fd,
            ( struct sockaddr * ) &serv_addr,
            sizeof( serv_addr ));

        if( connect_result < 0 )
        {
            log_error( connect_result,
                sock_fd,
                address_,
                errno,
                std::strerror( errno ))
            << "connecting link to remote host";

            continue;
        }

        uint32_t handshake = 0xCAFEBABE;

        const auto send_result = send(
            sock_fd,
            &handshake,
            sizeof( handshake ),
            0
        );

        if( send_result < 0 )
        {
            log_error( send_result,
                address_,
                handshake,
                sizeof( handshake ),
                errno,
                std::strerror( errno ))
            << "sending handshake from link to listener";

            continue;
        }

        handshake = 0;

        const auto recv_result = recv(
            sock_fd,
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
            << "receiving handshake from listener to link";

            continue;
        }

        if( handshake != 0xDEADBEEF )
        {
            log_error( recv_result,
                address_,
                handshake,
                sizeof( handshake ),
                std::strerror( errno ))
            << "handshake from listener to link was bad";

            continue;
        }

        _sock_fd = sock_fd;

        log_accept( _address ) << "connected link to remote host";

        break;
    }
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_link::deliver(
    const std::string &tag_,
    const std::string &payload_
)
{
    std::this_thread::yield();

    size_t header_size = tag_.size();

    {
        const auto send_result = send(
            _sock_fd,
            &header_size,
            sizeof( header_size ),
            0
        );

        if( send_result < 0 )
        {
            log_error( send_result,
                _address,
                header_size,
                sizeof( header_size ),
                errno,
                std::strerror( errno ))
            << "unable to deliver as error sending incoming payload body to remote";

            close( _sock_fd );

            dispatch(
                "disconnected",
                _address
            );

            return;
        }

        if( send_result == 0 )
        {
            log_error( send_result,
                _address,
                header_size,
                sizeof( header_size ),
                errno,
                std::strerror( errno ))
            << "unable to deliver as remote has disconnected from us";

            close( _sock_fd );

            dispatch(
                "disconnected",
                _address
            );

            return;
        }
    }

    {
        const auto send_result = send(
            _sock_fd,
            tag_.data(),
            header_size,
            0
        );

        if( send_result < 0 )
        {
            log_error( send_result,
                _address,
                tag_.data(),
                header_size,
                errno,
                std::strerror( errno ))
            << "unable to deliver as error receiving incoming payload body from remote";

            close( _sock_fd );

            dispatch(
                "disconnected",
                _address
            );

            return;
        }

        if( send_result == 0 )
        {
            log_error( send_result,
                _address,
                tag_.data(),
                header_size,
                errno,
                std::strerror( errno ))
            << "unable to deliver as remote has disconnected from us";

            close( _sock_fd );

            dispatch(
                "disconnected",
                _address
            );

            return;
        }
    }

    size_t payload_size = payload_.size();

    {
        const auto send_result = send(

            _sock_fd,
            &payload_size,
            sizeof( payload_size ),
            0
        );

        if( send_result < 0 )
        {
            log_error( send_result,
                _address,
                payload_size,
                sizeof( payload_size ),
                errno,
                std::strerror( errno ))
            << "unable to deliver as error receiving incoming payload body from remote";

            close( _sock_fd );

            dispatch(
                "disconnected",
                _address
            );

            return;
        }

        if( send_result == 0 )
        {
            log_error( send_result,
                _address,
                payload_size,
                sizeof( payload_size ),
                errno,
                std::strerror( errno ))
            << "unable to deliver as remote has disconnected from us";

            close( _sock_fd );

            dispatch(
                "disconnected",
                _address
            );

            return;
        }
    }

    {
        const auto send_result = send(
            _sock_fd,
            payload_.data(),
            payload_.size(),
            0
        );

        if( send_result < 0 )
        {
            log_error( send_result,
                _address,
                payload_.data(),
                payload_.size(),
                errno,
                std::strerror( errno ))
            << "unable to deliver as error receiving incoming payload body from remote";

            close( _sock_fd );

            dispatch(
                "disconnected",
                _address
            );

            return;
        }

        if( send_result == 0 )
        {
            log_error( send_result,
                _address,
                payload_.data(),
                payload_.size(),
                errno,
                std::strerror( errno ))
            << "unable to deliver as remote has disconnected from us";

            close( _sock_fd );

            dispatch(
                "disconnected",
                _address
            );

            close( _sock_fd );

            dispatch(
                "disconnected",
                _address
            );

            return;
        }
    }
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_link::start_dispatcher()
{
    std::thread(
        &zen::bones::framework_link::dispatcher,
        this,
        shared_from_this()).detach();
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

void
zen::bones::framework_link::dispatcher(
    framework_link_shared keep_me_alive_
)
{
    std::string thread_name = "d_" + _address;

    std::replace(
        std::begin( thread_name ),
        std::end( thread_name ),
        '.',
        '_'
    );

    std::replace(
        std::begin( thread_name ),
        std::end( thread_name ),
        ':',
        '_'
    );

    log_attempt( thread_name,
        pthread_self()) << "starting dispatcher";

    pthread_setname_np(
        pthread_self(),
        thread_name.c_str());

    char *buffer = 0;
    size_t length = 0;

    auto check_buffer = [
        &buffer,
        &length
    ](
        size_t needed
    )
    {
        if( needed > length )
        {
            delete[] buffer;
            buffer = new char[needed];
            length = needed;
        }
    };

    keep_me_alive_->dispatch(
        "connected",
        _address
    );

    while( true )
    {
        size_t header_size;

        {
            const auto recv_result = recv(
                _sock_fd,
                &header_size,
                sizeof( header_size ),
                0
            );

            if( recv_result < 0 )
            {
                log_error( recv_result,
                    _address,
                    header_size,
                    sizeof( header_size ),
                    errno,
                    std::strerror( errno ))
                << "unable to dispatch as error receiving incoming header size from remote";

                break;
            }

            if( recv_result == 0 )
            {
                log_error( recv_result,
                    _address,
                    header_size,
                    sizeof( header_size ),
                    errno,
                    std::strerror( errno ))
                << "unable to dispatch as remote has disconnected from us";

                break;
            }
        }

        check_buffer(
            header_size
        );

        if( header_size )
        {
            const auto recv_result = recv(
                _sock_fd,
                buffer,
                header_size,
                0
            );

            if( recv_result < 0 )
            {
                log_error( recv_result,
                    _address,
                    buffer,
                    header_size,
                    errno,
                    std::strerror( errno ))
                << "unable to dispatch as error receiving incoming header body from remote";

                break;
            }

            if( recv_result == 0 )
            {
                log_error( recv_result,
                    _address,
                    header_size,
                    sizeof( header_size ),
                    errno,
                    std::strerror( errno ))
                << "unable to dispatch as remote has disconnected from us";

                break;
            }
        }

        std::string tag(
            buffer,
            header_size
        );

        size_t payload_size;

        {
            const auto recv_result = recv(
                _sock_fd,
                &payload_size,
                sizeof( payload_size ),
                0
            );

            if( recv_result < 0 )
            {
                log_error( recv_result,
                    _address,
                    payload_size,
                    sizeof( payload_size ),
                    errno,
                    std::strerror( errno ))
                << "unable to dispatch as error receiving incoming payload size from remote";

                break;
            }

            if( recv_result == 0 )
            {
                log_error( recv_result,
                    _address,
                    header_size,
                    sizeof( header_size ),
                    errno,
                    std::strerror( errno ))
                << "unable to dispatch as remote has disconnected from us";

                break;
            }
        }

        check_buffer(
            payload_size
        );

        if( payload_size )
        {
            const auto recv_result = recv(
                _sock_fd,
                buffer,
                payload_size,
                0
            );

            if( recv_result < 0 )
            {
                log_error( recv_result,
                    _address,
                    buffer,
                    payload_size,
                    errno,
                    std::strerror( errno ))
                << "unable to dispatch as error receiving incoming payload body from remote";

                break;
            }

            if( recv_result == 0 )
            {
                log_error( recv_result,
                    _address,
                    header_size,
                    sizeof( header_size ),
                    errno,
                    std::strerror( errno ))
                << "unable to dispatch as remote has disconnected from us";

                break;
            }
        }

        std::string payload(
            buffer,
            payload_size
        );

        std::this_thread::yield();

        keep_me_alive_->dispatch(
            tag,
            payload
        );
    }

    close( _sock_fd );

    keep_me_alive_->dispatch(
        "disconnected",
        _address
    );

    delete[] buffer;

    log_accept( thread_name,
        pthread_self()) << "stopping dispatcher";
}

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

