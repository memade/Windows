#if !defined(INC_H___8B3B64A9_243B_4079_8B7B_7C0140D90803__HEAD__)
#define INC_H___8B3B64A9_243B_4079_8B7B_7C0140D90803__HEAD__

namespace local {

 class Server final {
 public:
  Server();
  ~Server();
 private:
  void Init();
  void UnInit();
 public:
  bool Start();
  void Stop();
 private:
  pchacker::libuvpp::IServer* m_pServer = nullptr;
  std::atomic_bool m_IsOpen = false;
 };

}///namespace local



/// /*新生®（上海）**/
/// /*2022_10_31T22:13:30.7498919Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___8B3B64A9_243B_4079_8B7B_7C0140D90803__HEAD__