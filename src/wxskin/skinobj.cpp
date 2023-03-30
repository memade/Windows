#include "stdafx.h"

namespace shared {
 namespace wx {

  ISkinObj::ISkinObj() {

  }

  void ISkinObj::operator=(const ISkinObj& obj) {
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


  bool ISkinObj::operator<<(const std::wstring& unicode_xml_data) {
   bool result = false;
   do {
    if (unicode_xml_data.empty())
     break;




    result = true;
   } while (0);
   return result;
  }


 }///namespace wx
}///namespace shared