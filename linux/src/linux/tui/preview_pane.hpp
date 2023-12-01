#pragma once

#include "ftxui/component.hpp"
#include "linux/tui/common.hpp"
#include "linux/tui/event_queue.hpp"

#include <core/note.hpp>

#include <cwchar>
#include <ftxui/component/component.hpp>

#include <string>

namespace linux::tui {

class PreviewPane {
public:
    explicit PreviewPane(EventQueue* eventQueue)
        : _eventQueue(eventQueue) {
        _notePreview = ftxui::Pager(_previewShift, _noteContent);
    }

    void reset() {
        _previewShift = 0;
        _noteContent.clear();
    }

    void setContent(const std::string& content) {
        _previewShift = 0;
        _noteContent = content;
    }

    // UI
    const ftxui::Component& getComponent() const { return _notePreview; }
    ftxui::Element getElement() const {
        using namespace ftxui;
        return vbox({
                   hcenter(bold(text("Preview"))),           // consider using "window"
                   separator(),                              //
                   _notePreview->Render() | yflex | yframe,  //
               }) |
               xflex | borderDecorator(_notePreview->Focused());
    }

private:
    EventQueue* _eventQueue{nullptr};

    int _previewShift = 0;

    std::string _noteContent;  // TODO: optimize copying
    ftxui::Component _notePreview;
};

}  // namespace linux::tui
