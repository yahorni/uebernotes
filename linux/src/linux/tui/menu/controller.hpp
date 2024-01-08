#pragma once

#include "ftxui/component.hpp"
#include "linux/logger.hpp"
#include "linux/tui/communicator.hpp"
#include "linux/tui/menu/sorter.hpp"
#include "linux/utils/noncopyable.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

#include <memory>
#include <utility>

namespace linux::tui::menu {

template<typename Model, typename View>
class Controller : private utils::NonCopyable {
public:
    Controller()
        : _model(std::make_unique<Model>()),
          _view(std::make_unique<View>()) {}

    virtual ~Controller() = default;

protected:
    virtual void configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) = 0;
    virtual void configureComponent(ftxui::Component& component, Communicator& communicator) = 0;

public:
    void createComponent(Communicator& communicator) {
        // DO NOT move it in constructor, there are virtual functions calls

        auto option = ftxui::MenuOption::Vertical();
        configureComponentOption(option, communicator);
        auto menu = _view->createComponent(option);
        configureComponent(menu, communicator);

        // make always focusable
        menu |= ftxui::FocusableWrapper();

        // set keys
        menu |= ftxui::IgnoreEvents({ftxui::Event::Tab, ftxui::Event::TabReverse});

        // add g/G command to scroll begin/end
        menu |= ftxui::CatchEvent([&](ftxui::Event event) {
            if (event == ftxui::Event::Character('g')) {
                communicator.cmdPush(Command::UIEvent, ftxui::Event::Home);
                return true;
            } else if (event == ftxui::Event::Character('G')) {
                communicator.cmdPush(Command::UIEvent, ftxui::Event::End);
                return true;
            }
            return false;
        });

        menu |= ftxui::EventHandler({ftxui::Event::Character('j')});
        _model->setComponent(std::move(menu));
    }

    std::optional<typename Model::EntityID> getSelectedItemID() const {
        return _model->getItemID(_view->getSelectedIndex());
    }

    // TODO: error prone while no data integrity between Model and View
    // Probably worth add size check
    Model::EntityPtr getSelectedItem() const { return _model->getItem(_view->getSelectedIndex()); }

    size_t getItemsAmount() const { return _model->getItems().size(); }

    int getSelectedIndex() const { return _view->getSelectedIndex(); }

    void setItems(const Model::ContainerType& items) {
        _model->setItems(items);
        _view->resetIndex();
        updateNames();
    }

    bool toggleShowID() {
        bool enabled = _view->toggleShowID();
        updateNames();
        return enabled;
    }

    template<typename Key>
    auto createIndexCache() const {
        return _view->template createIndexCache<Key>();
    }

    const ftxui::Component& component() const { return _model->getComponent(); }
    ftxui::Element element(int width) const { return _view->getElement(_model->getComponent(), width); }

    // sorting

    bool sortByField(Sorter::Field field) {
        if (_model->sortByField(field)) {
            updateNames();
            return true;
        }
        return false;
    }

    menu::Sorter::Field toggleSortField() {
        menu::Sorter::Field field{};
        switch (_model->getSortField()) {
        case menu::Sorter::Field::CreationTime:
        case menu::Sorter::Field::UpdateTime:
            field = Sorter::Field::Name;
            break;
        case menu::Sorter::Field::Name:
            field = Sorter::Field::CreationTime;
            break;
        }
        sortByField(field);
        return field;
    }

    bool toggleSortOrder() {
        _model->sortByOrder(!_model->isAscending());
        updateNames();
        return _model->isAscending();
    }

private:
    void updateNames() {
        _view->clearOptions();
        for (const auto& item : _model->getItems()) {
            _view->addOption(std::to_string(item->id), item->getName());
        }
        Log::debug("Update view names, size: {}", _view->getOptionsSize());
    }

    std::unique_ptr<Model> _model;
    std::unique_ptr<View> _view;
};

}  // namespace linux::tui::menu
