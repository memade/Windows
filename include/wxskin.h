#if !defined(INC_H___1DF22BD4_2FF6_45C0_B192_ED0140F4AB4A__HEAD__)
#define INC_H___1DF22BD4_2FF6_45C0_B192_ED0140F4AB4A__HEAD__

#pragma warning(disable:4996)

#if !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE  1
#endif
#if !defined(_CRT_NON_CONFORMING_SWPRINTFS)
#define _CRT_NON_CONFORMING_SWPRINTFS  1
#endif
#if !defined(_SCL_SECURE_NO_WARNINGS)
#define _SCL_SECURE_NO_WARNINGS  1
#endif
#if !defined(__WXMSW__)
#define __WXMSW__
#endif
#if !defined(NOPCH)
#define NOPCH
#endif

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"Rpcrt4.lib")

#if defined(_DEBUG)
#pragma comment(lib,"wxbase32ud.lib")
#pragma comment(lib,"wxmsw32ud_core.lib")
#pragma comment(lib,"wxmsw32ud_aui.lib")
#pragma comment(lib,"wxmsw32ud_html.lib")
#pragma comment(lib,"wxmsw32ud_core.lib")
#pragma comment(lib,"wxbase32ud_xml.lib")
#pragma comment(lib,"wxtiffd.lib")
#pragma comment(lib,"wxjpegd.lib")
#pragma comment(lib,"wxpngd.lib")
#pragma comment(lib,"wxzlibd.lib")
#pragma comment(lib,"wxregexud.lib")
#pragma comment(lib,"wxexpatd.lib")
#pragma comment(lib,"wxmsw32ud_gl.lib")
#pragma comment(lib,"wxmsw32ud_propgrid.lib")
#else
#pragma comment(lib,"wxbase32u.lib")
#pragma comment(lib,"wxmsw32u_core.lib")
#pragma comment(lib,"wxmsw32u_aui.lib")
#pragma comment(lib,"wxmsw32u_html.lib")
#pragma comment(lib,"wxmsw32u_core.lib")
#pragma comment(lib,"wxbase32u_xml.lib")
#pragma comment(lib,"wxtiff.lib")
#pragma comment(lib,"wxjpeg.lib")
#pragma comment(lib,"wxpng.lib")
#pragma comment(lib,"wxzlib.lib")
#pragma comment(lib,"wxregexu.lib")
#pragma comment(lib,"wxexpat.lib")
#pragma comment(lib,"wxmsw32u_gl.lib")
#pragma comment(lib,"wxmsw32u_propgrid.lib")
#endif
// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/wx.h"
#include "wx/nativewin.h"
#include "wx/mdi.h"
#include "wx/aui/aui.h"
#include "wx/app.h"
#include "wx/log.h"
#include "wx/frame.h"
#include "wx/menu.h"
#include "wx/image.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/listbox.h"
#include "wx/statbox.h"
#include "wx/stattext.h"
#include "wx/textctrl.h"
#include "wx/msgdlg.h"
#include "wx/grid.h"
#include "wx/propgrid/manager.h"
#include "wx/treectrl.h"
#include "wx/spinctrl.h"
#include "wx/artprov.h"
#include "wx/clipbrd.h"
#include "wx/colordlg.h"
#include "wx/wxhtml.h"
#include "wx/imaglist.h"
#include "wx/dataobj.h"
#include "wx/dcclient.h"
#include "wx/bmpbuttn.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
#include "wx/sysopt.h"
#include "wx/bookctrl.h"
#include "wx/treebook.h"
#include "wx/sizer.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/textdlg.h"
#include "wx/wupdlock.h"
#include "wx/textcompleter.h"
#include "wx/persist/toplevel.h"
#include "wx/persist/treebook.h"
#include "wx/listctrl.h"
#include "wx/glcanvas.h"
#include "wx/rawbmp.h"
#include "wx/popupwin.h"
#include "wx/spinctrl.h"
#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/aboutdlg.h"
#include "wx/odcombo.h"
#include "wx/dynlib.h"
#include "wx/msw/wrapwin.h"
#include "wx/dcbuffer.h"
#include "wx/mstream.h"

#ifndef RGB
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

namespace shared {
 namespace wx {
  using IdentifyTheme = std::string;

