#include "core/book.hpp"

namespace core {

BookInfo::BookInfo(std::string&& name)
    : BookInfo(0, std::move(name)) {}

BookInfo::BookInfo(BookID id, std::string&& name)
    : id(id),
      name(std::move(name)) {}

const std::string& BookInfo::getName() const { return name; }

}  // namespace core
