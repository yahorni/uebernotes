#pragma once

#include <core/comparator.hpp>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace linux::tui {

enum class SortType {
    Ascending,
    Descending,
    CreationTime  // TODO: make it sort by time, not ID
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
        return ftxui::Menu(&_itemNames, &_selectedIndex, menuOption);
    }

    std::optional<decltype(Entity::id)> getSelectedItemID() const {
        if (_sortedItems.size()) {
            return _sortedItems.at(_selectedIndex)->id;
        }
        return std::nullopt;
    }

    EntityPtr getSelectedItem() const {
        if (_sortedItems.size()) {
            return _sortedItems.at(_selectedIndex);
        }
        return {};
    }

    bool setSortType(SortType sortType) {
        if (sortType == _sortType) {
            return false;
        }

        _sortType = sortType;
        return true;
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

    void reloadItems(const Container& items) {
        _selectedIndex = 0;

        _sortedItems.clear();
        _sortedItems = std::vector<EntityPtr>{items.begin(), items.end()};

        switch (_sortType) {
        case SortType::CreationTime: {
            std::sort(_sortedItems.begin(), _sortedItems.end(), core::SharedPtrExtension::CompareID());
        } break;
        case SortType::Descending: {
            std::sort(_sortedItems.begin(), _sortedItems.end(), core::SharedPtrExtension::CompareNameAsc());
        } break;
        case SortType::Ascending:
        default: {
            std::sort(_sortedItems.begin(), _sortedItems.end(), core::SharedPtrExtension::CompareNameDesc());
        } break;
        }

        _itemNames.clear();
        _itemNames.reserve(_sortedItems.size());
        for (const auto& item : _sortedItems) {
            _itemNames.emplace_back(item->getName());
        }
    }

private:
    SortType _sortType = SortType::Ascending;
    Index _selectedIndex = 0;
    std::vector<EntityPtr> _sortedItems;
    std::vector<std::string> _itemNames;

    bool _useIndexCache = false;
    std::unordered_map<CacheKey, Index> _indecesCache;
};

}  // namespace linux::tui
