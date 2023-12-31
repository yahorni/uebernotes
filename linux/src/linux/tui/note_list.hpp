#pragma once

#include "linux/tui/event_queue.hpp"
#include "linux/tui/menu_controller.hpp"

#include <core/storage.hpp>

#include <ftxui/component/component.hpp>

#include <memory>

namespace linux::tui {

class NoteList {
public:
    NoteList(core::Storage* storage, EventQueue* eventQueue);

    std::shared_ptr<core::Note> getSelectedItem() const;
    std::optional<core::NoteID> getSelectedID() const;
    void reloadItems(core::BookID bookID, bool useCached = true);
    void cacheIndex(core::BookID bookID);
    void reset();

    // UI
    const ftxui::Component& getComponent() const;
    ftxui::Element getElement(int paneSize) const;

private:
    core::Storage* _storage{nullptr};
    EventQueue* _eventQueue{nullptr};

    MenuController<core::Note, core::NotesCache> _menuController;

    ftxui::Component _noteMenu;
};

}  // namespace linux::tui
