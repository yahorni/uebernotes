#include "linux/component.hpp"

namespace ftxui {

class IgnoreEventBase : public ComponentBase {
public:
    // Constructor.
    explicit IgnoreEventBase(std::function<bool(Event)> on_event)
        : on_event_(std::move(on_event)) {}

    // Component implementation.
    bool OnEvent(Event event) override {
        if (on_event_(event)) {
            return false;
        } else {
            return ComponentBase::OnEvent(event);
        }
    }

protected:
    std::function<bool(Event)> on_event_;
};

Component IgnoreEvent(Component child, std::function<bool(Event event)> on_event) {
    auto out = Make<IgnoreEventBase>(std::move(on_event));
    out->Add(std::move(child));
    return out;
}

ComponentDecorator IgnoreEvent(std::function<bool(Event)> on_event) {
    return [on_event = std::move(on_event)](Component child) {
        return IgnoreEvent(std::move(child), [on_event = on_event](Event event) { return on_event(std::move(event)); });
    };
}

}  // namespace ftxui
