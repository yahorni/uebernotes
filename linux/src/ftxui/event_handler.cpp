#include "ftxui/component.hpp"

namespace ftxui {

class EventHandlerBase : public ComponentBase {
public:
    explicit EventHandlerBase(Component child, std::vector<Event> events)
        : _events(std::move(events)) {
        Add(std::move(child));
    }

    bool OnEvent(Event event) override {
        for (const auto& ev : _events) {
            bool result = ActiveChild() && ActiveChild()->OnEvent(event);
            return ev == event ? true : result;
        }
        return false;
    }

private:
    std::vector<Event> _events;
};

Component EventHandler(Component child, const std::vector<Event>& events) {
    return Make<EventHandlerBase>(std::move(child), events);
}

ComponentDecorator EventHandler(const std::vector<Event>& events) {
    return [events](Component child) { return EventHandler(std::move(child), events); };
}

}  // namespace ftxui
