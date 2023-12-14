#pragma once

#include "linux/tui/event_queue.hpp"
#include "linux/tui/menu_controller.hpp"

#include <core/comparator.hpp>
#include <core/storage.hpp>

#include <ftxui/component/component.hpp>

namespace linux::tui {

class BookList {
public:
    BookList(core::Storage* storage, EventQueue* eventQueue);

    std::optional<core::BookID> getSelectedID() const;
    void reloadItems();
    void reset();

    // UI
    const ftxui::Component& getComponent() const;
    ftxui::Element getElement() const;

private:
    core::Storage* _storage{nullptr};
    EventQueue* _eventQueue{nullptr};

    MenuController<core::Book, core::BooksCache> _menuController;

    ftxui::Component _bookMenu;
};

}  // namespace linux::tui
