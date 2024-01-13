#pragma once

#include "linux/logger.hpp"
#include "linux/tui/menu/sorter.hpp"
#include "linux/tui/mvc.hpp"

#include <ftxui/component/component.hpp>

#include <vector>

namespace linux::tui::menu {

template<typename EntityPtrT, typename ContainerT>
class Model : public mvc::Model {
public:
    using EntityID = decltype(EntityPtrT()->id);
    using EntityPtr = EntityPtrT;
    using Container = ContainerT;
    using ItemsType = std::vector<EntityPtrT>;

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
        Log::debug("Set menu model items: {}", _items.size());
    }

    // sorting

    menu::Sorter::Field getSortField() const { return _sorter.getField(); }
    bool isAscending() const { return _sorter.isAscending(); }

    bool sortByField(Sorter::Field field) {
        if (!_sorter.setField(field)) {
            return false;
        }
        _sorter.sort<EntityPtr>(_items);
        return true;
    }

    bool sortByOrder(bool isAscending) {
        if (!_sorter.setOrder(isAscending)) {
            return false;
        }
        _sorter.sort<EntityPtr>(_items);
        return true;
    }

private:
    ItemsType _items;
    Sorter _sorter;
};

}  // namespace linux::tui::menu
