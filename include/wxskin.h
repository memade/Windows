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

#ifndef RGB
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

namespace shared {
 namespace wx {
  enum class SkinDataType { XML = 0, JSON = 1, BEGIN = XML, END = JSON, };
  enum class EncodingType { UTF8 = 0, UTF16 = 1, ASNI = 2, BEGIN = UTF8, END = ASNI, };

  class wxPointEx : public wxPoint {
  public:
   wxPointEx(const std::wstring& point_string/*{"x","y"}*/);
   void operator=(const std::wstring& point_string);
   wxPoint toPoint() const;
  };
  class wxPositionEx : public wxPosition {
  public:
   wxPositionEx(const std::wstring& pos_string/*{"x","y"}*/);
  };
  typedef class wxRectEx : public wxRect {
  public:
   wxRectEx(const std::wstring& string);
  }wxRange;

  class ISkin {
  public:
   void operator=(const ISkin&);
   bool operator<<(const std::wstring&);
  public:
   RECT pos;
   SIZE size;
   POINT range;
   std::wstring title;
   std::wstring class_name;
   std::wstring name;
   bool show;
   bool center;
   std::wstring bkimage_pathname;
   std::string bkimage_buffer;
   unsigned long bkcolor;
   unsigned long bkcolor1;
   unsigned long bkcolor2;
  protected:
   ISkin();
   virtual bool Parse() = 0;
  protected:
   std::string m_SkinBuffer;
   std::wstring m_SkinConfig;
  };

  typedef class IwxMDIParentFrame : public wxMDIParentFrame, public ISkin {
  public:
   IwxMDIParentFrame(
    const std::wstring& xml,
    wxWindow* parent = nullptr,
    const wxWindowID& id = wxID_ANY,
    const wxString& title = L"",
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxSize(1024, 768),
    long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
   IwxMDIParentFrame(wxWindow* parent = nullptr,
    const wxWindowID& id = wxID_ANY,
    const wxString& title = L"",
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxSize(1024, 768),
    long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
   virtual ~IwxMDIParentFrame();
  protected:
   bool Parse() override final;
  }IMDIParentFrame;

  // @Config class is ui configure xm or json
  // @Support for multiple encoding
  class IwxApp : public wxApp, public ISkin {
  public:
   static IwxApp* Create(const std::string& buffer, const SkinDataType& dataType, const EncodingType& encodingType);
   bool Ready() const;
   void Destory();
  protected:
   IwxApp(const std::string& config_buffer, const SkinDataType& dataType = SkinDataType::XML, const EncodingType& encodingType = EncodingType::UTF8);
   IwxApp();
   virtual ~IwxApp();
  private:
   bool Parse();
   bool ParseXML();
   bool ParseJSON();
  private:
   IMDIParentFrame* m_pFrame;
   SkinDataType m_DataType;
   EncodingType m_EncodingType;
  private:
   std::atomic_bool m_Ready;
   std::vector<std::thread> m_Threads;
  protected:
   bool OnInit() override;
   int OnExit() override;
  };


 }///namespace wx
}///namespace shared




#pragma comment(lib,"wxskin.lib")
/// /*新生®（上海）**/
/// /*2022_08_17T12:26:46.8845292Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___1DF22BD4_2FF6_45C0_B192_ED0140F4AB4A__HEAD__

