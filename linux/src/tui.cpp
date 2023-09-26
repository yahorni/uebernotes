#include "linux/tui.hpp"

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

#include "ftxui/component/component.hpp"           // for CatchEvent, Renderer, operator|=
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

// ftxui/dom/elements.hpp - list of elements to create layouts
// ftxui/dom/component.hpp - interactive components to respond to events

namespace linux {

TUI::TUI(const core::AppContext& context)
    : _storage(context) {}

bool TUI::run() {
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    auto left = Renderer([] { return text("left") | center; });
    auto middle = Renderer([] { return text("middle") | center; });
    auto right = Renderer([] { return text("right") | center; });

    int left_size = 20;
    int middle_size = 20;

    auto container = right;
    container = ResizableSplitLeft(middle, container, &middle_size);
    container = ResizableSplitLeft(left, container, &left_size);

    auto renderer = Renderer(container, [&] { return container->Render() | border; });

    auto component = CatchEvent(renderer, [&](Event event) {
      if (event == Event::Character('q')) {
        screen.ExitLoopClosure()();
        return true;
      }
      return false;
    });

    screen.Loop(component);
    return true;
}

}  // namespace linux
