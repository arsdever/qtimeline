#pragma once

#include <memory>

namespace tl
{

    class timeline_data_provider;
    using timeline_data_provider_ptr  = std::shared_ptr<timeline_data_provider>;
    using timeline_data_provider_wptr = std::weak_ptr<timeline_data_provider>;
    using timeline_data_provider_uptr = std::unique_ptr<timeline_data_provider>;

    class timeline_item;
    using timeline_item_ptr        = std::shared_ptr<timeline_item>;
    using timeline_item_wptr       = std::weak_ptr<timeline_item>;
    using timeline_item_uptr       = std::unique_ptr<timeline_item>;
    using const_timeline_item_ptr  = std::shared_ptr<const timeline_item>;
    using const_timeline_item_wptr = std::weak_ptr<const timeline_item>;
    using const_timeline_item_uptr = std::unique_ptr<const timeline_item>;

} // namespace tl