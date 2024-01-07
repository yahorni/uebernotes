#include "linux/tui/history_panel.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"

#include <utility>

namespace linux::tui {

HistoryPanel::HistoryPanel() {
    _messageHistory.reserve(_maxHistorySize);

    _historyPager = ftxui::Pager(_messageHistory, _shift, _wrap);
    _historyPager |= ftxui::Maybe([this] { return _enabled; });
}

void HistoryPanel::addMessage(std::string&& message) {
    if (_messageHistory.size() == _maxHistorySize) {
        _messageHistory.erase(_messageHistory.begin());
        Log::debug("Pop message from history");
    }

    _messageHistory.push_back(std::move(message));
    Log::debug("Push message to history");
}

bool HistoryPanel::toggle() {
    _enabled = !_enabled;
    return _enabled;
}

const ftxui::Component& HistoryPanel::getComponent() const { return _historyPager; }

ftxui::Element HistoryPanel::getElement(int panelSize) const {
    if (_enabled) {
        return _historyPager->Render() | ftxui::borderDecorator(_historyPager->Focused()) |
               ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, panelSize);
    } else {
        return ftxui::emptyElement();
    }
}

}  // namespace linux::tui
