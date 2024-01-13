#pragma once

#include "linux/utils/noncopyable.hpp"

#include <ftxui/component/component.hpp>

#include <string>
#include <utility>

namespace linux::tui::mvc {

// TODO: rename file
// think if common interface for View/Model is needed

class Model : private utils::NonCopyable {};

class View : private utils::NonCopyable {
public:
    // UI component
    const ftxui::Component& component() const { return _component; }

protected:
    ftxui::Component _component;
};


class Controller : private utils::NonCopyable {
public:
    explicit Controller(std::string name)
        : _name(std::move(name)) {}

    const std::string& name() const { return _name; }

private:
    std::string _name;
};

}  // namespace linux::tui::mvc
