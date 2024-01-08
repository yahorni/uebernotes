#include <catch2/catch_test_macros.hpp>

#include "ftxui/component.hpp"
#include "linux/logger.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>

static const char lorem[] =
    R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.)";

TEST_CASE("manual-tui", "[.]") {
    using namespace ftxui;  // NOLINT

    int argc = 1;
    const char* argv[]{"test"};
    linux::Log::init(argc, argv);
    linux::Log::info("=== TEST STARTED ===");

    SECTION("scroll") {
        size_t linesAmount = 100;

        std::string testString = "0 ==> FIRST ENTRY <==\n==> with second line <==\n";
        for (size_t i = 0; i < linesAmount; ++i) {
            testString += std::to_string(i + 1) + " ==> " + lorem + "\n";
        }
        testString += std::to_string(linesAmount + 1) + " ==> LAST ENTRY <==\n";

        int shift1 = 0;
        bool wrap1 = true;
        auto notePreview1 = Pager(testString, shift1, wrap1);

        std::vector<std::string> testLines;
        testLines.emplace_back("0 ==> FIRST ENTRY <==\n==> with second line <==");
        for (size_t i = 0; i < linesAmount; ++i) {
            testLines.emplace_back(std::to_string(i + 1) + " ==> " + lorem);
        }
        testLines.emplace_back(std::to_string(linesAmount + 1) + " ==> LAST ENTRY <==\n");

        int shift2 = 0;
        bool wrap2 = true;
        auto notePreview2 = Pager(testLines, shift2, wrap2);

        auto container = Container::Horizontal({notePreview1, notePreview2});

        int paneWidth = Terminal::Size().dimx / 2;

        // 1. to receive OnEvent calls Component should be passed as a first argument to Renderer
        // 2. to be visible inside vbox Component should be decorated by *flex decorator
        // 3. to be scrollable Component should be decorated by yframe
        auto layout = Renderer(container, [&] {
            return hbox({
                vbox({
                    hcenter(bold(text(std::format("Note preview with one string ({})",
                                                  notePreview1->Focused())))),  // consider using "window"
                    separator(),                                                //
                    notePreview1->Render() | flex | yframe,                     //
                }) | border |
                    size(WIDTH, EQUAL, paneWidth),
                vbox({
                    hcenter(bold(text(std::format("Note preview with string vector ({})",
                                                  notePreview2->Focused())))),  // consider using "window"
                    separator(),                                                //
                    notePreview2->Render() | flex | yframe,                     //
                }) | border |
                    size(WIDTH, EQUAL, paneWidth),
            });
        });

        auto screen = ScreenInteractive::Fullscreen();

        auto component = CatchEvent(layout, [&](Event event) {
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

    SECTION("switch-with-tab") {
        auto ignoreTabDecorator =
            IgnoreEvent([&](Event event) { return event == Event::Tab || event == Event::TabReverse; });

        std::vector<std::string> menu1Opts{"m1o1", "m1o2", "m1o3"};
        std::vector<std::string> menu2Opts{"m2o1", "m2o2", "m2o3", "m2o4", "m2o5"};
        std::vector<std::string> menu3Opts{};
        int selected1 = 0;
        int selected2 = 0;
        int selected3 = 0;

        MenuOption menuOpt1;
        MenuOption menuOpt2;
        MenuOption menuOpt3;

        menuOpt3.elements_prefix = [] { return text("[empty menu]"); };
        menuOpt3.on_enter = [&]() {
            menu3Opts.push_back("entry");
            return true;
        };

        auto menu1 = Menu(&menu1Opts, &selected1, menuOpt1) | ignoreTabDecorator;
        auto menu2_ = Menu(&menu2Opts, &selected2, menuOpt2) | ignoreTabDecorator;
        auto menu2 = FocusableWrapper(menu2_);
        auto menu3 = Menu(&menu3Opts, &selected3, menuOpt3) | ignoreTabDecorator | FocusableWrapper();

        auto pane = Renderer([&](bool focused) {
            if (focused) {
                return text("focused pane") | bold;
            }
            return text("usual pane");
        });

        auto container = Container::Horizontal({menu1, menu2, menu3, pane});

        auto screen = ScreenInteractive::Fullscreen();

        auto component = CatchEvent(container, [&](Event event) {
            if (event == Event::Character('q')) {
                screen.ExitLoopClosure()();
                return true;
            }
            return false;
        });

        screen.Loop(component);
    }
}
