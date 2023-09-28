#include "linux/tui.hpp"

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

#include "ftxui/component/component.hpp"           // for CatchEvent, Renderer, operator|=
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

// ftxui/dom/elements.hpp - list of elements to create layouts
// ftxui/dom/component.hpp - interactive components to respond to events

namespace linux {

TUI::TUI(const core::AppContext& context)
    : _storage(context) {}

bool TUI::run() {
    using namespace ftxui;

    const auto& books = _storage.getBooksInfo();

    std::vector<std::string> book_names;
    book_names.reserve(books.size());

    for (const auto& book : books) {
        book_names.emplace_back(book->name);
    }

    int selected = 0;
    auto book_list = Menu(&book_names, &selected);

    auto notes_list = Renderer([] { return text("notes") | center; });
    auto note_preview = Renderer([] { return text("note preview") | center; });

    auto container = Container::Horizontal({
        book_list,
        notes_list,
        note_preview,
    });

    int quarter = Terminal::Size().dimx / 4;

    auto renderer = Renderer(container, [&] {
        return hbox({
            // books pane
            vbox({
                hcenter(bold(text("Books"))),
                separator(),
                book_list->Render(),
            }) | size(WIDTH, EQUAL, quarter) | border,
            // notes pane
            vbox({
                hcenter(bold(text("Notes"))),
                separator(),
                notes_list->Render(),
            }) | size(WIDTH, EQUAL, quarter) | border,
            // note preview
            vbox({
                hcenter(bold(text("Note preview"))),
                separator(),
                note_preview->Render(),
            }) | xflex | border
        });
    });

    auto screen = ScreenInteractive::Fullscreen();

    auto component = CatchEvent(renderer, [&](Event event) {
      if (event == Event::Character('q')) {
        screen.ExitLoopClosure()();
        return true;
      }
      return false;
    });

    screen.Loop(component);
    return true;
}

}  // namespace linux
