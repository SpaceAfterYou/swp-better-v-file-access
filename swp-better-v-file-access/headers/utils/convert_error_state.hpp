#pragma once

// local
#include "../file_error_state.hpp"

// cpp
#include <cstdint>

// windows
#include <errhandlingapi.h>
#include <winerror.h>

namespace swp::utils
{
  auto convert_error_state(void) -> const file_error_state
  {
    auto code{ GetLastError() };
    if (code > ERROR_IO_DEVICE) {
      if (code >= ERROR_DISK_OPERATION_FAILED) {
        if (code <= ERROR_DISK_RESET_FAILED) {
          return file_error_state::retry;
        }

        if (code > ERROR_NO_INHERITANCE && (code <= ERROR_DISK_CORRUPT || code == ERROR_UNRECOGNIZED_MEDIA)) {
          return file_error_state::fatal;
        }
      }

      return file_error_state::unknown;
    }

    if (code == ERROR_IO_DEVICE) {
      return file_error_state::retry;
    }

    switch (code)
    {
    case ERROR_SUCCESS:
      return file_error_state::none;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
      return file_error_state::filenotfound;
    case ERROR_TOO_MANY_OPEN_FILES:
    case ERROR_INVALID_HANDLE:
    case ERROR_INVALID_DRIVE:
    case ERROR_NOT_DOS_DISK:
      return file_error_state::fatal;
    case ERROR_ACCESS_DENIED:
    case ERROR_WRITE_PROTECT:
      return file_error_state::accessdenied;
    case ERROR_CURRENT_DIRECTORY:
    case ERROR_NOT_SAME_DEVICE:
    case ERROR_NO_MORE_FILES:
    case ERROR_SEEK:
    case ERROR_HANDLE_EOF:
    case ERROR_HANDLE_DISK_FULL:
    case ERROR_FILE_EXISTS:
    case ERROR_CANNOT_MAKE:
    case ERROR_NEGATIVE_SEEK:
      return file_error_state::invalid_fileop;
    case ERROR_BAD_UNIT:
    case ERROR_NOT_READY:
    case ERROR_DRIVE_LOCKED:
      return file_error_state::retry;
    case ERROR_WRITE_FAULT:
    case ERROR_READ_FAULT:
      return file_error_state::readwritefault;
      break;
    case ERROR_DISK_CHANGE:
      return file_error_state::wrong_disk;
      break;
    case ERROR_DISK_FULL:
      return file_error_state::diskfull;
    default:
      return file_error_state::unknown;
    }
  }
}