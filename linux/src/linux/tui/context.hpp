#pragma once

#include <core/types.hpp>

#include <string>
#include <unordered_map>

namespace linux::tui {

class Context {
public:
    // menu selections
    int selectedBookIdx = 0;
    int selectedNoteIdx = 0;
    std::unordered_map<core::BookID, int> storedNoteIndeces{};

    // preview pane
    int previewShift = 0;

    // status line
    std::string lastMessage = "Welcome to uebernotes";
};

}  // namespace linux::tui
