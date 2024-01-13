#pragma once

#include "ftxui/component.hpp"
#include "linux/logger.hpp"
#include "linux/tui/communicator.hpp"
#include "linux/tui/menu/sorter.hpp"
#include "linux/tui/mvc.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

#include <memory>
#include <string>
#include <utility>

namespace linux::tui::menu {

template<typename Model, typename View>
class Controller : public mvc::Controller {
public:
    explicit Controller(std::string name)
        : mvc::Controller(std::move(name)),
          _model(std::make_unique<Model>()),
          _view(std::make_unique<View>()) {}

    virtual ~Controller() = default;

protected:
    virtual void configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) = 0;

public:
    void createComponent(Communicator& communicator) {
        // DO NOT move it in constructor, there are virtual functions calls

        auto option = ftxui::MenuOption::Vertical();
        configureComponentOption(option, communicator);

        option.on_change = [&]() {
            communicator.cmd.push(_viewCMD);
            Log::debug("[{}] Selected ID: {}", name(), *getSelectedItemID());
        };

        auto menu = _view->createComponent(option);
        configureComponent(menu, communicator);

        // make always focusable
        menu |= ftxui::FocusableWrapper();

        // set keys
        menu |= ftxui::IgnoreEvents({ftxui::Event::Tab, ftxui::Event::TabReverse});

        // add g/G command to scroll begin/end
        menu |= ftxui::CatchEvent([&](ftxui::Event event) {
            if (event == ftxui::Event::Character('g')) {
                communicator.ui.push(ftxui::Event::Home);
                return true;
            } else if (event == ftxui::Event::Character('G')) {
                communicator.ui.push(ftxui::Event::End);
                return true;
            }
            return false;
        });

        menu |= ftxui::EventHandler({ftxui::Event::Character('j')});
    }

    std::optional<typename Model::EntityID> getSelectedItemID() const {
        return _model->getItemID(_view->getSelectedIndex());
    }

    // TODO: error prone while no data integrity between Model and View
    // Probably worth add size check
    Model::EntityPtr getSelectedItem() const { return _model->getItem(_view->getSelectedIndex()); }

    size_t getItemsAmount() const { return _model->getItems().size(); }

    int getSelectedIndex() const { return _view->getSelectedIndex(); }

    void setItems(const Model::Container& items) {
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

    const ftxui::Component& component() const { return _view->component(); }
    ftxui::Element element(int width) const { return _view->element(name(), width); }

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

    void configureComponent(ftxui::Component& component, Communicator& communicator) {
        component |= ftxui::CatchEvent([&](ftxui::Event event) {
            if (event == ftxui::Event::Character('r')) {
                std::string message;
                if (auto itemID = getSelectedItemID(); itemID) {
                    message = std::format("[{}] Refreshed ID: {}", name(), *itemID);
                } else {
                    message = std::format("[{}] Nothing to refresh", name());
                }
                communicator.cmd.push(_refreshCMD);
                communicator.ntf.push(std::move(message));
                return true;
            } else if (event == ftxui::Event::Character('s')) {
                auto field = toggleSortField();
                communicator.cmd.push(_sortCMD);
                switch (field) {
                case menu::Sorter::Field::CreationTime:
                case menu::Sorter::Field::UpdateTime:
                    communicator.ntf.push(std::format("[{}] Sort by creation time", name()));
                    break;
                case menu::Sorter::Field::Name:
                    communicator.ntf.push(std::format("[{}] Sort by name", name()));
                    break;
                }
                return true;
            } else if (event == ftxui::Event::Character('o')) {
                bool isAscending = toggleSortOrder();
                communicator.cmd.push(_sortCMD);
                communicator.ntf.push(std::format("[{}] Ascending sort order: {}", name(), isAscending));
                return true;
            } else if (event == ftxui::Event::Character('i')) {
                bool enabled = toggleShowID();
                communicator.cmd.push(_viewCMD);
                communicator.ntf.push(std::format("[{}] Show ID: {}", name(), enabled));
                return true;
            }
            return false;
        });
    }

    std::unique_ptr<Model> _model;
    std::unique_ptr<View> _view;

protected:
    Command _refreshCMD;
    Command _sortCMD;
    Command _viewCMD;
};

}  // namespace linux::tui::menu
