#pragma once

#include "linux/tui/communicator.hpp"
#include "linux/tui/mvc.hpp"

#include <ftxui/component/component.hpp>

#include <memory>
#include <string>

namespace linux::tui::history {

// TODO: make it scroll to the end automatically

class Model;
class View;

class Controller : public mvc::Controller {
public:
    Controller();
    ~Controller();

    void createComponent(Communicator& communicator);

    bool toggleView();
    void addMessage(std::string message);

    const ftxui::Component& component() const;
    ftxui::Element element(int height) const;

private:
    void configureComponent(ftxui::Component& component, Communicator& communicator);

    std::unique_ptr<Model> _model;
    std::unique_ptr<View> _view;
};

}  // namespace linux::tui::history
