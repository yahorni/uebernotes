#pragma once

#include "linux/tui/book_list.hpp"
#include "linux/tui/context.hpp"

#include <ftxui/component/component.hpp>

#include <memory>
#include <string>
#include <vector>

namespace linux::tui {

class NoteList {
public:
    NoteList(core::Storage* storage, Context* ctx, BookList& bookList);

    // TODO: error-prone way. need to think how to redo it
    std::shared_ptr<core::NoteInfo> getSelected(core::BookID bookID);
    void updateItems(core::BookID bookID, bool forceUpdate = false);

    const ftxui::Component& getComponent() const;
    ftxui::Element getElement() const;

private:
    core::Storage* _storage{nullptr};
    Context* _ctx{nullptr};

    std::vector<std::string> noteNames;
    ftxui::Component noteMenu;
};

}  // namespace linux::tui
