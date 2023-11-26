#pragma once

#include "core/comparator.hpp"
#include "core/types.hpp"

#include <memory>
#include <set>
#include <string>

namespace core {

class Storage;

struct BookInfo {
    BookID id;
    std::string name;

    explicit BookInfo(std::string&& name = "");
    BookInfo(BookID id, std::string&& name);
};

class Book {
public:
    explicit Book(std::shared_ptr<BookInfo> book);

    const std::string& getName() const;

private:
    std::shared_ptr<BookInfo> _book;
};

using BooksCache = std::set<std::shared_ptr<BookInfo>, SharedPtrWithID::Compare>;

};  // namespace core
