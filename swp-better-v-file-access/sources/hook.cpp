#include "../headers/hook.hpp"

auto swp::hook::install(FARPROC _original_address, const PVOID _new_address) -> void
{
  if (not not DetourAttach(reinterpret_cast<PVOID*>(&_original_address), _new_address)) {
    spdlog::critical("DetourAttach: {}", std::system_category().message(GetLastError()));
    return;
  }
}

// https://youtu.be/BEcv39gKAkw?list=RDBEcv39gKAkw
