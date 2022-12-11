#include "note.hpp"

namespace uebernotes {

Note::Note(const NoteID& noteID, const std::string& initial_data)
    : _id(noteID)
    , _data(initial_data) {}

} // namespace uebernotes
