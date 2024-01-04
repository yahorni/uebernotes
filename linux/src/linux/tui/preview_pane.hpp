#pragma once

#include "ftxui/component.hpp"

#include <core/note.hpp>

#include <ftxui/component/component.hpp>

#include <string>

namespace linux::tui {

class PreviewPane {
public:
    // TODO: pass note ptr here or through set...() method
    PreviewPane() { _notePreview = ftxui::Pager(_noteContent, _previewShift, _wrapLines); }

    void resetShift() {
        _previewShift = 0;
    }

    void clearContent() {
        resetShift();
        _noteContent.clear();
    }

    void setContent(const std::string& content) {
        resetShift();
        _noteContent = content;
    }

    // UI
    const ftxui::Component& getComponent() const { return _notePreview; }
    ftxui::Element getElement() const {
        using namespace ftxui;  // NOLINT
        return vbox({
                   hcenter(bold(text("Preview"))),           // consider using "window"
                   separator(),                              //
                   _notePreview->Render() | yflex | yframe,  //
               }) |
               xflex | borderDecorator(_notePreview->Focused());
    }

private:
    int _previewShift = 0;
    bool _wrapLines = true;

    std::string _noteContent;
    ftxui::Component _notePreview;
};

}  // namespace linux::tui
