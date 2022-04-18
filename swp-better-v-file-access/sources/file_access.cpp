// local
#include "../headers/file_access.hpp"
#include "../headers/utils/convert_error_state.hpp"
#include "../headers/utils/memory.hpp"
#include "../headers/utils/win32/get_proc_address.hpp"

// deps
#include <spdlog/spdlog.h>

// windows
#include <handleapi.h>

// cpp
#include <filesystem>

namespace fs = std::filesystem;

#if USED_METHOD == WINAPI_METHOD

std::wstring str2wstr(const std::string& str) {
  std::string curLocale = setlocale(LC_ALL, NULL);
  setlocale(LC_ALL, "chs");

  const char* pstr = str.c_str();
  int strLen = str.length();
  int bufLen = sizeof(char) * strLen + 1;
  wchar_t* pRes = new wchar_t[bufLen];
  wmemset(pRes, '\0', bufLen);
  //mbstowcs_s(&convertedChars,dest,charNum,source,_TRUNCATE);
  size_t convertLen = 0;
  mbstowcs_s(&convertLen, pRes, bufLen, pstr, _TRUNCATE);
  std::wstring resStr = pRes;
  delete[] pRes;

  setlocale(LC_ALL, curLocale.c_str());
  return resStr;
}

#endif // USED_METHOD == WINAPI_METHOD

namespace swp
{
  char* file_entry::buffer{ nullptr };
}

namespace swp
{
#if USED_METHOD == WINAPI_METHOD

  static HANDLE open_handle(const char* _path, file_access::flags::open_flags _flags, unsigned int _share_flags)
  {
    while (true) {
      DWORD desired_access = 0;
      if ((_flags & file_access::flags::read) != 0) {
        desired_access = 0x80000000;
      }

      if ((_flags & file_access::flags::write) != 0) {
        desired_access |= 0x40000000u;
      }

      if ((_flags & file_access::flags::append) != 0) {
        desired_access |= 4u;
      }

      if (_flags == file_access::flags::read) {
        _share_flags |= 1u;
      }

      DWORD creation_disposition = 3;
      if ((_flags & file_access::flags::append) != 0) {
        creation_disposition = 4;
      } else if ((_flags & file_access::flags::write) != 0) {
        creation_disposition = 2;
      }

      auto name{ str2wstr(_path) };
      auto handle{ CreateFile(name.data(), desired_access, _share_flags, 0, creation_disposition, 0, 0) };

      return handle;
    }
  }

#endif // USED_METHOD == WINAPI_METHOD

  file_entry* open_entry(const char* _path)
  {
    if (not fs::exists(_path)) {
      spdlog::warn("{} not found", _path);
      return nullptr;
    }

#ifdef _DEBUG

    if (not fs::is_regular_file(_path)) {
      spdlog::critical("{} is not a regular file", _path);
      return nullptr;
    }

#endif // _DEBUG

    const auto size{ fs::file_size(_path) };

#if USED_METHOD == WINAPI_METHOD

    auto ptr = new file_entry{
      .size = (size_t)size,
      // .path = _path,
    };

#endif // USED_METHOD == WINAPI_METHOD

#if USED_METHOD == VISION_METHOD

    auto ptr{ (file_entry*)utils::memory::VBaseAlloc_rel(sizeof(file_entry)) };
    memset(ptr, 0, sizeof(file_entry));

    ptr->size = (size_t)size;
    // ptr->path = _path;
    /*ptr->buffer = (char*)utils::memory::VBaseAlloc_rel(ptr->size);
    if (nullptr == ptr->buffer) {
      return nullptr;
    }*/

    std::ifstream{ _path, std::ios::binary }.read(ptr->buffer, ptr->size);

#endif // USED_METHOD == VISION_METHOD

#if USED_METHOD == CPP_METHOD

    auto buffer{ std::make_unique<char[]>(size) };
    auto ptr = new file_entry{
      .size = (size_t)size,
      // .path = _path,
      .buffer = std::move(buffer),
    };

    std::ifstream{ _path, std::ios::binary }.read(ptr->buffer.get(), ptr->size);

#endif // USED_METHOD == CPP_METHOD

    return ptr;
  }
}

