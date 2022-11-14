#include "stdafx.h"

namespace shared {
 namespace wx {

#if 0
  wxPointEx::wxPointEx(const std::wstring& point_string) {
   *this = point_string;
  }
  void wxPointEx::operator=(const std::wstring& point_string) {
   std::vector<std::wstring> parse;
   shared::Win::File::ParseW(point_string, ',', parse);
   if (parse.size() == 2) {
    x = ::_wtoi(parse[0].c_str());
    y = ::_wtoi(parse[1].c_str());
   }
   else {
    x = wxDefaultPosition.x;
    y = wxDefaultPosition.y;
   }
  }
  wxPoint wxPointEx::toPoint() const {
   return wxPoint(x, y);
  }

  wxPositionEx::wxPositionEx(const std::wstring& pos_string) {
   std::vector<std::wstring> parse;
   shared::Win::File::ParseW(pos_string, ',', parse);
   if (parse.size() == 2) {
    SetRow(::_wtoi(parse[0].c_str()));
    SetColumn(::_wtoi(parse[1].c_str()));
   }
   else {
    SetRow(wxDefaultPosition.x);
    SetColumn(wxDefaultPosition.y);
   }
  }
  wxRectEx::wxRectEx(const std::wstring& string) {
   std::vector<std::wstring> parse;
   shared::Win::File::ParseW(string, ',', parse);
   if (parse.size() == 4) {
    x = ::_wtoi(parse[0].c_str());
    y = ::_wtoi(parse[1].c_str());
    width = ::_wtoi(parse[2].c_str());
    height = ::_wtoi(parse[3].c_str());
   }
  }
#endif


 }///namespace wx
}///namespace shared