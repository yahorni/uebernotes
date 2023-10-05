#pragma once

#include "core/appcontext.hpp"
#include "core/storage.hpp"
#include "linux/argparser.hpp"

#include <string>
#include <vector>

namespace linux {

class TUI {
public:
    explicit TUI(const core::AppContext& context);
    bool run();

private:
    std::vector<std::string> getBookNames();
    std::vector<std::string> getNoteNames(core::BookID bookID);
    std::vector<std::string> getNoteNames(core::BookID bookID, int paneWidth);

    std::shared_ptr<core::BookInfo> getSelectedBook(int selectedBook);
    std::shared_ptr<core::NoteInfo> getSelectedNote(int selectedNote, core::BookID bookID);

    core::Storage _storage;

    void _setStatusMessage(const std::string_view msg);
    // TODO: change default message
    std::string _lastMessage = "Welcome to uebernotes";
};

}  // namespace linux
