#pragma once

// windows
#include <minwindef.h>

namespace swp::utils
{
  class memory final
  {
  public:

#pragma region Static Types

    using VBaseAlloc_rel_t = auto(*)(unsigned int _size) -> void*;
    using VBaseDealloc_rel_t = auto(*)(void* _ptr) -> void;

#pragma endregion Static Types

#pragma region Static Members

    static VBaseAlloc_rel_t VBaseAlloc_rel;
    static VBaseDealloc_rel_t VBaseDealloc_rel;

#pragma endregion Static Members

#pragma region Static Methods

    static auto setup(const HMODULE _handle) -> void;

#pragma endregion Static Methods

  };
}