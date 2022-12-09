#include "stdafx.h"

namespace shared {
 static const char route_packet_head_tag = 0x23;
 static const char route_packet_tail_tag = 0x21;

 bool Win::Packet::Made(const unsigned long long& in_cmd, const std::string& in_buffer, std::string& out_buffer) {
  bool result = false;
  out_buffer.clear();
  do {
   if (in_cmd <= 0)
    break;
   std::string temp_buffer;
   tagPacketHead head;
   head.command = in_cmd;
   head.size_data = static_cast<decltype(PACKETHEAD::size_data)>(in_buffer.size());
   head.size_total = head.size_data + head.size_head_real;
   temp_buffer.append(std::string((char*)&head, head.size_head_real));
   if (in_buffer.empty())
    head.data[0] = 0;
   else
    temp_buffer.append(in_buffer);
   out_buffer = Win::Encryption::WemadeEncode(temp_buffer);
   result = !out_buffer.empty();
  } while (0);
  return result;
 }

 size_t Win::Packet::UnMade(std::string& in_buffer, std::vector<std::string>& out_buffer_s, const bool& fixed_buffer /*= true*/) {
  size_t result = 0;
  out_buffer_s.clear();
  do {
   if (in_buffer.empty())
    break;
   char* begin = nullptr;
   size_t pos = 0;
   size_t current_pak_size = 0;
   do {
    if (in_buffer.empty())
     break;
    char* begin = &in_buffer[0];
    if (*begin != route_packet_head_tag) {
     do {
      if (in_buffer.empty() || in_buffer[0] == route_packet_head_tag) {
       begin = &in_buffer[0];
       break;
      }
      in_buffer.erase(in_buffer.begin());
     } while (1);
    }
    if (in_buffer.empty())
     break;
    pos = in_buffer.find(route_packet_tail_tag);
    if (pos == std::string::npos)
     break;
    current_pak_size = pos + sizeof(route_packet_tail_tag);
    result += current_pak_size;
    auto dedata = shared::Win::Encryption::WemadeDecode(std::string(begin, current_pak_size));
    do {
     if (dedata.empty())
      break;
     if (!((PPACKETHEAD)dedata.data())->Verify())
      break;
     out_buffer_s.emplace_back(dedata);
    } while (0);

    in_buffer.erase(0, current_pak_size);
    if (in_buffer.empty())
     break;
   } while (1);

  } while (0);
  return result;
 }

 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 tagPacketHead::tagPacketHead() {
  ::memset(this, 0x00, sizeof(*this));
  identify_head = LOGO_OF_DEVELOPER_BEGIN;
  identify_tail = LOGO_OF_DEVELOPER_END;
  size_head_std = sizeof(*this);
  size_head_real = sizeof(*this) - sizeof(data);
 }
 bool tagPacketHead::Verify() const {
  return identify_head == LOGO_OF_DEVELOPER_BEGIN && identify_tail == LOGO_OF_DEVELOPER_END;
 }
}///namespace shared