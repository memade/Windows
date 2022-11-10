#if !defined(INC_H___E59EAC68_A1CB_419A_92D7_B7031F0A1005__HEAD__)
#define INC_H___E59EAC68_A1CB_419A_92D7_B7031F0A1005__HEAD__

namespace local {

 class Core final {
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 public:
  Core();
  ~Core();
 private:
  void Init();
  void UnInit();
 public:
  bool Open();
  void Close();
 private:
  std::atomic_bool m_IsOpen = false;
  Server* m_pServer = nullptr;
  uiframework::IWxMain* m_pUIMain = nullptr;
 };

}///namespace local



/// /*新生®（上海）**/
/// /*2022_10_31T22:13:20.8537628Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___E59EAC68_A1CB_419A_92D7_B7031F0A1005__HEAD__
