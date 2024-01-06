#pragma once

#include "linux/tui/menu/index_cache.hpp"
#include "linux/tui/menu/sorter.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"
#include "linux/tui/communicator.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

// view <-> controller <-> model
//              |
//          user (tui)

// TODO
// rename file, maybe move
// use detail namespace?

namespace linux::tui::menu {

template<typename Entity, typename Container>
class Model {
public:
    using EntityID = decltype(Entity::id);
    using EntityPtr = std::shared_ptr<Entity>;
    using ContainerType = Container;
    using ItemsType = std::vector<EntityPtr>;

    Model() = default;
    Model(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) = delete;

    // UI Component
    ftxui::Component& getComponent() { return _menu; }
    void setComponent(ftxui::Component menu) { _menu = menu; }

    // items
    std::optional<EntityID> getItemID(int index) const {
        if (_items.size()) {
            return _items.at(index)->id;
        }
        return std::nullopt;
    }

    EntityPtr getItem(int index) const {
        if (_items.size()) {
            return _items.at(index);
        }
        return nullptr;
    }

    const ItemsType& getItems() const { return _items; }

    void setItems(const Container& items) {
        _items.clear();
        _items = std::vector<EntityPtr>{items.begin(), items.end()};
        // TODO: move sort to controller
        _sorter.sort<EntityPtr>(_items);
        Log::debug("set model items: {}", _items.size());
    }

    // sorting
    menu::Sorter::Field getSortField() const { return _sorter.getField(); }

    bool sortByField(Sorter::Field field) {
        if (!_sorter.setField(field)) {
            return false;
        }
        _sorter.sort<EntityPtr>(_items);
        return true;
    }

    bool toggleSortOrder() {
        // TODO: move toggle to controller, leave here only getSortOrder() or similar
        _sorter.toggleOrder();
        _sorter.sort<EntityPtr>(_items);
        return _sorter.getOrder();
    }

private:
    ItemsType _items;
    Sorter _sorter;
    ftxui::Component _menu;
};

template<bool UseIndexCache = false>
class View {
public:
    // index cache
    std::optional<IndexCache> cache;

    View() {
        if constexpr (UseIndexCache) {
            cache.emplace(_selectedIndex);
        }
    }

    View(const View&) = delete;
    View(View&&) = delete;
    View& operator=(const View&) = delete;
    View& operator=(View&&) = delete;

    virtual ftxui::Element getElement(ftxui::Component& menu, int paneSize) const = 0;

    // UI component
    ftxui::Component createComponent(ftxui::MenuOption option) {
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

        return ftxui::Menu(&_options, &_selectedIndex, std::move(option));
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

    // view options
    bool toggleShowID() {
        _showID = !_showID;
        return _showID;
    }

private:
    int _selectedIndex = 0;
    bool _showID = false;
    std::vector<std::string> _options;
};

template<typename Model, typename View>
class Controller {
public:
    explicit Controller(Model& model, View& view)
        : _model(model),
          _view(view) {}

    Controller(const Controller&) = delete;
    Controller(Controller&&) = delete;
    Controller& operator=(const Controller&) = delete;
    Controller& operator=(Controller&&) = delete;
    virtual ~Controller() = default;

    virtual void configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) = 0;
    virtual void configureComponent(ftxui::Component& component, Communicator& communicator) = 0;

    void createComponent(Communicator& communicator) {
        // DO NOT move it in constructor, there are virtual functions calls

        auto option = ftxui::MenuOption::Vertical();
        configureComponentOption(option, communicator);
        auto menu = _view.createComponent(option);
        configureComponent(menu, communicator);

        // make always focusable
        menu |= ftxui::FocusableWrapper();

        // set keys
        menu |= ftxui::IgnoreEvents({ftxui::Event::Tab, ftxui::Event::TabReverse});

        // add g/G command to scroll begin/end
        menu |= ftxui::CatchEvent([&](ftxui::Event event) {
            if (event == ftxui::Event::Character('g')) {
                communicator.cmdPush(Command::UIEvent, ftxui::Event::Home);
                return true;
            } else if (event == ftxui::Event::Character('G')) {
                communicator.cmdPush(Command::UIEvent, ftxui::Event::End);
                return true;
            }
            return false;
        });

        menu |= ftxui::EventHandler({ftxui::Event::Character('j')});
        _model.setComponent(std::move(menu));
    }

    std::optional<typename Model::EntityID> getSelectedItemID() const {
        return _model.getItemID(_view.getSelectedIndex());
    }

    // TODO: error prone while no data integrity between Model and View
    // Probably worth add size check
    Model::EntityPtr getSelectedItem() const { return _model.getItem(_view.getSelectedIndex()); }

    const Model::ItemsType& getItems() const { return _model.getItems(); }

    void resetView() {
        _view.resetIndex();
        updateNames();
    }

    bool sortByField(Sorter::Field field) {
        if (_model.sortByField(field)) {
            updateNames();
            return true;
        }
        return false;
    }

    menu::Sorter::Field toggleSortField() {
        menu::Sorter::Field field{};
        switch (_model.getSortField()) {
        case menu::Sorter::Field::CreationTime:
        case menu::Sorter::Field::UpdateTime:
            field = Sorter::Field::Name;
            break;
        case menu::Sorter::Field::Name:
            field = Sorter::Field::CreationTime;
            break;
        }
        sortByField(field);
        return field;
    }

    bool toggleSortOrder() {
        bool isAscending = _model.toggleSortOrder();
        updateNames();
        return isAscending;
    }

    bool toggleShowID() {
        bool enabled = _view.toggleShowID();
        updateNames();
        return enabled;
    }

private:
    void updateNames() {
        _view.clearOptions();
        for (const auto& item : _model.getItems()) {
            _view.addOption(std::to_string(item->id), item->getName());
        }
        Log::debug("update view names, size: {}", _view.getOptionsSize());
    }

    Model& _model;
    View& _view;
};

}  // namespace linux::tui::menu
