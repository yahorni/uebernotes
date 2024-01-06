#pragma once

#include <unordered_map>

namespace linux::tui::menu {

class IndexCache {
public:
    explicit IndexCache(int& selected)
        : _selected(selected) {}

    void add(int key) { _cache[key] = _selected; }

    void restore(int key) { _selected = _cache.count(key) ? _cache.at(key) : 0; }

    void remove(int keyToReset) { _cache.erase(keyToReset); }

    void clear() { _cache.clear(); }

private:
    int& _selected;
    std::unordered_map<int, int> _cache;
};

}  // namespace linux::tui::menu
