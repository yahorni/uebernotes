#pragma once

#include <string>
#include <utility>

namespace linux::tui::mvc {

// TODO: rename file
// think if common interface for View/Model is needed

class Controller {
public:
    explicit Controller(std::string name)
        : _name(std::move(name)) {}

    const std::string& name() const { return _name; }

private:
    std::string _name;
};

}  // namespace linux::tui::mvc
