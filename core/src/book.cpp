#include "book.hpp"

namespace uebernotes {

// BaseBook
BaseBook::BaseBook(const BookID& bookID, const std::string& name)
    : _id(bookID)
    , _name(name)
{}

const BookID& BaseBook::getID() const { return _id; }
const std::string& BaseBook::getName() const { return _name; }

// NormalBook
const NoteCollection& NormalBook::getNotes() const { return _notes; }

void NormalBook::addNote(const Note& note) {
    _notes.push_back(note);
}

void NormalBook::addNote(Note&& note) {
    _notes.push_back(std::move(note));
}

} // namespace uebernotes
