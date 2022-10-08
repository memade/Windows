#include "stdafx.h"

namespace shared {
 namespace wx {

  ISkin::ISkin() {
   ::memset(&pos, 0x00, sizeof(pos));
   ::memset(&size, 0x00, sizeof(size));
   ::memset(&range, 0x00, sizeof(range));

   title = L"";
   class_name = L"";
   name = L"";
   show = false;
   center = false;
   bkimage_pathname = L"";
   bkimage_buffer = "";
   bkcolor = RGB(255, 0, 0);
   bkcolor1 = RGB(255, 0, 0);
   bkcolor2 = RGB(255, 0, 0);
  }

  void ISkin::operator=(const ISkin& obj) {
   ::memcpy(&pos, &obj.pos, sizeof(pos));
   ::memcpy(&size, &obj.size, sizeof(size));
   ::memcpy(&range, &obj.range, sizeof(range));

   title = obj.title;
   class_name = obj.class_name;
   name = obj.name;
   show = obj.show;
   center = obj.center;
   bkimage_buffer = obj.bkimage_buffer;
   bkimage_pathname = obj.bkimage_pathname;
   bkcolor = obj.bkcolor;
   bkcolor1 = obj.bkcolor1;
   bkcolor2 = obj.bkcolor2;

  }

  IwxApp::IwxApp() {
   m_Ready.store(true);
  }
  IwxApp::IwxApp(
   const std::string& config_buffer /*= ""*/,
   const SkinDataType& dataType /*= DataType::XML*/,
   const EncodingType& encodingType /*= EncodingType::UTF8*/) :
   m_pFrame(nullptr),
   m_Ready(false),
   m_DataType(dataType),
   m_EncodingType(encodingType) {
   m_SkinBuffer = config_buffer;


  }
  IwxApp::~IwxApp() {
   if (!m_Ready.load())
    return;
   for (auto& t : m_Threads)
    t.join();
   m_Threads.clear();
  }

  bool IwxApp::OnInit() {
   do {
    if (m_Ready.load())
     break;
    if (!wxApp::OnInit())
     break;

    m_Ready.store(Parse());
   } while (0);
   return m_Ready.load();
  }
  int IwxApp::OnExit() {
   int result = 0;
   do {
    if (!m_Ready.load())
     break;


    result = wxApp::OnExit();
   } while (0);
   return result;
  }
  bool IwxApp::Ready() const {
   return m_Ready.load();
  }
  void IwxApp::Destory() {

  }
  bool IwxApp::ParseXML() {
   bool result = false;
   do {
    if (m_SkinBuffer.empty())
     break;
    std::wstring parse_cache;
    switch (m_EncodingType) {
    case EncodingType::UTF8: {
     parse_cache = shared::IConv::UTF8ToWString(m_SkinBuffer);
    }break;
    case EncodingType::UTF16: {
     parse_cache = std::wstring((wchar_t*)&m_SkinBuffer[0], m_SkinBuffer.size());
    }break;
    case EncodingType::ASNI: {
     parse_cache = shared::IConv::MBytesToWString(m_SkinBuffer);
    }break;
    default: {
    }break;
    }
    if (parse_cache.empty())
     break;
    auto pDoc = std::make_shared<rapidxml::xml_document<wchar_t>>();
    try {
     pDoc->parse<0>((wchar_t*)parse_cache.c_str());
    }
    catch (rapidxml::parse_error& err) {
     auto what = err.what();
     break;
    }
    auto first_node = pDoc->first_node();
    if (::_wcsicmp(L"wxApp", first_node->name()))
     break;
    auto wxApp = first_node;
    while (wxApp) {
     auto first_attribute_wxApp = wxApp->first_attribute();
     while (first_attribute_wxApp) {
      if (!::_wcsicmp(L"ClassName", first_attribute_wxApp->name())) {
       m_className = first_attribute_wxApp->value();
      }
      first_attribute_wxApp = first_attribute_wxApp->next_attribute();
     }
     auto first_node_wxApp = wxApp->first_node();
     while (first_node_wxApp) {
      if (!::_wcsicmp(L"wxMDIParentFrame", first_node_wxApp->name())) {
       std::wstring temp;
       rapidxml::print(std::back_inserter(temp), *first_node_wxApp, rapidxml::print_no_indenting);
       m_pFrame = new IwxMDIParentFrame(temp);

      }
      first_node_wxApp = first_node_wxApp->next_sibling();
     }
     wxApp = wxApp->next_sibling();
    }

    result = true;
   } while (0);
   return result;
  }
  bool IwxApp::ParseJSON() {
   bool result = false;
   do {
    if (m_SkinBuffer.empty())
     break;
    switch (m_EncodingType) {
    case EncodingType::UTF8: {

    }break;
    case EncodingType::UTF16: {
    }break;
    case EncodingType::ASNI: {
    }break;
    default:
     break;
    }
   } while (0);
   return result;
  }
  bool IwxApp::Parse() {
   m_Ready = false;
   do {
    if (m_SkinBuffer.empty())
     break;

    switch (m_DataType) {
    case SkinDataType::XML: {
     m_Ready = ParseXML();
    }break;
    case SkinDataType::JSON: {
     m_Ready = ParseJSON();
    }break;
    default:
     break;
    }
   } while (0);
   return m_Ready;
  }
  IwxApp* IwxApp::Create(const std::string& buffer, const SkinDataType& dataType, const EncodingType& encodingType) {
   bool result = false;
   IwxApp* app = nullptr;
   do {
    if (buffer.empty())
     break;
    if (dataType<SkinDataType::BEGIN || dataType>SkinDataType::END)
     break;
    if (encodingType<EncodingType::BEGIN || encodingType>EncodingType::END)
     break;
    app =  new IwxApp(buffer, dataType, encodingType);
    if (!app)
     break;
    result = true;
   } while (0);

   if (!result)
    SK_DELETE_PTR(app);

   return app;
  }



 }///namespace wx
}///namespace shared