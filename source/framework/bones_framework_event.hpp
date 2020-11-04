#ifndef __ZEN__BONES__FRAMEWORK_EVENT__HPP
#define __ZEN__BONES__FRAMEWORK_EVENT__HPP

#include <zen/application.hh>
#include <zen/bones/bones_framework.h>
#include <unordered_map>

///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///

class zen::bones::framework_event
{
public:

    framework_event(
        const framework_event &
    ) = delete;

    framework_event &
    operator=(
        framework_event
    ) = delete;

    framework_event(
        framework_event &&
    ) = delete;

    framework_event &
    operator=(
        framework_event &&
    ) = delete;

    ~framework_event() = default;

    framework_event(
        std::string tag_,
        std::string payload_
    );

    const std::string &
    tag() const
    {
        return _tag;
    }

    const std::string &
    payload() const
    {
        return _payload;
    }

private:

    const std::string _tag;
    const std::string _payload;
};

///
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ZEN__BONES__FRAMEWORK_EVENT__HPP
