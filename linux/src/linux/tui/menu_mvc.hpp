#pragma once

#include "linux/tui/event_queue.hpp"

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

class SortOptions {
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
    bool sortItems(std::vector<EntityPtr>& items) {
        switch (_field) {
        case SortOptions::Field::CreationTime:
        case SortOptions::Field::UpdateTime: {
            if (_isAscending) {
                std::sort(items.begin(), items.end(), core::SharedPtrExtension::CompareID<>());
            } else {
                std::sort(items.begin(), items.end(), core::SharedPtrExtension::CompareID<std::greater<void>>());
            }
        } break;
        case SortOptions::Field::Name:
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
    using EntityPtr = std::shared_ptr<Entity>;
    using ContainerType = Container;

    Model() = default;
    Model(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) = delete;

    // UI Component
    ftxui::Component& getComponent() { return _menu; }
    void setComponent(ftxui::Component menu) { _menu = menu; }

    // items
    std::optional<decltype(Entity::id)> getItemID(int index) const {
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

    const std::vector<EntityPtr>& getItems() const { return _items; }

    void setItems(const Container& items) {
        _items.clear();
        _items = std::vector<EntityPtr>{items.begin(), items.end()};
        _sortOptions.sortItems<EntityPtr>(_items);
    }

    // sorting
    const SortOptions& getSortOptions() const { return _sortOptions; }

    bool sortByField(SortOptions::Field field) {
        if (!_sortOptions.setField(field)) {
            return false;
        }
        _sortOptions.sortItems<EntityPtr>(_items);
        return true;
    }

    bool toggleSortOrder() {
        _sortOptions.toggleOrder();
        _sortOptions.sortItems<EntityPtr>(_items);
        return _sortOptions.getOrder();
    }

private:
    std::vector<EntityPtr> _items;
    SortOptions _sortOptions;
    ftxui::Component _menu;
};

template<typename CacheKey = int, bool UseIndexCache = false>
class View {
public:
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

    // items

    void clear() { _options.clear(); }

    void addItem(std::string id, std::string name) {
        if (_showID) {
            _options.emplace_back(std::format("[{}] {}", id, name));
        } else {
            _options.emplace_back(name);
        }
    }

    // index (with cache)

    using Index = int;

    int getSelectedIndex() const { return _selectedIndex; }

    void cacheIndex(CacheKey key) { _indecesCache[key] = _selectedIndex; }

    void restoreIndex(CacheKey key) { _selectedIndex = _indecesCache.count(key) ? _indecesCache.at(key) : 0; }

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
    explicit Controller(EventQueue* eventQueue, Model& model, View& view)
        : _eventQueue(eventQueue),
          _model(model),
          _view(view) {}

    Controller(const Controller&) = delete;
    Controller(Controller&&) = delete;
    Controller& operator=(const Controller&) = delete;
    Controller& operator=(Controller&&) = delete;
    virtual ~Controller() = default;

    void createComponent() {
        auto option = ftxui::MenuOption::Vertical();
        configureComponentOption(option);
        auto menu = _view.createComponent(option);
        configureComponent(menu);

        menu |= ftxui::CatchEvent([this](ftxui::Event event) {
            if (event == ftxui::Event::Character('g')) {
                _eventQueue->push(Event::PostScreenEvent, "", ftxui::Event::Home);
                return true;
            } else if (event == ftxui::Event::Character('G')) {
                _eventQueue->push(Event::PostScreenEvent, "", ftxui::Event::End);
                return true;
            }
            return false;
        });
    }

    virtual void configureComponentOption(ftxui::MenuOption& option) = 0;
    virtual void configureComponent(ftxui::Component& menu) = 0;

    using ModelID = std::result_of_t<decltype (&Model::getItemID)(Model, int)>;
    ModelID getSelectedItemID() const { return _model.getItemID(_view.getSelectedIndex()); }

    Model::EntityPtr getSelectedItem() const { return _model.getItem(_view.getSelectedIndex()); }

    void setItems(const Model::ContainerType& items) {
        _view.resetIndex();
        _model.setItems(items);
        updateNames();
    }

    void setComponent(ftxui::Component menu) { _model.setComponent(std::move(menu)); }

    bool sortByField(SortOptions::Field field) {
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

protected:
    EventQueue* _eventQueue{nullptr};

private:
    void updateNames() {
        _view.clear();
        for (const auto& item : _model.getItems()) {
            _view.addItem(std::to_string(item->id), item->getName());
        }
    }

    Model& _model;
    View& _view;
};

}  // namespace menu
}  // namespace linux::tui