  typedef class IwxMDIParentFrame : public wxMDIParentFrame {
  public:
   IwxMDIParentFrame(wxWindow* parent = nullptr,
    const wxWindowID& id = wxID_ANY,
    const wxString& title = L"",
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxSize(1024, 768),
    long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
   virtual ~IwxMDIParentFrame();
  protected:
   void OnSize(wxSizeEvent&);
   void OnClose(wxCloseEvent&);
  private:
   wxDECLARE_EVENT_TABLE();
  }IMDIParentFrame;

  class IwxFrame : public wxFrame {
  public:
   IwxFrame();
   IwxFrame(wxWindow* parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE,
    const wxString& name = wxASCII_STR(wxFrameNameStr));
   virtual ~IwxFrame();
  protected:
   virtual void OnErase(wxEraseEvent&);
   virtual void OnPaint(wxPaintEvent&);
   virtual void OnSize(wxSizeEvent&);
   virtual void OnClose(wxCloseEvent&);
   virtual bool SetShape(const wxImage&);
  public:
   virtual void ReSetShape();
   virtual void SetBackgroundImage(const wxString&);
  private:
   DECLARE_DYNAMIC_CLASS(IwxFrame)
    wxDECLARE_EVENT_TABLE();
  protected:
   wxImage m_ImgShapeBackgroundShape;
   wxImage m_ImgShapeBackground;
  };

  class Theme final {
  public:
   Theme(const IdentifyTheme& identify, const std::string& shape_img_pathname, const std::string& bgk_img_pathname);
   ~Theme();
  public:
   const wxImage& ImgShape() const;
   const wxImage& ImgBgk() const;
   const IdentifyTheme& Identify() const;
   const wxSize& MainWindowSize() const;
   const DWORD& ColorBgk() const;
   bool Ready() const;
   static bool From(const std::string& imgPathname, wxMemoryInputStream**, wxImage**);
  private:
   wxMemoryInputStream* m_pMImgShape = nullptr;
   wxMemoryInputStream* m_pMImgBgk = nullptr;
   const IdentifyTheme m_Identify;
   wxImage* m_pImgShape = nullptr;
   wxImage* m_pImgBgk = nullptr;
   std::atomic_bool m_Ready = false;
   std::map<std::string, wxFont> m_Fonts;
   wxSize m_MainWindowSize = { 0,0 };
   DWORD m_BgkColor = RGB(255, 0, 0);
  };

  class IwxFrameSkin : public wxFrame {
  public:
   IwxFrameSkin(wxWindow* parent,
    wxWindowID id = wxID_ANY,
    const wxString& title = _TEXT(""),
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = wxNO_BORDER | wxFRAME_SHAPED,
    const wxString& name = wxASCII_STR(wxFrameNameStr));
   virtual ~IwxFrameSkin();
  public:
   bool AppendTheme(Theme*);
   bool SetTheme(const IdentifyTheme& current = "");
  protected:
   void OnErase(wxEraseEvent&);
   void OnPaint(wxPaintEvent&);
   void OnSize(wxSizeEvent&);
   void OnClose(wxCloseEvent&);
   void OnMotion(wxMouseEvent&);
   void OnLeftDown(wxMouseEvent&);
   void OnLeftUp(wxMouseEvent&);
   void OnRightDown(wxMouseEvent&);
   void OnRightUp(wxMouseEvent&);
   void OnMouseLost(wxMouseCaptureLostEvent&);
   void OnEnterWindow(wxMouseEvent&);
   void OnLeaveWindow(wxMouseEvent&);
   void OnContextMenu(wxContextMenuEvent&);
  protected:
   bool SetShape(const wxImage& img);
  protected:
   wxMenu m_MouseRightPopupMenu;
   wxPoint m_offset{ -1,-1 };
   wxRect m_rect{ 0,0,0,0 };
   std::map<IdentifyTheme, Theme*> m_Themes;
   IdentifyTheme m_CurrentTheme;
  private:
   DECLARE_EVENT_TABLE()
  };

  using tfAppInitEventCallback = std::function<void(const bool&)>;
  using tfAppCloseEventNotifyCallback = std::function<void(void)>;
  using tfAppCreateFrameEventCallback = std::function<void(wxFrame*)>;
  class IwxApp : public wxApp {
  public:
   IwxApp();
   virtual ~IwxApp();
  private:
   void OnCreateFrame(wxThreadEvent& event);
   void OnDestory(wxThreadEvent& event);
  public:
   bool OnInit() override;
   int OnExit() override;
   wxFrame* FrameGet() const;
   void RegisterAppInitEventCb(const tfAppInitEventCallback&);
   void RegisterAppCloseEventNotifyCb(const tfAppCloseEventNotifyCallback&);
   void RegisterAppCreateFrameEventCb(const tfAppCreateFrameEventCallback&);
  protected:
   std::vector<std::thread> m_Threads;
   wxFrame* m_pFrame = nullptr;
   tfAppInitEventCallback m_AppInitEventCb = nullptr;
   tfAppCloseEventNotifyCallback m_AppCloseEventNotifyCb = nullptr;
   tfAppCreateFrameEventCallback m_AppCreateFrameEventCb = nullptr;
  };

  class Misc final {
  public:
  };
  extern const int WX_CMD_ONAPPCREATEFRAME;
  extern const int WX_CMD_ONAPPDESTORY;
 }///namespace wx
}///namespace shared




#pragma comment(lib,"wxskin.lib")
/// /*新生®（上海）**/
/// /*2022_08_17T12:26:46.8845292Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___1DF22BD4_2FF6_45C0_B192_ED0140F4AB4A__HEAD__

