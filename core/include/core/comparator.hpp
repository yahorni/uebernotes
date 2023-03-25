#pragma once

#include <memory>

namespace core {

struct SharedPtrWithID {
    struct Hash {
        template <typename T>
        std::size_t operator()(std::shared_ptr<T> const& p) const {
            return std::hash<decltype(p->id)>()(p->id);
        }
    };
    struct Compare {
        template <typename T>
        bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
            return lhs->id < rhs->id;
        }
    };
};

}  // namespace core
