#if !defined(INC_H___909F111D_2A54_4E1E_9551_145FD497C0B8__HEAD__)
#define INC_H___909F111D_2A54_4E1E_9551_145FD497C0B8__HEAD__

namespace shared {
 namespace hijack {
  //!@ [https://github.com/anhkgg/SuperDllHijack]
  class anhkgg final {
  public:
   anhkgg() {}
   ~anhkgg() = delete;
  public:
   static bool SuperDllHijack(const std::wstring&, const HMODULE&);
   static bool SuperDllHijack(const std::wstring&, const std::wstring&);
  };

  //!@ [https://github.com/monoxgas/Koppeling/blob/master/Theif]
  class monoxgas final {
  public:
   monoxgas() {}
   ~monoxgas() = delete;
  public:
   static BOOL RebuildExportTable(PBYTE ourBase, PBYTE targetBase);
   static PBYTE AllocateUsableMemory(PBYTE baseAddress, DWORD size, DWORD protection = PAGE_READWRITE);
  };



 }///namespace hijack
}///namespace shared

#pragma comment(lib,"hijack")
/// /*新生®（上海）**/
/// /*2022_02_19T05:51:07.9292267Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___909F111D_2A54_4E1E_9551_145FD497C0B8__HEAD__