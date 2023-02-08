#ifndef GEOM_2D_UTIL_H_
#define GEOM_2D_UTIL_H_

#include <format>
#include <iostream>
#include <string>

namespace geom_2d {

template <typename... Args>
void debug(const std::string_view& str, Args&&... args) {
#if GEOM_DEBUG
  std::cout << std::vformat(str, std::make_format_args(args...));
#endif
}


};      // namespace geom_2d
#endif  // GEOM_2D_UTIL_H_
