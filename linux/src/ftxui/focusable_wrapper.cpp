#include "ftxui/component.hpp"

namespace ftxui {

class FocusableWrapperBase : public ComponentBase {
public:
    explicit FocusableWrapperBase(Component child) { Add(std::move(child)); }

    Element Render() override {
        if (children_.size()) {
            return children_[0]->Render();
        }
        return text("Empty container");
    }

    bool Focusable() const override { return true; }
};

Component FocusableWrapper(Component child) { return Make<FocusableWrapperBase>(std::move(child)); }

ComponentDecorator FocusableWrapper() {
  return [](Component component) {
    return FocusableWrapper(std::move(component));
  };
}

}  // namespace ftxui
