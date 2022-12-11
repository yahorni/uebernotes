#pragma once

#include <cstdint>
#include <string>

namespace uebernotes {

using NoteID = uint64_t;

class Note {
public:
    Note(const NoteID& noteID, const std::string& initial_data);

private:
    NoteID _id;
    std::string _data;
};

} // namespace uebernotes
