#pragma once

#include "core/comparator.hpp"
#include "core/note.hpp"
#include "core/types.hpp"

#include <list>
#include <memory>
#include <optional>
#include <set>
#include <string>

namespace core {

class Storage;

struct BookInfo {
    // TODO(same for NoteInfo): if the fields are not constant that means they can be changed?
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
