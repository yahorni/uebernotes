#pragma once

#include "ftxui/component.hpp"
#include "linux/tui/menu/index_cache.hpp"
#include "linux/tui/mvc.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

#include <string>
#include <utility>
#include <vector>

namespace linux::tui::menu {

class View : public mvc::View {
public:
    ftxui::Element element(const std::string& name, int width) const {
        using namespace ftxui;  // NOLINT
        return vbox({
                   hcenter(bold(text(name))),  // consider using "window"
                   separator(),                //
                   _component->Render(),       //
               }) |
               borderDecorator(_component->Focused()) | size(WIDTH, EQUAL, width);
    }

    // UI component
    ftxui::Component& createComponent(ftxui::MenuOption option) {
        option.focused_entry = &_focusedIndex;
        option.entries_option.transform = [](const ftxui::EntryState& state) {
            ftxui::Element e = ftxui::text((state.active ? "> " : "  ") + state.label);
            if (state.focused) {
                e |= ftxui::inverted;
            }
            if (state.active) {
                e |= ftxui::bold;
            }
            return e;
        };

        _component = ftxui::Menu(&_options, &_selectedIndex, std::move(option));
        return _component;
    }

    // options
    void addOption(std::string_view id, std::string_view name) {
        if (_showID) {
            _options.emplace_back(std::format("[{}] {}", id, name));
        } else {
            _options.emplace_back(name);
        }
    }
    void clearOptions() { _options.clear(); }
    std::size_t getOptionsSize() { return _options.size(); }

    // index
    int getSelectedIndex() const { return _selectedIndex; }
    void resetIndex() { _selectedIndex = 0; }

    // cache
    template<typename Key>
    auto createIndexCache() {
        return IndexCache<Key>(_selectedIndex, _focusedIndex);
    }

    // view options
    bool toggleShowID() {
        _showID = !_showID;
        return _showID;
    }

private:
    int _selectedIndex = 0;
    int _focusedIndex = 0;
    bool _showID = false;
    std::vector<std::string> _options;
};

}  // namespace linux::tui::menu
