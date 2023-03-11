#include "linux/tui.hpp"

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

#include "ftxui/component/component.hpp"           // for CatchEvent, Renderer, operator|=
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

namespace linux {

TUI::TUI(const core::Config&) {}

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

    screen.Loop(renderer);
    return true;
}

}  // namespace linux
