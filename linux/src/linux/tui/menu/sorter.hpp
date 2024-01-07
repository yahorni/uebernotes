#pragma once

#include <core/comparator.hpp>

#include <algorithm>
#include <functional>
#include <vector>

namespace linux::tui::menu {

class Sorter {
public:
    enum class Field {
        CreationTime,  // TODO: make it sort by actual time, not ID
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

    bool isAscending() const { return _isAscending; }

    bool setOrder(bool isAscending) {
        if (_isAscending == isAscending) {
            return false;
        }
        _isAscending = isAscending;
        return true;
    }

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

}  // namespace linux::tui::menu
