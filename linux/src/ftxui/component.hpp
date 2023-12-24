#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <string>
#include <vector>

namespace ftxui {

/* Scrollable text area
 * 1. to make it scroll use 'yframe'
 * 2. to show entries use '*flex'
 */
Element pagerarea(const std::string& content, int& shift, bool& wrap);
Element pagerarea(const std::vector<std::string>& content, int& shift, bool& wrap);
Component Pager(const std::string& content, int& shift, bool& wrap);
Component Pager(const std::vector<std::string>& lines, int& shift, bool& wrap);

/* Based on CatchEvent but the logic is reversed
 * to ignore some event which are implemented for Component */
Component IgnoreEvent(Component child, std::function<bool(Event event)> on_event);
ComponentDecorator IgnoreEvent(std::function<bool(Event)> on_event);

/* To make focusable component, which are not focusable by default */
Component FocusableWrapper(Component child);
ComponentDecorator FocusableWrapper();

Element linefiller(char c);

}  // namespace ftxui
