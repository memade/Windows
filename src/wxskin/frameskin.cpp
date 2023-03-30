#include "stdafx.h"

namespace shared {
 namespace wx {

  /*IMPLEMENT_DYNAMIC_CLASS(IwxFrameSkin, wxFrame)*/

  wxBEGIN_EVENT_TABLE(IwxFrameSkin, wxFrame)
   EVT_SIZE(IwxFrameSkin::OnSize)
   EVT_PAINT(IwxFrameSkin::OnPaint)
   EVT_CLOSE(IwxFrameSkin::OnClose)
   EVT_LEFT_DOWN(IwxFrameSkin::OnLeftDown)
   EVT_LEFT_UP(IwxFrameSkin::OnLeftUp)
   EVT_RIGHT_DOWN(IwxFrameSkin::OnRightDown)
   EVT_RIGHT_UP(IwxFrameSkin::OnRightUp)
   EVT_MOUSE_CAPTURE_LOST(IwxFrameSkin::OnMouseLost)
   EVT_MOTION(IwxFrameSkin::OnMotion)
   EVT_ERASE_BACKGROUND(IwxFrameSkin::OnErase)
   EVT_ENTER_WINDOW(IwxFrameSkin::OnEnterWindow)
   EVT_LEAVE_WINDOW(IwxFrameSkin::OnLeaveWindow)
   /*EVT_CONTEXT_MENU(IwxFrameSkin::OnContextMenu)*/
   wxEND_EVENT_TABLE();

  IwxFrameSkin::IwxFrameSkin(wxWindow* parent,
   wxWindowID id,
   const wxString& title /*= _TEXT("")*/,
   const wxPoint& pos /*= wxDefaultPosition*/,
   const wxSize& size /*= wxDefaultSize*/,
   long style /*= wxNO_BORDER | wxFRAME_SHAPED*/,
   const wxString& name /*= wxASCII_STR(wxFrameNameStr)*/)
   : wxFrame(parent, id, title, pos, size, style, name) {

   m_MouseRightPopupMenu.Append(0, "&About");
  //m_MouseRightPopupMenu.Append(1, "&Submenu", CreateDummyMenu(NULL));
   m_MouseRightPopupMenu.Append(2, "To be &deleted");
   m_MouseRightPopupMenu.AppendCheckItem(3, "To be &checked");
   m_MouseRightPopupMenu.Append(4, "To be &greyed",
    "This menu item should be initially greyed out");
   m_MouseRightPopupMenu.AppendSeparator();
   m_MouseRightPopupMenu.Append(5, "E&xit")
    ->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_QUIT, wxART_MENU));

   //m_MouseRightPopupMenu.Delete(Menu_Popup_ToBeDeleted);
   //m_MouseRightPopupMenu.Check(Menu_Popup_ToBeChecked, true);
   //m_MouseRightPopupMenu.Enable(Menu_Popup_ToBeGreyed, false);

