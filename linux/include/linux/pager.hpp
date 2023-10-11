#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <string>

namespace ftxui {

class PagerArea : public Node {
public:
    explicit PagerArea(Elements children, int& shift);

    void SetBox(Box box) final;

private:
    void NormalizeShift(int yMax);

    int& _shift;
    const int _linesAmount;
};

Element pagerarea(Elements children, int& shift);

// 1. to make it scroll use 'yframe'
// 2. to show entries use 'flex'
class PagerBase : public ComponentBase {
public:
    explicit PagerBase(const std::string& content);

private:
    Element Render() final;
    bool OnEvent(Event event) final;
    bool Focusable() const final { return true; }

    const std::string& _content;
    int _shift = 0;
};

Component Pager(const std::string& content);

}  // namespace ftxui
