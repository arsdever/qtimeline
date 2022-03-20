#pragma once

#include <chrono>
#include <timeline_item.hpp>

namespace tl
{
    class item : public timeline_item
    {
    public:
        item(std::string_view                      name,
             std::chrono::steady_clock::time_point begin,
             std::chrono::steady_clock::time_point end)
            : _name { name }
            , _start { begin }
            , _end { end }
        {
        }
        std::string_view                      name() const override { return _name; }
        std::chrono::steady_clock::time_point start() const override { return _start; }
        std::chrono::steady_clock::time_point end() const override { return _end; }

    private:
        std::string                           _name;
        std::chrono::steady_clock::time_point _start;
        std::chrono::steady_clock::time_point _end;
    };
} // namespace tl