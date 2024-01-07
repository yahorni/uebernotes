#pragma once

#include <unordered_map>

namespace linux::tui::menu {

template<typename Key>
class IndexCache {
public:
    explicit IndexCache(int& selected, int& focused)
        : _selected(selected),
          _focused(focused) {}

    void add(Key key) { _cache[key] = _selected; }

    void restore(Key key) {
        _selected = _cache.count(key) ? _cache.at(key) : 0;
        _focused = _selected;
    }

    void remove(Key key) { _cache.erase(key); }

    void clear() { _cache.clear(); }

private:
    int& _selected;
    int& _focused;
    std::unordered_map<Key, int> _cache;
};

}  // namespace linux::tui::menu
