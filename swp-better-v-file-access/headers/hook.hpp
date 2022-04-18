#pragma once

// windows
#include <WTypesbase.h>

// deps
#include <detours/detours.h>
#include <spdlog/spdlog.h>

namespace swp::hook
{
  auto install(const FARPROC _original_address, const PVOID _new_address) -> void;
}