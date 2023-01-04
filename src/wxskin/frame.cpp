#include "stdafx.h"

namespace shared {
 namespace wx {

  IMPLEMENT_DYNAMIC_CLASS(IwxFrame, wxFrame)

  wxBEGIN_EVENT_TABLE(IwxFrame, wxFrame)
   EVT_SIZE(IwxFrame::OnSize)
   EVT_PAINT(IwxFrame::OnPaint)
   EVT_CLOSE(IwxFrame::OnClose)
   EVT_ERASE_BACKGROUND(IwxFrame::OnErase)
   wxEND_EVENT_TABLE();

  IwxFrame::IwxFrame() {
   Init();
  }
  IwxFrame::IwxFrame(wxWindow* parent,
   wxWindowID id,
   const wxString& title,
   const wxPoint& pos /*= wxDefaultPosition*/,
   const wxSize& size /*= wxDefaultSize*/,
   long style /*= wxDEFAULT_FRAME_STYLE*/,
   const wxString& name /*= wxASCII_STR(wxFrameNameStr)*/) {

  }

  IwxFrame::~IwxFrame() {

  }

  void IwxFrame::OnSize(wxSizeEvent& wxEvent) {
   ReSetShape();
   wxEvent.Skip();
  }
  void IwxFrame::OnClose(wxCloseEvent& wxEvent) {
   int res = wxMessageBox(
    "Are you sure you want to exit system?",
    "tip",
    wxYES_NO,
    this);
   if (res != wxYES) {
    wxEvent.Veto();
   }
   else {
    auto handle = wxApp::GetInstance();
    if (handle)
     wxQueueEvent(handle, new wxThreadEvent(wxEVT_THREAD, WX_CMD_ONAPPDESTORY));
    wxEvent.Skip();


   }
  }
  void IwxFrame::OnErase(wxEraseEvent& wxEvent) {
   wxEvent.Skip();
  }
  void IwxFrame::OnPaint(wxPaintEvent& wxEvent) {
   do {
    int width{ 0 }, height{ 0 };
    GetSize(&width, &height);
    if (width <= 0 || height <= 0)
     break;
    wxBufferedPaintDC BufferDC(this);
    //清除背景
    BufferDC.SetBrush(*wxTRANSPARENT_BRUSH);
    BufferDC.Clear();

    if (m_ImgShapeBackground.IsOk()) {
     BufferDC.DrawBitmap(wxBitmap(m_ImgShapeBackground.Scale(width, height)), 0, 0, true);
    }
#if 0
    //在窗口上绘制图片，默认都是按照窗体大小进行等比例绘制
    if (m_bInside && image_over.IsOk())
    {
     //当鼠标在窗口里面的时候
     dcp.DrawBitmap(wxBitmap(image_over.Scale(w, h)), 0, 0, true);
    }
    else if (IsEnabled() && image_normal.IsOk())
    {
     //当窗口不属于禁用状态并且有正常的皮肤图片时
     dcp.DrawBitmap(wxBitmap(image_normal.Scale(w, h)), 0, 0, true);
    }
    else if (!IsEnabled() && image_disable.IsOk())
    {
     //当窗口处于禁用状态时
     dcp.DrawBitmap(wxBitmap(image_disable.Scale(w, h)), 0, 0, true);
    }
    else if (image_normal.IsOk())
    {
     //当窗口不属于上面几种条件时，默认绘制正常的皮肤图片
     dcp.DrawBitmap(wxBitmap(image_normal.Scale(w, h)), 0, 0, true);
    }
#endif

   } while (0);

  }
  void IwxFrame::ReSetShape() {
   if (m_ImgShapeBackgroundShape.IsOk()) {
    int w, h;
    GetSize(&w, &h);
    if (w != 0 && h != 0) {
     SetShape(m_ImgShapeBackgroundShape.Scale(w, h));
    }
   }
   Layout();
   Refresh();
  }
  bool IwxFrame::SetShape(const wxImage& img) {
   wxImage m = img;
   if (m.HasAlpha()) {
    m.ConvertAlphaToMask();
   }
   return wxNonOwnedWindow::SetShape(wxRegion(m));
  }

  void IwxFrame::SetBackgroundImage(const wxString& bkImgPathname) {
   m_ImgShapeBackgroundShape = wxImage(bkImgPathname);
   if (m_ImgShapeBackgroundShape.IsOk()) {

    auto sss = 0;
   }
  }



 }///namespace wx
}///namespace shared