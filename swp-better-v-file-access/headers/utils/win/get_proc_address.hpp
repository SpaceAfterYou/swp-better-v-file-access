#pragma once

// windows
#include <minwindef.h>

// deps
#include <spdlog/spdlog.h>

namespace swp::utils::win
{
  template <typename T>
  constexpr auto get_proc_address(const HMODULE _handle, const char* _proc_name) -> T
  {
    const auto ptr{ GetProcAddress(_handle, _proc_name) };
    if (not ptr) {
      spdlog::critical("can't get proc: {}", _proc_name);
    }

    return reinterpret_cast<T>(ptr);
  }
}