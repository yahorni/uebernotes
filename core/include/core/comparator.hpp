#pragma once

#include "core/logger.hpp"

#include <functional>
#include <memory>

namespace core {

template<typename T>
concept HasFieldID = requires { T::id; };

template<typename T>
concept HasMethodGetName = requires(T t) {
    { t.getName() } -> std::convertible_to<std::string_view>;
};

struct SharedPtrExtension {
    struct HashID {
        template<typename T>
            requires HasFieldID<T>
        std::size_t operator()(const std::shared_ptr<T>& p) const {
            return std::hash<decltype(p->id)>()(p->id);
        }
    };
    template<typename Op = std::less<void>>
    struct CompareID {
        template<typename T>
            requires HasFieldID<T>
        bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
            return Op{}(lhs->id, rhs->id);
        }
    };
    template<typename Op = std::less<void>>
    struct CompareName {
        template<typename T>
            requires HasMethodGetName<T>
        bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
            return Op{}(lhs->getName(), rhs->getName());
        }
    };
};

}  // namespace core
