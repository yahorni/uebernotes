#include "storage.hpp"

namespace uebernotes {
    const std::list<NormalBook>& Storage::getNormalBooks() const { return _normalBooks; }

    void Storage::addNormalBook(NormalBook&& book) {
        _normalBooks.push_back(std::move(book));
    }
}
