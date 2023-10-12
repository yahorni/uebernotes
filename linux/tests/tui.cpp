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

TEST_CASE("manual-tui", "[.]") {
    using namespace ftxui;

    int argc = 1;
    const char* argv[]{"test"};
    linux::Log::init(argc, argv);
    linux::Log::info("=== TEST STARTED ===");

    SECTION("scroll") {
        std::string fillerText = "0 ==> FIRST ENTRY <==\n";
        size_t amount = 64;
        for (size_t i = 0; i < amount; ++i) {
            fillerText += std::to_string(i + 1) + " ==> " + lorem + "\n";
        }
        fillerText += std::to_string(amount + 1) + " ==> LAST ENTRY <==\n";

        int shift = 0;
        auto notePreview = Pager(shift, fillerText);
        // 1. to receive OnEvent calls Component should be passed as a first argument to Renderer
        // 2. to be visible inside vbox Component should be decorated by *flex decorator
        // 3. to be scrollable Component should be decorated by yframe
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

    SECTION("focus") {
        auto focusBorder = borderStyled(BorderStyle::HEAVY);
        auto unfocusBorder = borderStyled(BorderStyle::LIGHT);

        auto pane1 = Renderer([&](bool) { return paragraph(">>> FOCUSABLE PANE 1 <<<"); });
        auto pane2 = Renderer([&] { return paragraph(">>> NON-FOCUSABLE PANE 2 <<<"); });
        auto pane3 = Renderer([&](bool) { return paragraph(">>> FOCUSABLE PANE 3 <<<"); });

        auto container = Container::Horizontal({pane1, pane2, pane3});

        auto renderer = Renderer(container, [&] {
            int widthSixth = Terminal::Size().dimx / 6;

            auto pane1Wrapped = [&] {
                return vbox({
                           hcenter(bold(text("Pane 1"))),  //
                           separator(),                    //
                           pane1->Render(),                //
                       }) |
                       yflex | size(WIDTH, EQUAL, widthSixth) | (pane1->Focused() ? focusBorder : unfocusBorder);
            };

            auto pane2Wrapped = [&] {
                return vbox({
                           hcenter(bold(text("Pane 2"))),  //
                           separator(),                    //
                           pane2->Render(),                //
                       }) |
                       yflex | size(WIDTH, EQUAL, 2 * widthSixth) | (pane2->Focused() ? focusBorder : unfocusBorder);
            };

            return vbox({hbox({pane1Wrapped(), pane2Wrapped(),
                               pane3->Render() | (pane3->Focused() ? focusBorder : unfocusBorder)}) |
                             yflex,
                         paragraph("NON-FOCUSABLE LINE") | size(HEIGHT, EQUAL, 1)});
        });

        auto screen = ScreenInteractive::Fullscreen();

        auto component = CatchEvent(renderer, [&](Event event) {
            if (event == Event::Character('q')) {
                screen.ExitLoopClosure()();
                return true;
            }
            return false;
        });

        screen.Loop(component);
    }
}
