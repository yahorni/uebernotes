#include "linux/tui/book_list.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"
#include "linux/tui/common.hpp"

namespace linux {
namespace tui {

BookList::BookList(core::Storage* storage, Context* ctx, ftxui::ScreenInteractive& screen)
    : _storage(storage),
      _ctx(ctx) {
    auto bookMenuOption = ftxui::MenuOption::Vertical();

    // to select focused item immediately
    // FIXME: allow select without focusing
    bookMenuOption.focused_entry = &_ctx->selectedBookIdx;

    // set events
    bookMenuOption.on_change = [&]() {
        auto bookPtr = getSelected();
        _ctx->selectedNoteIdx =
            (_ctx->storedNoteIndeces.count(bookPtr->id) ? _ctx->storedNoteIndeces.at(bookPtr->id) : 0);
        _ctx->previewShift = 0;

        Log::debug("Selected book: book={}, note={}", _ctx->selectedBookIdx, _ctx->selectedNoteIdx);
    };
    bookMenuOption.on_enter = [&]() { screen.PostEvent(ftxui::Event::ArrowRight); };

    // set component
    bookMenu = ftxui::Menu(&bookNames, &_ctx->selectedBookIdx, bookMenuOption);
    bookMenu |= ftxui::FocusableWrapper();

    // set keys
    bookMenu |= ignoreTabDecorator;
    bookMenu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            Log::info("Refreshing books cache");
            this->updateItems(true);
            // TODO: decide, what should be updated: book list or note list when book is selected
            // TODO: refresh notes for selected book
            return true;
        }
        return false;
    });
}

// TODO: error-prone way. need to think how to redo it
std::shared_ptr<core::BookInfo> BookList::getSelected() {
    const auto& books = _storage->getBookInfos();
    if (!books.size()) return nullptr;

    int i = 0;
    for (const auto& book : books) {
        if (i == _ctx->selectedBookIdx) return book;
        i++;
    }
    return nullptr;
}

void BookList::updateItems(bool forceUpdate) {
    // TODO: add force refresh without cache
    const auto& books = _storage->getBookInfos(forceUpdate);

    // TODO: shrink_to_fit()
    bookNames.clear();
    bookNames.reserve(books.size());

    for (const auto& book : books) {
        bookNames.emplace_back(book->name);
    }
}

const ftxui::Component& BookList::getComponent() const { return bookMenu; }

ftxui::Element BookList::getElement() const {
    using namespace ftxui;
    return vbox({
               hcenter(bold(text("Books"))),  // consider using "window"
               separator(),                   //
               bookMenu->Render(),            //
           }) |
           borderDecorator(bookMenu->Focused());
}

}  // namespace tui
}  // namespace linux
