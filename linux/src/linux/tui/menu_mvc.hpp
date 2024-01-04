#pragma once

#include "ftxui/component.hpp"
#include "linux/tui/communicator.hpp"

#include <core/comparator.hpp>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

// view <-> controller <-> model

// TODO
// rename file, maybe move
// use detail namespace?

namespace linux::tui {

class Sorter {
public:
    enum class Field {
        CreationTime,  // TODO: make it sort by time, not ID
        UpdateTime,    // TODO: implement
        Name,
    };

    Field getField() const { return _field; }

    bool setField(Field field) {
        if (_field == field) {
            return false;
        }
        _field = field;
        return true;
    }

    bool getOrder() const { return _isAscending; }

    bool setOrder(bool isAscending) {
        if (_isAscending == isAscending) {
            return false;
        }
        _isAscending = isAscending;
        return true;
    }

    void toggleOrder() { _isAscending = !_isAscending; }

    template<class EntityPtr>
    bool sort(std::vector<EntityPtr>& items) {
        switch (_field) {
        case Sorter::Field::CreationTime:
        case Sorter::Field::UpdateTime: {
            if (_isAscending) {
                std::sort(items.begin(), items.end(), core::SharedPtrExtension::CompareID<>());
            } else {
                std::sort(items.begin(), items.end(), core::SharedPtrExtension::CompareID<std::greater<void>>());
            }
        } break;
        case Sorter::Field::Name:
        default: {
            if (_isAscending) {
                std::sort(items.begin(), items.end(), core::SharedPtrExtension::CompareName<>());
            } else {
                std::sort(items.begin(), items.end(), core::SharedPtrExtension::CompareName<std::greater<void>>());
            }
        } break;
        }

        return true;
    }

private:
    Field _field = Field::CreationTime;
    bool _isAscending = true;
};

namespace menu {

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
        _sorter.sort<EntityPtr>(_items);
    }

    // sorting
    bool sortByField(Sorter::Field field) {
        if (!_sorter.setField(field)) {
            return false;
        }
        _sorter.sort<EntityPtr>(_items);
        return true;
    }

    bool toggleSortOrder() {
        _sorter.toggleOrder();
        _sorter.sort<EntityPtr>(_items);
        return _sorter.getOrder();
    }

private:
    ItemsType _items;
    Sorter _sorter;
    ftxui::Component _menu;
};

template<bool UseIndexCache = false, typename CacheKey = int>
class View {
public:
    using CacheKeyType = CacheKey;

    View() = default;
    View(const View&) = delete;
    View(View&&) = delete;
    View& operator=(const View&) = delete;
    View& operator=(View&&) = delete;

    virtual ftxui::Element getElement(ftxui::Component& menu, int paneSize) const = 0;

    // UI component
    ftxui::Component createComponent(ftxui::MenuOption menuOption) {
        // FIXME: allow select without focusing
        // now it sets focused item immediately to selected one
        menuOption.focused_entry = &_selectedIndex;
        return ftxui::Menu(&_options, &_selectedIndex, std::move(menuOption));
    }

    // options

    void addOption(std::string_view id, std::string_view name) {
        if (_showID) {
            _options.emplace_back(std::format("[{}] {}", id, name));
        } else {
            _options.emplace_back(name);
        }
    }

    void clear() { _options.clear(); }

    // index (optional caching)

    using Index = int;

    int getSelectedIndex() const { return _selectedIndex; }

    void cacheIndex(CacheKey key) { _indecesCache[key] = _selectedIndex; }

    void restoreIndex(std::optional<CacheKey> key) {
        _selectedIndex = (key && _indecesCache.count(*key)) ? _indecesCache.at(*key) : 0;
    }

    void resetIndex(std::optional<CacheKey> keyToReset = std::nullopt) {
        _selectedIndex = 0;
        if constexpr (UseIndexCache) {
            if (keyToReset) {
                _indecesCache.erase(*keyToReset);
            } else {
                _indecesCache.clear();
            }
        }
    }

    // other view options

    bool toggleShowID() {
        _showID = !_showID;
        return _showID;
    }

private:
    bool _showID = false;

    Index _selectedIndex = 0;

    std::unordered_map<CacheKey, Index> _indecesCache;

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
    virtual void configureComponent(ftxui::Component& menu, Communicator& communicator) = 0;

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

    Model::EntityPtr getSelectedItem() const { return _model.getItem(_view.getSelectedIndex()); }

    const Model::ItemsType& getItems() const { return _model.getItems(); }

    void setItems(const Model::ContainerType& items) {
        _view.resetIndex();
        _model.setItems(items);
        updateNames();
    }

    void cacheKey(View::CacheKeyType cacheKey) {
        _view.cacheIndex(cacheKey);
    }

    bool sortByField(Sorter::Field field) {
        if (_model.sortByField(field)) {
            updateNames();
            return true;
        }
        return false;
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
        _view.clear();
        for (const auto& item : _model.getItems()) {
            _view.addOption(std::to_string(item->id), item->getName());
        }
    }

    Model& _model;
    View& _view;
};

}  // namespace menu
}  // namespace linux::tui
