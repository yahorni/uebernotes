#include "linux/pager.hpp"

#include "linux/logger.hpp"

#include <ftxui/component/event.hpp>

#include <memory>
#include <sstream>
#include <utility>

// https://github.com/ArthurSonzogni/FTXUI/issues/247
// Feature request: Support multi-paragraph text layout #247

namespace ftxui {

PagerArea::PagerArea(Elements children, int& shift)
    : _shift(shift),
      _linesAmount(children.size()) {
    children_ = {vbox(std::move(children))};
}

void PagerArea::normalizeShift(const Box& box) {
    int height = box.y_max - box.y_min + 1;
    if (_shift < 0) {
        _shift = 0;
    } else if (_linesAmount > height) {
        if (_shift > _linesAmount - height) {
            // to not overscroll when shift is bigger than remaining lines
            _shift = _linesAmount - height;
        }
    } else {
        // shift > 0, but _linesAmount <= y_max, so need to scroll
        _shift = 0;
    }
}

void PagerArea::SetBox(Box box) {
    normalizeShift(box);
    box.y_min -= _shift;

    Node::SetBox(box);
    children_[0]->SetBox(box);
}

Element pagerarea(Elements children, int& shift) { return std::make_shared<PagerArea>(std::move(children), shift); }

namespace {
Elements Split(const std::string& content) {
    Elements output;
    std::stringstream ss(content);
    std::string word;
    while (std::getline(ss, word)) {
        output.push_back(text(word));
    }
    return output;
}
}  // namespace

PagerBase::PagerBase(int& shift, const std::string& content)
    : _shift(shift),
      _content(content) {}

Element PagerBase::Render() { return pagerarea(Split(_content), _shift); }

bool PagerBase::OnEvent(Event event) {
    if (event == Event::Character('j')) {
        _shift++;
        return true;
    } else if (event == Event::Character('k')) {
        _shift--;
        return true;
    } else if (event == Event::Character('g')) {
        _shift = 0;
        return true;
    } else if (event == Event::Character('G')) {
        _shift = 99999;  // TODO: remove random huge value
        return true;
    }
    return false;
}

bool PagerBase::Focusable() const { return !_content.empty(); }

Component Pager(int& shift, const std::string& content) { return Make<PagerBase>(shift, content); }

}  // namespace ftxui
