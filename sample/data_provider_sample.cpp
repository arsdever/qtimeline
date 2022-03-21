#include <vector>

#include "data_provider_sample.hpp"

namespace tl
{
    data_provider_sample::data_provider_sample(std::vector<std::vector<const_timeline_item_ptr>> const& data)
        : _data { data }
    {
    }

    data_provider_sample::data_provider_sample(std::vector<std::vector<const_timeline_item_ptr>>&& data)
        : _data { std::move(data) }
    {
    }

    size_t data_provider_sample::layer_count() const { return _data.size(); }

    data_provider_sample::const_item_iterator_t data_provider_sample::layer_begin(size_t layer) const
    {
        return _data[layer].begin();
    }

    data_provider_sample::const_item_iterator_t data_provider_sample::layer_end(size_t layer) const
    {
        return _data[layer].end();
    }

} // namespace tl