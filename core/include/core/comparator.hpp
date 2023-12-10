#pragma once

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
        std::size_t operator()(std::shared_ptr<T> const& p) const {
            return std::hash<decltype(p->id)>()(p->id);
        }
    };
    struct CompareID {
        template<typename T>
            requires HasFieldID<T>
        bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
            return lhs->id < rhs->id;
        }
    };
    struct CompareNameAsc {
        template<typename T>
            requires HasMethodGetName<T>
        bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
            return lhs->getName() < rhs->getName();
        }
    };
    struct CompareNameDesc {
        template<typename T>
            requires HasMethodGetName<T>
        bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
            return lhs->getName() > rhs->getName();
        }
    };
};

}  // namespace core
