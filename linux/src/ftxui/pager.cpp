#include "ftxui/component.hpp"

#include "ftxui/utils/optional_ref.hpp"

#include <ftxui/component/component.hpp>

#include <functional>

// Feature request: Support multi-paragraph text layout #247
// https://github.com/ArthurSonzogni/FTXUI/issues/247
//
// How to display a multi-line string? #68
// https://github.com/ArthurSonzogni/FTXUI/issues/68

namespace ftxui {

// TODO:
// add horizontal scroll
// mouse scroll support
// test that it's working not only as a one-time created object
// and use it without creating a new instance on each Render()
class PagerArea : public Node {
public:
    PagerArea(const utils::OptionalCRef<std::string>& content, int& shift, bool& wrap)
        : PagerArea(shift, wrap) {
        // copies ref wrapper
        _stringContent = content;
    }

    PagerArea(const std::vector<std::string>& content, int& shift, bool& wrap)
        : PagerArea(shift, wrap) {
        // creates optional with ref wrapper
        _vectorContent = std::cref(content);
    }

    void ComputeRequirement() override {
        int maxLineLength = constructContentContainer();

        requirement_.min_y = _lines.size();
        requirement_.min_x = maxLineLength;
    }

    void Render(Screen& screen) override {
        int x = box_.x_min;
        int y = box_.y_min;

        calculateShift();

        auto lineIt = _lines.begin();
        std::advance(lineIt, _shift);
        for (; lineIt != _lines.end(); ++lineIt) {
            if (y > box_.y_max) {
                return;
            }

            x = box_.x_min;

            for (const auto& it : Utf8ToGlyphs(std::string(*lineIt))) {
                if (x > box_.x_max) {
                    if (!_wrap) {
                        break;
                    }
                    x = box_.x_min;
                    y++;
                    if (y > box_.y_max) {
                        return;
                    }
                }

                screen.PixelAt(x, y).character = it;
                x++;
            }
            y++;
        }
    }

private:
    PagerArea(int& shift, bool& wrap)
        : _shift(shift),
          _wrap(wrap) {}

    int constructContentContainer() {
        _lines.clear();
        int maxLineLength = 0;

        if (_stringContent.has_value()) {
            preprocessContent(_stringContent->get(), maxLineLength);
        } else if (_vectorContent.has_value()) {
            auto& values = _vectorContent.value().get();
            for (const auto& value : values) {
                preprocessContent(value, maxLineLength);
            }
        }

        return maxLineLength;
    }

    void preprocessContent(const std::string& content, int& maxLineLength) {
        int maxLength = 0;
        int lineViewIndex = 0;
        for (size_t i = 0; i < content.size(); ++i) {
            if (content[i] == '\n' || i == content.size() - 1) {
                // create new string_view, shift lineViewIndex
                size_t length = i - lineViewIndex + (i == content.size() - 1);
                std::string_view lineView{content.data() + lineViewIndex, length};
                lineViewIndex = i + 1;

                // populate string_view vector
                _lines.push_back(lineView);

                // recalculate max lines length
                int unicodeLength = string_width(std::string(lineView));
                if (unicodeLength > maxLength) {
                    maxLength = unicodeLength;
                }
            }
        }

        maxLineLength = std::max(maxLength, maxLineLength);
    }

    void calculateShift() {
        int size = _lines.size();
        if (_shift < 0 || size == 0) {
            _shift = 0;
        } else if (_shift >= size) {
            _shift = size - 1;
        }
    }

    int& _shift;
    bool& _wrap;

    utils::OptionalCRef<std::string> _stringContent;
    utils::OptionalCRef<std::vector<std::string>> _vectorContent;

    std::vector<std::string_view> _lines;
};

Element pagerarea(const utils::OptionalCRef<std::string>& content, int& shift, bool& wrap) {
    return std::make_shared<PagerArea>(content, shift, wrap);
}

Element pagerarea(const std::vector<std::string>& content, int& shift, bool& wrap) {
    return std::make_shared<PagerArea>(content, shift, wrap);
}

template<typename T>
class PagerBase : public ComponentBase {
public:
    PagerBase(const T& content, int& shift, bool& wrap)
        : _shift(shift),
          _wrap(wrap),
          _content(content) {}

private:
    Element Render() final { return pagerarea(_content, _shift, _wrap); }

    bool OnEvent(Event event) final {
        if (event == Event::Character('w')) {
            _wrap = !_wrap;
            return true;
        } else if (event == Event::Character('j')) {
            _shift++;
            return true;
        } else if (event == Event::Character('k')) {
            _shift--;
            return true;
        } else if (event == Event::Character('g')) {
            _shift = 0;
            return true;
        } else if (event == Event::Character('G')) {
            // TODO: remove random huge value
            _shift = 99999;
            return true;
        }
        return false;
    }

    bool Focusable() const override { return true; }

    int& _shift;
    bool& _wrap;

    const T& _content;
};

Component Pager(const utils::OptionalCRef<std::string>& content, int& shift, bool& wrap) {
    return Make<PagerBase<utils::OptionalCRef<std::string>>>(content, shift, wrap);
}

Component Pager(const std::vector<std::string>& content, int& shift, bool& wrap) {
    return Make<PagerBase<std::vector<std::string>>>(content, shift, wrap);
}

}  // namespace ftxui
