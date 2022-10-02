#if !defined(INC_H___52FDDE09_749B_4C31_9937_E6846805CF86__HEAD__)
#define INC_H___52FDDE09_749B_4C31_9937_E6846805CF86__HEAD__

#include <win.h>
#include <uv.h>

namespace shared {
 namespace libuv {

  const size_t DEFAULT_BUFFER_SIZE_READ = 0x10000;
  const size_t DEFAULT_BUFFER_SIZE_WRITE = 0x10000;

  class IUV {
  protected:
   IUV();
   ~IUV();
  protected:
   uv_loop_t* m_default_loop = nullptr;
   uv_connect_t m_connect = { 0 };
   uv_pipe_t m_pipe = { 0 };
   uv_tcp_t m_tcp = { 0 };
   uv_udp_t m_udp = { 0 };
  protected:
   std::atomic_bool m_IsOpen = false;
   std::string m_Address;
   std::vector<std::thread> m_Threads;
  };

  class IClient : public IUV {
  public:
   virtual bool Start(const std::string& address) = 0;
   virtual void Stop() = 0;
  };

  class IServer : public IUV {
  public:
   class Session final {
    std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
   public:
   Session();
   ~Session();
   public:
    uv_pipe_t pipe = { 0 };
    std::string pipe_name;
    uv_buf_t read_buf;
    uv_buf_t write_buf;
   public:
    void operator<<(const std::string&);
    void operator>>(const std::string&);
    void Read(const char*, const ssize_t&);
    void Write(const char*, const ssize_t&);
    std::string PopRead();
    std::string PopWrite();
    bool CloseIs() const;
    void Close();
   private:
    bool close_flag = false;
    std::string read_cache;
    std::string write_cache;
   };
  public:
   virtual bool Start(const std::string& address) = 0;
   virtual void Stop() = 0;
  };

  class PipeClient : public IClient {
  public:
   PipeClient();
   virtual ~PipeClient();
  public:
   bool Start(const std::string& address) override final;
   void Stop() override final;
  private:
   std::atomic_bool m_pipe_close_flag = false;
  };

  class PipeServer : public IServer {
  public:
   PipeServer();
   virtual ~PipeServer();
  public:
   bool Start(const std::string& address) override final;
   void Stop() override final;
  private:
   void Process();
   std::atomic_bool m_pipe_close_flag = false;
   shared::container::map<std::string, Session*> m_Sessions;
  };



 }///namespace libuv
}///namespace shared


#pragma comment(lib,"libuv")
#pragma comment(lib,"libuvimpl")
/// /*新生®（上海）**/
/// /*2022_04_08T06:57:16.0028067Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___52FDDE09_749B_4C31_9937_E6846805CF86__HEAD__