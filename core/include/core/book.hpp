#pragma once

#include "core/comparator.hpp"
#include "core/types.hpp"

#include <memory>
#include <string>
#include <unordered_set>

namespace core {

class Storage;

struct BookInfo {
    BookID id;
    std::string name;

    explicit BookInfo(std::string&& name = "");
    BookInfo(BookID id, std::string&& name);

    const std::string& getName() const;
};

using BookPtr = std::shared_ptr<BookInfo>;

using BooksCache = std::unordered_set<BookPtr, SharedPtrExtension::HashID, SharedPtrExtension::CompareID>;

};  // namespace core
