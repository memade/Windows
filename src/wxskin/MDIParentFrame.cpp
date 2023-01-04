#include "stdafx.h"

namespace shared {
 namespace wx {

  wxBEGIN_EVENT_TABLE(IwxMDIParentFrame, wxMDIParentFrame)
   EVT_SIZE(IwxMDIParentFrame::OnSize)
   EVT_CLOSE(IwxMDIParentFrame::OnClose)
   wxEND_EVENT_TABLE();

  IwxMDIParentFrame::IwxMDIParentFrame(
   wxWindow* parent,
   const wxWindowID& id,
   const wxString& title,
   const wxPoint& pos,
   const wxSize& size,
   long style) :
   wxMDIParentFrame(parent, id, title, pos, size, style) {
  }

  IwxMDIParentFrame::~IwxMDIParentFrame() {

  }

  void IwxMDIParentFrame::OnSize(wxSizeEvent& wxEvent) {
   wxEvent.Skip();
  }
  void IwxMDIParentFrame::OnClose(wxCloseEvent& wxEvent) {
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
#if 0
  IwxMDIParentFrame::IwxMDIParentFrame(const std::wstring& xml,
   wxWindow* parent,
   const wxWindowID& id,
   const wxString& title,
   const wxPoint& pos,
   const wxSize& size,
   long style) :
   wxMDIParentFrame(parent, id, title, pos, size, style) {
   m_SkinConfig = xml;
   Parse();
  }

  IwxMDIParentFrame::IwxMDIParentFrame(
   wxWindow* parent,
   const wxWindowID& id,
   const wxString& title,
   const wxPoint& pos,
   const wxSize& size,
   long style) :
   wxMDIParentFrame(parent, id, title, pos, size, style) {
   Parse();
  }

  IwxMDIParentFrame::~IwxMDIParentFrame() {

  }

  bool IwxMDIParentFrame::Parse() {
   bool result = false;
   shared::tf_rapidxml_wchar_doc* pDoc = nullptr;
   do {
    if (m_SkinConfig.empty())
     break;
    std::wstring xml_buffer{ m_SkinConfig };
    pDoc = shared::rapidxml_parse(xml_buffer);
    if (!pDoc)
     break;
    /*<wxMDIParentFrame show="true" center="true"><wxMDIChildFrame/></wxMDIParentFrame>*/
    auto root = pDoc->first_node();
    if (!root)
     break;
    if (::_wcsicmp(L"wxMDIParentFrame", root->name()))
     break;

    bool bShow = false;
    bool bCenter = false;
    wxString strTitle;
    wxPoint pos;
    wxSize size;

    auto first_attribute = root->first_attribute();
    while (first_attribute) {
     if (!::_wcsicmp(L"show", first_attribute->name())) {
      if (!::_wcsicmp(L"true", first_attribute->value()))
       bShow = true;
     }
     else if (!::_wcsicmp(L"center", first_attribute->name())) {
      if (!::_wcsicmp(L"true", first_attribute->value()))
       bCenter = true;
     }
     else if (!::_wcsicmp(L"title", first_attribute->name())) {
      strTitle = first_attribute->value();
     }
     else if (!::_wcsicmp(L"size", first_attribute->name())) {
      wxPointEx pt(first_attribute->value());
      size = wxSize(pt.x, pt.y);
     }
     else if (!::_wcsicmp(L"pos", first_attribute->name())) {
      pos = wxPointEx(first_attribute->value()).toPoint();
     }

     first_attribute = first_attribute->next_attribute();
    }

    if (!strTitle.empty())
     SetTitle(strTitle);
    if (size.x || size.y)
     SetSize(size);
    if (pos.x > 0 || pos.y > 0 && !bCenter)
     SetPosition(pos);
    if (bCenter)
     Center(bCenter);

    Show(bShow);

    auto first_node = root->first_node();
    while (first_node) {
     auto name = first_node->name();
     first_node = first_node->next_sibling();
    }

    result = true;
   } while (0);
   SK_DELETE_PTR(pDoc);
   return result;
  }
#endif




 }///namespace wx
}///namespace shared