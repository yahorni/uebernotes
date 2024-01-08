#pragma once

#include <optional>

namespace ftxui::utils {

template<typename T>
using OptionalCRef = std::optional<std::reference_wrapper<const T>>;

}  // namespace ftxui::utils
