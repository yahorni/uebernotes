#pragma once

#include "linux/tui/context.hpp"

#include <core/storage.hpp>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <memory>
#include <string>
#include <vector>

namespace linux::tui {

class BookList {
public:
    // TODO: get rid of straight screen passing
    BookList(core::Storage* storage, Context* ctx, ftxui::ScreenInteractive& screen);

    // TODO: error-prone way. need to think how to redo it
    std::shared_ptr<core::BookInfo> getSelected();
    void updateItems(bool forceUpdate = false);

    const ftxui::Component& getComponent() const;
    ftxui::Element getElement() const;

private:
    core::Storage* _storage{nullptr};
    Context* _ctx{nullptr};

    std::vector<std::string> bookNames;
    ftxui::Component bookMenu;
};

}  // namespace linux::tui
