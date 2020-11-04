#ifndef __ZEN__BONES__FRAMEWORK_CONFIGURATION__HPP
#define __ZEN__BONES__FRAMEWORK_CONFIGURATION__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

class zen::bones::framework_configuration
{
public:

    framework_configuration(
        const framework_configuration &
    ) = delete;

    framework_configuration &
    operator=(
        framework_configuration
    ) = delete;

    framework_configuration(
        framework_configuration &&
    ) = delete;

    framework_configuration &
    operator=(
        framework_configuration &&
    ) = delete;

    ~framework_configuration() = default;

    framework_configuration(
        int argc_,
        char *argv_[]
    );

//private:

    const int _argc;
    const char *const *const _argv;
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_CONFIGURATION__HPP
