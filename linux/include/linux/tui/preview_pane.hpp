#pragma once

#include "linux/component.hpp"
#include "linux/tui/common.hpp"
#include "linux/tui/context.hpp"

#include <ftxui/component/component.hpp>

#include <memory>
#include <string>

namespace linux::tui {

class PreviewPane {
public:
    explicit PreviewPane(Context* ctx)
        : _ctx(ctx) {
        notePreview = ftxui::Pager(_ctx->previewShift, noteContent);
    }

    void updateContent(std::shared_ptr<core::NoteInfo> notePtr) {
        if (notePtr)
            noteContent = notePtr->content;
        else
            noteContent.clear();
    }

    const ftxui::Component& getComponent() const { return notePreview; }

    ftxui::Element getElement() const {
        using namespace ftxui;
        return vbox({
                   hcenter(bold(text("Preview"))),          // consider using "window"
                   separator(),                             //
                   notePreview->Render() | yflex | yframe,  //
               }) |
               xflex | borderDecorator(notePreview->Focused());
    }

private:
    Context* _ctx{nullptr};

    std::string noteContent;  // TODO: optimize copying
    ftxui::Component notePreview;
};

}  // namespace linux::tui
