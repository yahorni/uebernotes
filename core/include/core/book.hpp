#pragma once

#include "core/comparator.hpp"
#include "core/types.hpp"

#include <memory>
#include <string>
#include <unordered_set>

namespace core {

class Storage;

struct Book {
    BookID id;
    std::string name;

    explicit Book(std::string&& name = "");
    Book(BookID id, std::string&& name);

    const std::string& getName() const;
};

using BookPtr = std::shared_ptr<Book>;

using BooksCache = std::unordered_set<BookPtr, SharedPtrExtension::HashID, SharedPtrExtension::CompareID>;

};  // namespace core
