#pragma once

// local
#include "file_error_state.hpp"

// windows
#include <minwindef.h>

// cpp
#include <fstream>
#include <filesystem>
#include <functional>

#define VISION_METHOD 1
#define WINAPI_METHOD 2
#define CPP_METHOD 3

#define USED_METHOD VISION_METHOD

namespace swp
{
  struct file_entry final
  {

#if USED_METHOD == WINAPI_METHOD

    HANDLE hFile{};
    HANDLE hMapping{};
    char* buff{};

#endif // USED_METHOD == WINAPI_METHOD

    size_t read{};
    size_t size{};
    size_t pos{};
    
    // std::filesystem::path path;

#if USED_METHOD == VISION_METHOD

    static char* buffer;

#endif // USED_METHOD == VISION_METHOD

#if USED_METHOD == CPP_METHOD

    std::unique_ptr<char[]> buffer{};

#endif // USED_METHOD == CPP_METHOD

  };

  struct handle_ptr final
  {
    file_entry* hFile;
    bool bUseReadCache;
    bool bEOF;
  };

  class file_access final
  {
  public:

#pragma region Enums

    class flags
    {
    public:
      enum open_flags
      {
        read = 0x1,
        write = 0x2,
        append = 0x4,
        create = 0x1000,
        create_if_not_exist = 0x2000
      };
    };

#pragma endregion Enums

#pragma region Types

    using handle_error_t = auto(*)(const file_error_state) -> bool;

#pragma endregion Types

#pragma region Static Methods

    static auto open(handle_ptr* _handle, const char* _path, flags::open_flags _flags, [[maybe_unused]] uint32_t _share_flags = 0) -> bool;
    static auto get_size(handle_ptr* _handle) -> long;
    static auto read(handle_ptr* _handle, void* _buffer, uint32_t _length) -> long;
    static auto write(handle_ptr* _handle, const void* _buffer, uint32_t _length) -> long;
    static auto get_pos(handle_ptr* _handle) -> long;
    static auto set_pos(handle_ptr* _handle, long _offset, std::ios::seekdir _mode) -> bool;
    static auto close(handle_ptr* _handle) -> void;
    static auto is_eof(handle_ptr* _handle) -> bool;
    static auto flush(handle_ptr* _handle) -> void;

    static auto setup(const HMODULE _handle) -> void;

#pragma endregion Static Methods

#pragma region Static Members

    inline static handle_error_t handle_error{ nullptr };

#pragma endregion Static Members

  };
}