// local
#include "../headers/hooks.hpp"
#include "../headers/hook.hpp"
#include "../headers/file_access.hpp"
#include "../headers/utils/memory.hpp"

// deps
#include <spdlog/spdlog.h>

int32_t __fastcall VAssert(const char* szFile, int iLine, const char* pszText, const char* pszMsg)
{
  spdlog::info("file: {} | line: {} | text: {} | message: {}", szFile, iLine, pszText, pszMsg);
  return IsDebuggerPresent();
}

using VAssertSetHandler_t = auto(*)(decltype(VAssert)* pfHandler) ->int32_t;
VAssertSetHandler_t VAssertSetHandler;

inline static auto swp::hooks::base_dll(void) -> void
{
  const std::initializer_list<std::pair<std::string_view, LPVOID>> values{
    { "?GetSize@VFileAccess@@SAJPAUVFileHandle@@@Z", file_access::get_size },
    { "?Close@VFileAccess@@SAXPAUVFileHandle@@@Z", file_access::close },
    { "?IsEOF@VFileAccess@@SA_NPAUVFileHandle@@@Z", file_access::is_eof },
    { "?Flush@VFileAccess@@SAXPAUVFileHandle@@@Z", file_access::flush },
    // { "?GetErrorHandlerCallback@VFileAccess@@SAP6A_NW4FileErrorState@1@@ZXZ::file_access:: },
    // { "?SetErrorHandlerCallback@VFileAccess@@SAP6A_NW4FileErrorState@1@@ZP6A_N0@Z@Z::file_access:: },
    // { "?HandleError@VFileAccess@@SA_NW4FileErrorState@1@@Z::file_access:: },
    { "?Write@VFileAccess@@SAJPAUVFileHandle@@PBXI@Z", file_access::write },
    { "?Open@VFileAccess@@SA_NPAUVFileHandle@@PBDII@Z", file_access::open },
    { "?Read@VFileAccess@@SAJPAUVFileHandle@@PAXI@Z", file_access::read },
    { "?GetPos@VFileAccess@@SAJPAUVFileHandle@@@Z", file_access::get_pos },
    { "?SetPos@VFileAccess@@SA_NPAUVFileHandle@@JW4SeekPosition@1@@Z", file_access::set_pos }
  };

  const auto handle{ GetModuleHandle(TEXT("Base.dll")) };
  if (not handle) {
    spdlog::error("Base.dll module not found");
    return;
  }

  VAssertSetHandler = reinterpret_cast<decltype(VAssertSetHandler)>(GetProcAddress(handle, "?VAssertSetHandler@@YAP6AHPBDH00@ZP6AH0H00@Z@Z"));
  VAssertSetHandler(VAssert);

  utils::memory::setup(handle);
  file_access::setup(handle);

  for (const auto& [name, func] : values) {
    const auto address{ GetProcAddress(handle, name.data()) };
    if (not address) {
      spdlog::error("Base.dll export not found");
      return;
    }

    spdlog::debug("Install hook [{}]", name);

    hook::install(address, func);
  }
}