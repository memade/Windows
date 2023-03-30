#include "stdafx.h"

namespace shared {
 namespace wx {
  Theme::Theme(const IdentifyTheme& identify, const std::string& shape_img_pathname, const std::string& bgk_img_pathname)
   : m_Identify(identify) {
   do {
    if (m_Identify.empty())
     break;
    if (!From(bgk_img_pathname, &m_pMImgBgk, &m_pImgBgk))
     break;
    if (!From(shape_img_pathname, &m_pMImgShape, &m_pImgShape))
     break;
    m_MainWindowSize = m_pImgShape->GetSize();
    m_Ready.store(true);
   } while (0);
  }
  Theme::~Theme() {
   SK_DELETE_PTR(m_pImgBgk);
   SK_DELETE_PTR(m_pImgShape);
   SK_DELETE_PTR(m_pMImgBgk);
   SK_DELETE_PTR(m_pMImgShape);
  }
  const DWORD& Theme::ColorBgk() const {
   return m_BgkColor;
  }
  const wxSize& Theme::MainWindowSize() const {
   return m_MainWindowSize;
  }
  const wxImage& Theme::ImgShape() const {
   return *m_pImgShape;
  }
  const wxImage& Theme::ImgBgk() const {
   return *m_pImgBgk;
  }
  const IdentifyTheme& Theme::Identify() const {
   return m_Identify;
  }
  bool Theme::Ready() const {
   return m_Ready.load();
  }
  bool Theme::From(const std::string& imgPathname, wxMemoryInputStream** ppMInputStream, wxImage** ppImage) {
   bool result = false;
   *ppImage = nullptr;
   *ppMInputStream = nullptr;
   do {
    do {
     auto inputBuffer = shared::Win::File::Read(imgPathname);
     if (inputBuffer.empty())
      break;
     *ppMInputStream = new wxMemoryInputStream(inputBuffer.data(), inputBuffer.size());
     result = true;
     shared::ImageType imgType = shared::Win::GetImageTypeByMemory(inputBuffer);
     switch (imgType) {
     case shared::ImageType::_PNG: {
      *ppImage = new wxImage(**ppMInputStream, wxBITMAP_TYPE_PNG);
      break;
     }break;
     case shared::ImageType::_BMP: {
      *ppImage = new wxImage(**ppMInputStream, wxBITMAP_TYPE_BMP);
     }break;
     case shared::ImageType::_GIF: {
      *ppImage = new wxImage(**ppMInputStream, wxBITMAP_TYPE_GIF);
     }break;
     case shared::ImageType::_ICO: {
      *ppImage = new wxImage(**ppMInputStream, wxBITMAP_TYPE_ICO);
     }break;
     case shared::ImageType::_JPEG: {
      *ppImage = new wxImage(**ppMInputStream, wxBITMAP_TYPE_JPEG);
     }break;
     default: {
      result = false;
     }break;
     }
    } while (0);
    if (result) {
     result = (*ppImage)->IsOk();
    }
    if (!result) {
     SK_DELETE_PTR(*ppImage);
     SK_DELETE_PTR(*ppMInputStream);
    }
   } while (0);
   return result;
  }



  extern const int WX_CMD_ONAPPCREATEFRAME = wxNewId();
  extern const int WX_CMD_ONAPPDESTORY = wxNewId();
 }///namespace wx
}///namespace shared