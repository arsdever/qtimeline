#pragma once

template <typename T>
struct ptr_t
{
#ifdef USE_SMART_PTR
    using type = std::shared_ptr<T>;
#else
    using type = T*;
#endif
};