#pragma once

#include "linux/tui/communicator.hpp"

#include <core/note.hpp>

#include <ftxui/component/component.hpp>

#include <string>

namespace linux::tui::preview {

// TODO: try PImpl if it works
namespace detail {

class Model {
public:
    Model() = default;
    Model(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) = delete;

    // UI Component
    const ftxui::Component& getComponent() const;
    void setComponent(ftxui::Component pager);

    // data
    void setNote(const core::NotePtr& notePtr);

private:
    core::NotePtr _notePtr;
    ftxui::Component _pager;
};

class View {
public:
    View() = default;
    View(const View&) = delete;
    View(View&&) = delete;
    View& operator=(const View&) = delete;
    View& operator=(View&&) = delete;

    ftxui::Component createComponent();

    void resetShift();
    void clearContent();
    void setContent(const std::string& content);

    ftxui::Element getElement(const ftxui::Component& component) const;

private:
    std::string _noteContent;  // TODO: get rid of it
    int _previewShift = 0;
    bool _wrapLines = true;
};

}  // namespace detail

class Controller {
public:
    Controller();
    Controller(const Controller&) = delete;
    Controller(Controller&&) = delete;
    Controller& operator=(const Controller&) = delete;
    Controller& operator=(Controller&&) = delete;

    void createComponent(Communicator& communicator);
    void configureComponent(ftxui::Component& component, Communicator& communicator);
    void setNote(const core::NotePtr& notePtr);
    const ftxui::Component& component() const;
    ftxui::Element element() const;

private:
    detail::Model _model;
    detail::View _view;
};

}  // namespace linux::tui::preview
