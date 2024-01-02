#include "linux/tui/note_list.hpp"

#include "ftxui/component.hpp"
#include "linux/tui/event_queue.hpp"

#include <core/types.hpp>

#include <string>
#include <utility>

namespace linux::tui::note {

ftxui::Element View::getElement(ftxui::Component& menu, int paneSize) const {
    using namespace ftxui;  // NOLINT
    return vbox({
               hcenter(bold(text("Notes"))),  // consider using "window"
               separator(),                   //
               menu->Render(),                //
           }) |
           borderDecorator(menu->Focused()) | size(WIDTH, EQUAL, paneSize);
}

void Controller::configureComponentOption(ftxui::MenuOption& option) {
    option.elements_postfix = [this] { return getItems().size() ? ftxui::linefiller('-') : ftxui::emptyElement(); };

    option.on_change = [this]() {
        _eventQueue->push(Event::NoteChanged, std::format("Selected note: {}", *getSelectedItemID()));
    };
    option.on_enter = [this]() { _eventQueue->push(Event::OpenEditor, "TODO: Open editor", getSelectedItemID()); };
}

void Controller::configureComponent(ftxui::Component& menu) {
    menu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            std::string message;
            if (auto noteID = getSelectedItemID(); noteID) {
                message = std::format("Refreshed note: {}", *noteID);
            } else {
                message = "No note to refresh";
            }
            _eventQueue->push(Event::RefreshNote, std::move(message));
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            if (sortByField(Sorter::Field::Name)) {
                _eventQueue->push(Event::NoteListUpdated, "Sort notes by name");
            }
            return true;
        } else if (event == ftxui::Event::Character('S')) {
            if (sortByField(Sorter::Field::CreationTime)) {
                _eventQueue->push(Event::NoteListUpdated, "Sort notes by date");
            }
            return true;
        } else if (event == ftxui::Event::Character('o')) {
            bool isAscending = toggleSortOrder();
            _eventQueue->push(Event::NoteListUpdated, std::format("Ascending notes sort order: {}", isAscending));
            return true;
        } else if (event == ftxui::Event::Character('i')) {
            bool enabled = toggleShowID();
            _eventQueue->push(Event::NoteListUpdated, std::format("Show notes ID: {}", enabled));
            return true;
        }
        return false;
    });
}

}  // namespace linux::tui::note
