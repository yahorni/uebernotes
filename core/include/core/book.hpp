#pragma once

#include <string>
#include <list>

#include "note.hpp"

namespace uebernotes {

using BookID = uint64_t;
using NoteCollection = std::list<Note>;

class BaseBook {
public:
    BaseBook(const BookID& bookID, const std::string& name);
    virtual ~BaseBook() = default;

    const BookID& getID() const;
    const std::string& getName() const;

    virtual const NoteCollection& getNotes() const = 0;


protected:
    const BookID _id;
    std::string _name;
};

class NormalBook : public BaseBook {
public:
    using BaseBook::BaseBook;

    const NoteCollection& getNotes() const override;

    void addNote(const Note& note);
    void addNote(Note&& note);

private:
    NoteCollection _notes;
};

}; // namespace uebernotes
