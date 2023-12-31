#pragma once

#include <ftxui/component/component.hpp>

#include <vector>
#include <string>

namespace linux::tui {

class HistoryPanel {
public:
    HistoryPanel();

    void addMessage(std::string&& message);
    bool toggle();

    // UI
    const ftxui::Component& getComponent() const;
    ftxui::Element getElement(int panelSize) const;

private:
    const std::size_t _maxHistorySize = 50;
    std::vector<std::string> _messageHistory;

    bool _enabled = false;

    int _shift = 0;
    bool _wrap = false;

    ftxui::Component _historyPager;
};

}  // namespace linux::tui
