#pragma once

#include <prototypes.hpp>
#include <vector>

namespace tl
{

    class timeline_data_provider
    {
    public:
        using const_item_iterator_t = std::vector<const_timeline_item_ptr>::const_iterator;

    public:
        virtual size_t                layer_count() const             = 0;
        virtual const_item_iterator_t layer_begin(size_t layer) const = 0;
        virtual const_item_iterator_t layer_end(size_t layer) const   = 0;
    };

} // namespace tl