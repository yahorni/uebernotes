#pragma once

#include "linux/tui/event_queue.hpp"

#include <core/storage.hpp>

#include <ftxui/component/component.hpp>

#include <string>
#include <vector>

namespace linux::tui {

class BookList {
public:
    BookList(core::Storage* storage, EventQueue* eventQueue);

    void reset();

    std::optional<core::BookID> getSelectedBookID();
    void updateItems();

    // UI
    const ftxui::Component& getComponent() const;
    ftxui::Element getElement() const;

private:
    core::Storage* _storage{nullptr};
    EventQueue* _eventQueue{nullptr};

    int _selectedBookIdx = 0;

    std::vector<std::string> _bookNames;
    ftxui::Component _bookMenu;
};

}  // namespace linux::tui
