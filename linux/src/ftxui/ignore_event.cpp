#include "ftxui/component.hpp"

namespace ftxui {

class IgnoreEventBase : public ComponentBase {
public:
    // Constructor.
    explicit IgnoreEventBase(Component child, std::function<bool(Event)> on_event)
        : on_event_(std::move(on_event)) {
        Add(std::move(child));
    }

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
    return Make<IgnoreEventBase>(std::move(child), std::move(on_event));
}

ComponentDecorator IgnoreEvent(std::function<bool(Event)> on_event) {
    return [on_event = std::move(on_event)](Component child) {
        return IgnoreEvent(std::move(child), [on_event = on_event](Event event) { return on_event(std::move(event)); });
    };
}

ComponentDecorator IgnoreEvents(const std::vector<Event>& events) {
    return IgnoreEvent([events](ftxui::Event event) {
        for (const auto& ignored : events) {
            if (event == ignored) {
                return true;
            }
        }
        return false;
    });
}

}  // namespace ftxui
