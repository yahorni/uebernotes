#pragma once

#include "linux/tui/event_queue.hpp"
#include <core/comparator.hpp>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace linux::tui {

enum class SortField {
    // TODO: make it sort by time, not ID
    CreationTime,
    // TODO: implement
    UpdateTime,
    Name,
};

template<typename Entity, typename Container, typename CacheKey = int>
class MenuController {
public:
    using EntityPtr = std::shared_ptr<Entity>;
    using Index = int;

    explicit MenuController(bool useIndexCache = false)
        : _useIndexCache(useIndexCache) {}

    ftxui::Component createMenu(ftxui::MenuOption& menuOption) {
        // to select focused item immediately
        // FIXME: allow select without focusing
        menuOption.focused_entry = &_selectedIndex;

        // return ftxui::Menu(&_itemNames, &_selectedIndex, menuOption);
        ftxui::Component menu = ftxui::Menu(&_itemNames, &_selectedIndex, menuOption);
        // FIXME: _selectedIndex assignment is not generating on_change event
        menu |= ftxui::CatchEvent([this](ftxui::Event event) {
            if (event == ftxui::Event::Character('g')) {
                _selectedIndex = 0;
                return true;
            } else if (event == ftxui::Event::Character('G')) {
                // TODO: remove random huge value
                _selectedIndex = 99999;
                return true;
            }
            return false;
        });
        return menu;
    }

    std::optional<decltype(Entity::id)> getSelectedItemID() const {
        if (_items.size()) {
            return _items.at(_selectedIndex)->id;
        }
        return std::nullopt;
    }

    EntityPtr getSelectedItem() const {
        if (_items.size()) {
            return _items.at(_selectedIndex);
        }
        return {};
    }

    void resetIndex(std::optional<CacheKey> keyToReset = std::nullopt) {
        _selectedIndex = 0;
        if (_useIndexCache) {
            if (keyToReset) {
                _indecesCache.erase(*keyToReset);
            } else {
                _indecesCache.clear();
            }
        }
    }

    void cacheIndex(CacheKey key) { _indecesCache[key] = _selectedIndex; }

    void useCachedIndex(CacheKey key) { _selectedIndex = _indecesCache.count(key) ? _indecesCache.at(key) : 0; }

    bool setSortType(SortField sortField) {
        if (_sortField == sortField) {
            return false;
        }

        _sortField = sortField;
        return true;
    }

    bool toggleSortOrder() {
        _ascendingOrder = !_ascendingOrder;
        return _ascendingOrder;
    }

    void sortItems() {
        switch (_sortField) {
        case SortField::CreationTime:
        case SortField::UpdateTime: {
            if (_ascendingOrder) {
                std::sort(_items.begin(), _items.end(), core::SharedPtrExtension::CompareID<>());
            } else {
                std::sort(_items.begin(), _items.end(), core::SharedPtrExtension::CompareID<std::greater<void>>());
            }
        } break;
        case SortField::Name:
        default: {
            if (_ascendingOrder) {
                std::sort(_items.begin(), _items.end(), core::SharedPtrExtension::CompareName<>());
            } else {
                std::sort(_items.begin(), _items.end(), core::SharedPtrExtension::CompareName<std::greater<void>>());
            }
        } break;
        }

        updateNames();
    }

    void setItems(const Container& items) {
        // TODO: get rid of index resetting here or in resetIndex()
        _selectedIndex = 0;

        _items.clear();
        _items = std::vector<EntityPtr>{items.begin(), items.end()};

        sortItems();
    }

    bool toggleShowID() {
        _showID = !_showID;
        updateNames();
        return _showID;
    }

    size_t getItemsAmount() {
        return _items.size();
    }

private:
    void updateNames() {
        _itemNames.clear();
        _itemNames.reserve(_items.size());
        for (const auto& item : _items) {
            if (_showID) {
                _itemNames.emplace_back(std::format("[{}] {}", item->id, item->getName()));
            } else {
                _itemNames.emplace_back(item->getName());
            }
        }
    }

    SortField _sortField = SortField::CreationTime;
    bool _ascendingOrder = true;
    bool _showID = false;

    Index _selectedIndex = 0;
    std::vector<EntityPtr> _items;
    std::vector<std::string> _itemNames;

    bool _useIndexCache = false;
    std::unordered_map<CacheKey, Index> _indecesCache;
};

}  // namespace linux::tui
