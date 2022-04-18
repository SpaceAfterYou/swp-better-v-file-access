// local
#include "../../headers/utils/memory.hpp"
#include "../../headers/utils/win/get_proc_address.hpp"

// windows
#include <libloaderapi.h>

// deps
#include <spdlog/spdlog.h>

auto swp::utils::memory::setup(const HMODULE _handle) -> void
{
  VBaseAlloc_rel = utils::win::get_proc_address<VBaseAlloc_rel_t>(_handle, "?VBaseAlloc_rel@@YAPAXI@Z");
  VBaseDealloc_rel = utils::win::get_proc_address<VBaseDealloc_rel_t>(_handle, "?VBaseDealloc_rel@@YAXPAX@Z");
}

swp::utils::memory::VBaseAlloc_rel_t swp::utils::memory::VBaseAlloc_rel{ nullptr };
swp::utils::memory::VBaseDealloc_rel_t swp::utils::memory::VBaseDealloc_rel{ nullptr };

