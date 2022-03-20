#pragma once

namespace tl
{

    class timeline_item
    {
    public:
        virtual std::string_view                      name() const  = 0;
        virtual std::chrono::steady_clock::time_point start() const = 0;
        virtual std::chrono::steady_clock::time_point end() const   = 0;
    };

    class const_timeline_item_iterator;

} // namespace tl