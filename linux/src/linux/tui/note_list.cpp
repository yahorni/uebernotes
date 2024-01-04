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

void Controller::configureComponentOption(ftxui::MenuOption& option, EventQueue& eventQueue) {
    option.elements_postfix = [&] { return getItems().size() ? ftxui::linefiller('-') : ftxui::emptyElement(); };

    option.on_change = [&]() {
        eventQueue.push(Event::NoteChanged, std::format("Selected note: {}", *getSelectedItemID()));
    };
    option.on_enter = [&]() { eventQueue.push(Event::OpenEditor, "TODO: Open editor", getSelectedItemID()); };
}

void Controller::configureComponent(ftxui::Component& menu, EventQueue& eventQueue) {
    menu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            std::string message;
            if (auto noteID = getSelectedItemID(); noteID) {
                message = std::format("Refreshed note: {}", *noteID);
            } else {
                message = "No note to refresh";
            }
            eventQueue.push(Event::RefreshNote, std::move(message));
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            if (sortByField(Sorter::Field::Name)) {
                eventQueue.push(Event::NoteListUpdated, "Sort notes by name");
            }
            return true;
        } else if (event == ftxui::Event::Character('S')) {
            if (sortByField(Sorter::Field::CreationTime)) {
                eventQueue.push(Event::NoteListUpdated, "Sort notes by date");
            }
            return true;
        } else if (event == ftxui::Event::Character('o')) {
            bool isAscending = toggleSortOrder();
            eventQueue.push(Event::NoteListUpdated, std::format("Ascending notes sort order: {}", isAscending));
            return true;
        } else if (event == ftxui::Event::Character('i')) {
            bool enabled = toggleShowID();
            eventQueue.push(Event::NoteListUpdated, std::format("Show notes ID: {}", enabled));
            return true;
        }
        return false;
    });
}

}  // namespace linux::tui::note
