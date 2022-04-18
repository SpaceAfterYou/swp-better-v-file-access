// local
#include "../headers/plugin.hpp"
#include "../headers/file_access.hpp"
#include "../headers/hooks.hpp"

// deps
#include <detours/detours.h>

namespace swp
{
  auto plugin::main(void) const -> void
  {
    if (not not DetourTransactionBegin()) {
      spdlog::error("DetourTransactionBegin {}", GetLastError());
      return;
    }

    if (not not DetourUpdateThread(GetCurrentThread())) {
      spdlog::error("DetourUpdateThread {}", GetLastError());
      return;
    }

    hooks::base_dll();

    if (not not DetourTransactionCommit()) {
      spdlog::error("DetourTransactionCommit {}", GetLastError());
      return;
    }
  }
};

// https://youtu.be/_2IDE97E-qU?list=PLJ4jpRq1tGIkhOSJMX3RRKYHu7HyIDuJq