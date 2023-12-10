#include "core/book.hpp"

namespace core {

Book::Book(std::string&& name)
    : Book(0, std::move(name)) {}

Book::Book(BookID id, std::string&& name)
    : id(id),
      name(std::move(name)) {}

const std::string& Book::getName() const { return name; }

}  // namespace core