auto swp::file_access::open(handle_ptr* _handle, const char* _path, flags::open_flags _flags, uint32_t _share_flags) -> bool
{
  assert(nullptr != _handle);

  spdlog::info("open file: {} / _flags: {} / _share_flags: {}", _path, _flags, _share_flags);

  _handle->hFile = open_entry(_path);

#if USED_METHOD == WINAPI_METHOD

  if (nullptr != _handle->hFile) {
    _handle->hFile->hFile = open_handle(_path, _flags, _share_flags);
    if (INVALID_HANDLE_VALUE == _handle->hFile->hFile) {
      spdlog::critical("open_handle: {}", std::system_category().message(GetLastError()));
      return false;
    }

    _handle->hFile->hMapping = CreateFileMapping(_handle->hFile->hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (NULL == _handle->hFile->hMapping) {
      spdlog::critical("CreateFileMapping: {}", std::system_category().message(GetLastError()));
      return false;
    }

    _handle->hFile->buff = (char*)MapViewOfFile(_handle->hFile->hMapping, FILE_MAP_READ, 0, 0, _handle->hFile->size);
    if (nullptr == _handle->hFile->buff) {
      spdlog::critical("MapViewOfFile: {}", std::system_category().message(GetLastError()));
      return false;
    }
  }

#endif // USED_METHOD == WINAPI_METHOD

  return nullptr != _handle->hFile;
}

auto swp::file_access::get_size(handle_ptr* _handle) -> long
{
  spdlog::debug("get size: {}", _handle->hFile->size);

  return _handle->hFile->size;
}

auto swp::file_access::read(handle_ptr* _handle, void* _buffer, uint32_t _length) -> long
{
  assert(nullptr != _handle);

  auto dst_size{ _length };
  const auto src_size{ _handle->hFile->size };

  const auto pos{ _handle->hFile->pos };
  const auto end_offset{ _handle->hFile->pos + dst_size };

  if (end_offset > src_size) {
    dst_size -= end_offset - src_size;
    if (not dst_size || dst_size >= src_size) {
      _handle->bEOF = true;
      spdlog::debug("EOF");
      return 0;
    }

    spdlog::warn("the result has been shrink, from: {} to: {}", _length, dst_size);
  }

#if USED_METHOD == WINAPI_METHOD

  auto buffer{ _handle->hFile->buff + _handle->hFile->pos };

#endif // USED_METHOD == WINAPI_METHOD

#if USED_METHOD == VISION_METHOD

  auto buffer{ _handle->hFile->buffer + _handle->hFile->pos };

#endif // USED_METHOD == VISION_METHOD

#if USED_METHOD == CPP_METHOD

  auto buffer{ _handle->hFile->buffer.get() + _handle->hFile->pos };

#endif // USED_METHOD == CPP_METHOD

  memcpy((char*)_buffer, (char*)buffer, dst_size);

  _handle->hFile->pos += dst_size;
  _handle->hFile->read += dst_size;

  // spdlog::debug("read: {}", _handle->hFile->read);

  if (_handle->hFile->size <= _handle->hFile->pos) {
    _handle->bEOF = true;
    spdlog::debug("EOF");
  }

  return dst_size;
}

auto swp::file_access::write(handle_ptr* _handle, const void* _buffer, uint32_t uiWriteSize) -> long
{
  assert(nullptr != _handle);

  return 0;
}

auto swp::file_access::get_pos(handle_ptr* _handle) -> long
{
  assert(nullptr != _handle);

  // spdlog::debug("get pos: {}", _handle->hFile->pos);

  return _handle->hFile->pos;
}

auto swp::file_access::set_pos(handle_ptr* _handle, long _offset, std::ios::seekdir _mode) -> bool
{
  assert(nullptr != _handle);

  switch (_mode)
  {
  case std::ios::cur:
    _handle->hFile->pos += _offset;
    break;

  case std::ios::end:
    _handle->hFile->pos = _handle->hFile->size - _offset;
    break;

  default:
    spdlog::warn("[swp::file_access::set_pos] unknown mode: {}", _mode);
    [[fallthrough]];

  case std::ios::beg:
    _handle->hFile->pos = _offset;
    break;
  }

  if (_handle->hFile->size > _handle->hFile->pos) {
    _handle->bEOF = false;
  }

  // spdlog::debug("set pos: {}, offset: {}", _handle->hFile->pos, _offset);

  return true;
}

auto swp::file_access::close(handle_ptr* _handle) -> void
{
  assert(nullptr != _handle);

  spdlog::debug("read {} of {}", _handle->hFile->read, _handle->hFile->size);

#if USED_METHOD == WINAPI_METHOD

  UnmapViewOfFile(_handle->hFile->buff);
  CloseHandle(_handle->hFile->hMapping);
  CloseHandle(_handle->hFile->hFile);

  delete _handle->hFile;

#endif // USED_METHOD == WINAPI_METHOD

#if USED_METHOD == VISION_METHOD

  // utils::memory::VBaseDealloc_rel(_handle->hFile->buffer);
  utils::memory::VBaseDealloc_rel(_handle->hFile);

#endif // USED_METHOD == VISION_METHOD

#if USED_METHOD == CPP_METHOD

  _handle->hFile->buffer.reset();

  delete _handle->hFile;

#endif // USED_METHOD == CPP_METHOD

  _handle->hFile = nullptr;
}

auto swp::file_access::is_eof(handle_ptr* _handle) -> bool
{
  assert(nullptr != _handle);

  return _handle->bEOF;
}

auto swp::file_access::flush(handle_ptr* _handle) -> void
{
  assert(nullptr != _handle);
}

auto swp::file_access::setup(const HMODULE _handle) -> void
{
  handle_error = utils::win32::get_proc_address<handle_error_t>(_handle, "?HandleError@VFileAccess@@SA_NW4FileErrorState@1@@Z");
  
  file_entry::buffer = (char*)utils::memory::VBaseAlloc_rel(541953606);
}

// https://youtu.be/VszJf7g4EC4?list=PLJ4jpRq1tGIkhOSJMX3RRKYHu7HyIDuJq