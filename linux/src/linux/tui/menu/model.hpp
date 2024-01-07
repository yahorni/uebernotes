#pragma once

#include "linux/logger.hpp"
#include "linux/tui/menu/sorter.hpp"

#include <ftxui/component/component.hpp>

#include <memory>
#include <vector>

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
        if (_sorter.setOrder(isAscending)) {
            return false;
        }
        _sorter.sort<EntityPtr>(_items);
        return true;
    }

private:
    ItemsType _items;
    Sorter _sorter;
    ftxui::Component _menu;
};

}  // namespace linux::tui::menu
