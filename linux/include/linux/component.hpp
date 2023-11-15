#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <string>

namespace ftxui {

/* Scrollable text area
 * 1. to make it scroll use 'yframe'
 * 2. to show entries use '*flex'
 * TODO: text wrapping */
Element pagerarea(Elements children, int& shift);
Component Pager(int& shift, const std::string& content);

/* Based on CatchEvent but the logic is reversed
 * to ignore some event which are implemented for Component */
Component IgnoreEvent(Component child, std::function<bool(Event event)> on_event);
ComponentDecorator IgnoreEvent(std::function<bool(Event)> on_event);

/* To make focusable component, which are not focusable by default */
Component FocusableWrapper(Component child);
ComponentDecorator FocusableWrapper();

}  // namespace ftxui
