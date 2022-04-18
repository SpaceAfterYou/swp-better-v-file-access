#pragma once

#include <cstdint>

namespace swp
{
  /// <summary>
  /// Error code.
  /// </summary>
  enum class file_error_state : uint32_t
  {
    /// <summary>
    /// No error
    /// </summary>
    none,

    /// <summary>
    /// Unknown error
    /// </summary>
    unknown,

    /// <summary>
    /// No disk in the drive
    /// </summary>
    no_disk,

    /// <summary>
    /// Wrong disk in the drive
    /// </summary>
    wrong_disk,

    /// <summary>
    /// Retry error occurred
    /// </summary>
    retry,

    /// <summary>
    /// Fatal error occurred
    /// </summary>
    fatal,

    /// <summary>
    /// File not found
    /// </summary>
    filenotfound,

    /// <summary>
    /// Access to file denied
    /// </summary>
    accessdenied,

    /// <summary>
    /// Disk full
    /// </summary>
    diskfull,

    /// <summary>
    /// Read/Write Fault (e.g. media broken)
    /// </summary>
    readwritefault,

    /// <summary>
    /// General invalid file operation
    /// </summary>
    invalid_fileop,

    /// <summary>
    /// Media not ready (e.g. no SD card)
    /// </summary>
    media_not_ready,
  };
}