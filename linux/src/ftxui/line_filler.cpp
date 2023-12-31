#include "ftxui/component.hpp"

#include <ftxui/component/component.hpp>

namespace ftxui {

class LineFiller : public Node {
public:
    explicit LineFiller(char c = ' ')
        : _c(c) {}

    void ComputeRequirement() override {
        requirement_.min_y = 1;
        requirement_.flex_grow_x = 1;
    }
    void Render(Screen& screen) final {
        int y = box_.y_min;
        for (int x = box_.x_min; x <= box_.x_max; ++x) {
            screen.PixelAt(x, y) = Pixel();
            screen.PixelAt(x, y).character = _c;
        }
    }

private:
    char _c;
};

Element linefiller(char c) { return std::make_shared<LineFiller>(c); }

}  // namespace ftxui
