#pragma once

#include <list>

#include "book.hpp"

namespace uebernotes {

class Storage {
public:
    const std::list<NormalBook>& getNormalBooks() const;
    void addNormalBook(NormalBook&& book);

private:
    std::list<NormalBook> _normalBooks;
};

} // namespace uebernotes
