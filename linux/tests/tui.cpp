#include <catch2/catch_test_macros.hpp>

#include "linux/logger.hpp"
#include "linux/pager.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

static const char lorem[] =
    R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.)";

TEST_CASE("tui-scroll", "[.]") {
    using namespace ftxui;

    int argc = 1;
    const char* argv[]{"test"};
    linux::Log::init(argc, argv);
    linux::Log::info("=== TEST STARTED ===");

    std::string fillerText = "0 ==> FIRST ENTRY <==\n";
    size_t amount = 64;
    for (size_t i = 0; i < amount; ++i) {
        fillerText += std::to_string(i + 1) + " ==> " + lorem + "\n";
    }
    fillerText += std::to_string(amount + 1) + " ==> LAST ENTRY <==\n";

    auto notePreview = Pager(fillerText);
    // 1. to receive OnEvent calls Component should be passed as a first argument to Renderer
    // 2. to be visible inside vbox Component should be decorated by *flex decorator
    auto previewPane = Renderer(notePreview, [&] {
        return vbox({
                   hcenter(bold(text("Note preview"))),    // consider using "window"
                   separator(),                            //
                   notePreview->Render() | flex | yframe,  //
               }) |
               border;
    });

    auto screen = ScreenInteractive::Fullscreen();

    auto component = CatchEvent(previewPane, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(component);
}
