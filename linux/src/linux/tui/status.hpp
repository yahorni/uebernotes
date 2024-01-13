#pragma once

#include "linux/tui/communicator.hpp"
#include "linux/tui/mvc.hpp"

#include <ftxui/component/component.hpp>

#include <memory>
#include <string>

namespace linux::tui::status {

enum class Mode { Status, Search, Command };

class Model;
class View;

class Controller : public mvc::Controller {
public:
    Controller();
    ~Controller();

    void createComponent(Communicator& communicator);

    const std::string& getLastInput() const;
    void setMessage(std::string message);
    void setMode(Mode mode);

    const ftxui::Component& component() const;
    ftxui::Element element() const;

private:
    void configureComponentOption(ftxui::InputOption& option, Communicator& communicator);
    void configureComponent(ftxui::Component& component, Communicator& communicator);

    std::unique_ptr<Model> _model;
    std::unique_ptr<View> _view;
};

}  // namespace linux::tui::status
