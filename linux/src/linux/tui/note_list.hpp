#pragma once

#include "linux/tui/event_queue.hpp"

#include <core/storage.hpp>

#include <ftxui/component/component.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace linux::tui {

class NoteList {
public:
    NoteList(core::Storage* storage, EventQueue* eventQueue);
    void reset();

    std::shared_ptr<core::NoteInfo> getSelected(core::BookID bookID, bool refresh = false);
    void updateItems(core::BookID bookID, bool refresh = false);

    void cacheNoteIdx(core::BookID bookID);

    // UI
    const ftxui::Component& getComponent() const;
    ftxui::Element getElement() const;

private:
    core::Storage* _storage{nullptr};
    EventQueue* _eventQueue{nullptr};

    int _selectedNoteIdx = 0;
    std::unordered_map<core::BookID, int> _storedNoteIndeces{};

    std::vector<std::string> _noteNames;
    ftxui::Component _noteMenu;
};

}  // namespace linux::tui
