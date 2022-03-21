#pragma once

#include <timeline_data_provider.hpp>

namespace tl
{

    class data_provider_sample : public timeline_data_provider
    {
    public:
        data_provider_sample(std::vector<std::vector<const_timeline_item_ptr>> const& data);
        data_provider_sample(std::vector<std::vector<const_timeline_item_ptr>>&& data);

        size_t                layer_count() const override;
        const_item_iterator_t layer_begin(size_t layer) const override;
        const_item_iterator_t layer_end(size_t layer) const override;

    private:
        std::vector<std::vector<const_timeline_item_ptr>> _data;
    };

} // namespace tl