   //PopupMenu(&m_MouseRightPopupMenu, pos);
  }
  IwxFrameSkin::~IwxFrameSkin() {
  }
  void IwxFrameSkin::OnSize(wxSizeEvent& wxEvent) {
   wxEvent.Skip();
  }
  void IwxFrameSkin::OnClose(wxCloseEvent& wxEvent) {
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
  void IwxFrameSkin::OnErase(wxEraseEvent& wxEvent) {
   wxEvent.Skip();
  }
  void IwxFrameSkin::OnPaint(wxPaintEvent& wxEvent) {
   do {
    int width{ 0 }, height{ 0 };
    GetSize(&width, &height);
    if (width <= 0 || height <= 0)
     break;
    wxBufferedPaintDC BufferDC(this);
    BufferDC.SetBrush(*wxTRANSPARENT_BRUSH);
    BufferDC.Clear();

    SetBackgroundColour(wxColour(255, 0, 0, wxALPHA_OPAQUE));
#if 0
    BufferDC.SetBrush(*wxTRANSPARENT_BRUSH);
    BufferDC.Clear();
    auto current_theme = m_Themes.find(m_CurrentTheme);
    if (current_theme == m_Themes.end())
     break;

    //BufferDC.DrawBitmap(wxBitmap(current_theme->second->ImgBgk().Scale(width, height)), 0, 0, true);
#endif
   } while (0);

  }


  bool IwxFrameSkin::AppendTheme(Theme* theme) {
   bool result = false;
   do {
    if (!theme->Ready())
     break;
    auto found = m_Themes.find(theme->Identify());
    if (found != m_Themes.end())
     m_Themes.erase(found);
    m_Themes.emplace(theme->Identify(), theme);
    result = true;
   } while (0);
   return result;
  }
  bool IwxFrameSkin::SetTheme(const IdentifyTheme& current /*= ""*/) {
   bool result = false;
   do {
    Theme* pos = nullptr;
    if (m_Themes.empty())
     break;
    if (!current.empty()) {
     auto found = m_Themes.find(current);
     if (found == m_Themes.end())
      break;
     pos = found->second;
    }
    else
     pos = m_Themes.begin()->second;
    m_CurrentTheme = pos->Identify();
    auto size = pos->MainWindowSize();
    SetSize(size);
    if (!SetShape(pos->ImgShape().Scale(size.x, size.y)))
     break;

    SetBackgroundColour(wxColour(255, 0, 0, wxALPHA_OPAQUE));
    Layout();
    Refresh();
    result = true;
   } while (0);
   return result;
  }
  bool IwxFrameSkin::SetShape(const wxImage& img) {
   wxImage m = img;
   if (m.HasAlpha())
    m.ConvertAlphaToMask();
   return wxNonOwnedWindow::SetShape(wxRegion(m));
  }

  void IwxFrameSkin::OnMotion(wxMouseEvent& wxEvent) {
   wxPoint pt = wxEvent.GetPosition();
   bool isDown = wxEvent.LeftIsDown();
   if (isDown && wxEvent.Dragging() && HasCapture()) {
    wxPoint mouse_pos = ClientToScreen(pt);
    if (m_offset.x != -1) {
     wxPoint dp = mouse_pos - m_offset;
     this->Move(dp);
    }
   }
  }
  void IwxFrameSkin::OnLeftDown(wxMouseEvent& wxEvent) {
   wxPoint pt = wxEvent.GetPosition();
   if (!HasCapture() && !m_rect.Contains(pt))
    CaptureMouse();
   wxPoint mouse_pos = ClientToScreen(pt);
   wxPoint wnd_pos = this->GetPosition();
   m_offset.x = mouse_pos.x - wnd_pos.x;
   m_offset.y = mouse_pos.y - wnd_pos.y;
  }
  void IwxFrameSkin::OnLeftUp(wxMouseEvent& wxEvent) {
   m_offset = wxPoint(-1, -1);
   if (HasCapture())
    ReleaseMouse();
  }
  void IwxFrameSkin::OnContextMenu(wxContextMenuEvent&) {
   std::cout << __FUNCTION__ << std::endl;
  }
  void IwxFrameSkin::OnRightDown(wxMouseEvent& wxEvent) {
   auto sk = 0;
  }
  void IwxFrameSkin::OnRightUp(wxMouseEvent& wxEvent) {
   auto sk = 0;
   std::cout << __FUNCTION__ << std::endl;

   wxPoint point = wxEvent.GetPosition();
   //// If from keyboard
   //if (point.x == -1 && point.y == -1) {
   // wxSize size = GetSize();
   // point.x = size.x / 2;
   // point.y = size.y / 2;
   //}
   //else {
   // point = ScreenToClient(point);
   //}

   PopupMenu(&m_MouseRightPopupMenu, point);
  }
  void IwxFrameSkin::OnMouseLost(wxMouseCaptureLostEvent& wxEvent) {
   m_offset = wxPoint(-1, -1);
   if (HasCapture())
    ReleaseMouse();
  }
  void IwxFrameSkin::OnEnterWindow(wxMouseEvent& wxEvent) {
  /* wxEvent.Skip();*/
   CaptureMouse();
   std::cout << __FUNCTION__ << std::endl;
  }
  void IwxFrameSkin::OnLeaveWindow(wxMouseEvent& wxEvent) {
   //wxEvent.Skip();

   //Refresh();
   // 
#if 0
   //鼠标拖动事件，当鼠标离开窗口时，发送信号告诉窗口
   //这个是为了解决拖动速度过快导致窗口没有跟踪上鼠标
   bool isDown = wxEvent.LeftIsDown();
   bool isDragging = wxEvent.Dragging();
   if (isDown && isDragging)
   {
    wxCommandEvent ev(wxEVT_MOTION, GetId());
    ev.SetEventObject(this);
    GetEventHandler()->ProcessEvent(ev);
   }
#endif

   ReleaseMouse();
   std::cout << __FUNCTION__ << std::endl;
   auto sk = 0;
  }
 }///namespace wx
}///namespace shared