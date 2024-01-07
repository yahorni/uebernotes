#pragma once

// https://stackoverflow.com/questions/31357515/how-constructor-works-in-private-inheritance

namespace linux::tui::utils {

struct NonCopyable {
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = delete;

    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
};

}  // namespace linux::tui::utils
