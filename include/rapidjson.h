#if !defined(INC_H___023AEF5D_F6B2_47C9_B549_C3D2702291AD__HEAD__)
#define INC_H___023AEF5D_F6B2_47C9_B549_C3D2702291AD__HEAD__

#include "../3rdparty/rapidjson/include/rapidjson/reader.h"
#include "../3rdparty/rapidjson/include/rapidjson/document.h"
#include "../3rdparty/rapidjson/include/rapidjson/rapidjson.h"
#include "../3rdparty/rapidjson/include/rapidjson/stringbuffer.h"
#include "../3rdparty/rapidjson/include/rapidjson/writer.h"
#include "../3rdparty/rapidjson/include/rapidjson/stream.h"

#define JSON_VALUE_TO_STRING(in,out) \
if (in.IsInt())\
out = std::to_string(in.GetInt());\
else if (in.IsUint())\
out = std::to_string(in.GetUint());\
else if (in.IsUint64())\
out = std::to_string(in.GetUint64());\
else if (in.IsInt64())\
out = std::to_string(in.GetInt64());\
else if (in.IsFalse())\
out = std::to_string(in.GetFloat());\
else if (in.IsDouble())\
out = std::to_string(in.GetDouble());\
else if (in.IsBool())\
out = in.GetBool() ? "1" : "0";\
else if(in.IsString())\
out=std::string(in.GetString(),in.GetStringLength());\

#define JSON_VALUE_FIXED(in,out)\
out=in;\
if(!out.empty())\
for(auto it=out.begin();it!=out.end();++it)\
if(*it==':')\
*it = '_';\
else if(*it=='\\')\
*it = '_';\
else if(*it=='/')\
*it = '_';\
else if(*it==',')\
*it=' ';\
else if(*it=='{')\
*it='(';\
else if(*it=='}')\
*it=')';\
else if(*it=='<')\
*it='(';\
else if(*it=='>')\
*it=')';\
else if(*it=='[')\
*it='(';\
else if(*it==']')\
*it=')';\

namespace shared {

 static bool operator<(const rapidjson::Value& obj1, const rapidjson::Value& obj2) {
  bool result = false;
  if (obj1.GetType() != obj2.GetType())
   return false;
  switch (obj1.GetType()) {
  case rapidjson::Type::kFalseType:
  case rapidjson::Type::kTrueType: {
   return obj1.GetBool() < obj2.GetBool();
  }break;
  case rapidjson::Type::kStringType: {
   return std::string(obj1.GetString(), obj1.GetStringLength()) < std::string(obj2.GetString(), obj2.GetStringLength());
  }break;
  case rapidjson::Type::kNumberType: {
   if (obj1.IsDouble())
    return obj1.GetDouble() < obj2.GetDouble();
   else if (obj1.IsUint())
    return obj1.GetUint() < obj2.GetUint();
   else if (obj1.IsInt())
    return obj1.GetInt() < obj2.GetInt();
   else if (obj1.IsFloat())
    return obj1.GetFloat() < obj2.GetFloat();
   else if (obj1.IsUint64())
    return obj1.GetUint64() < obj2.GetUint64();
   else if (obj1.IsInt64())
    return obj1.GetInt64() < obj2.GetInt64();
  }break;
  case rapidjson::Type::kObjectType:
  case rapidjson::Type::kArrayType:
  case rapidjson::Type::kNullType:
  default: {
  }break;
  }
  return false;
 }

 static bool operator>(const rapidjson::Value& obj1, const rapidjson::Value& obj2) {
  bool result = false;
  if (obj1.GetType() != obj2.GetType())
   return false;
  switch (obj1.GetType()) {
  case rapidjson::Type::kFalseType:
  case rapidjson::Type::kTrueType: {
   return obj1.GetBool() > obj2.GetBool();
  }break;
  case rapidjson::Type::kStringType: {
   return std::string(obj1.GetString(), obj1.GetStringLength()) > std::string(obj2.GetString(), obj2.GetStringLength());
  }break;
  case rapidjson::Type::kNumberType: {
   if (obj1.IsDouble())
    return obj1.GetDouble() > obj2.GetDouble();
   else if (obj1.IsUint())
    return obj1.GetUint() > obj2.GetUint();
   else if (obj1.IsInt())
    return obj1.GetInt() > obj2.GetInt();
   else if (obj1.IsFloat())
    return obj1.GetFloat() > obj2.GetFloat();
   else if (obj1.IsUint64())
    return obj1.GetUint64() > obj2.GetUint64();
   else if (obj1.IsInt64())
    return obj1.GetInt64() > obj2.GetInt64();
  }break;
  case rapidjson::Type::kObjectType:
  case rapidjson::Type::kArrayType:
  case rapidjson::Type::kNullType:
  default: {
  }break;
  }
  return false;
 }

 static bool operator==(const rapidjson::Value& obj1, const rapidjson::Value& obj2) {
  bool result = false;
  if (obj1.GetType() != obj2.GetType())
   return false;
  switch (obj1.GetType()) {
  case rapidjson::Type::kFalseType:
  case rapidjson::Type::kTrueType: {
   return obj1.GetBool() == obj2.GetBool();
  }break;
  case rapidjson::Type::kStringType: {
   return std::string(obj1.GetString(), obj1.GetStringLength()) == std::string(obj2.GetString(), obj2.GetStringLength());
  }break;
  case rapidjson::Type::kNumberType: {
   if (obj1.IsDouble())
    return obj1.GetDouble() == obj2.GetDouble();
   else if (obj1.IsUint())
    return obj1.GetUint() == obj2.GetUint();
   else if (obj1.IsInt())
    return obj1.GetInt() == obj2.GetInt();
   else if (obj1.IsFloat())
    return obj1.GetFloat() == obj2.GetFloat();
   else if (obj1.IsUint64())
    return obj1.GetUint64() == obj2.GetUint64();
   else if (obj1.IsInt64())
    return obj1.GetInt64() == obj2.GetInt64();
  }break;
  case rapidjson::Type::kObjectType:
  case rapidjson::Type::kArrayType:
  case rapidjson::Type::kNullType:
  default: {
  }break;
  }
  return false;
 }

 class Json final {
 public:
  static std::string& Replace(std::string& str, const std::string& old_value, const std::string& new_value) {
   std::string::size_type pos(0);
   pos = 0;
   while (true) {
    if ((pos = str.find(old_value, pos)) != std::string::npos) {
     str.replace(pos, old_value.length(), new_value);
     pos = pos + new_value.length() + 1;
    }
    else
     break;
   }
   return str;
  }
  static bool Sort(rapidjson::Value& inObj, rapidjson::Document& inDoc, const bool& level1 = false/*max_level<=1*/) {
   bool result = false;
   if (level1) {
    if (inObj.IsArray()) {
     do {
      if (inObj.Empty()) {
       result = true;
       break;
      }
      for (auto it = inObj.Begin(); it != inObj.End(); ++it) {
       if (it->IsArray() || it->IsObject())
        Sort(*it, inDoc, level1);
       else {
        std::vector<std::shared_ptr<rapidjson::Value>> sortCache;
        auto temp = std::make_shared<rapidjson::Value>(rapidjson::Value::GenericValue(it->GetType()));
        *temp = *it;
        sortCache.emplace_back(temp);
        std::sort(sortCache.begin(), sortCache.end(),
         [](auto& obj1, auto& obj2)->bool {
          return (*obj1) < (*obj2);
         });
        inObj.Erase(inObj.Begin(), inObj.End());
        std::for_each(sortCache.begin(), sortCache.end(),
         [&](const auto& node) {
          inObj.PushBack(*node, inDoc.GetAllocator());
         });
       }
      }
      result = true;
     } while (0);
    }
    else if (inObj.IsObject()) {
     do {
      if (inObj.ObjectEmpty()) {
       result = true;
       break;
      }


      std::vector<std::shared_ptr<rapidjson::Value>> sortCache;
      std::map<std::shared_ptr<rapidjson::Value>, std::shared_ptr<rapidjson::Value>> sortSuccessCache;
      for (auto it = inObj.MemberBegin(); it != inObj.MemberEnd(); ++it) {
       auto tempName = std::make_shared<rapidjson::Value>(rapidjson::Value::GenericValue(it->name.GetType()));
       *tempName = it->name;
       auto tempValue = std::make_shared<rapidjson::Value>(rapidjson::Value::GenericValue(it->value.GetType()));
       *tempValue = it->value;
       sortSuccessCache.emplace(std::make_pair(tempName, tempValue));
       sortCache.emplace_back(tempName);
      }
      if (sortCache.empty())
       break;
      std::sort(sortCache.begin(), sortCache.end(),
       [&](auto& obj1, auto& obj2)->bool {
        return	(*obj1) < (*obj2);
       });
      inObj.RemoveAllMembers();
      std::for_each(sortCache.begin(), sortCache.end(),
       [&](const auto& node) {
        inObj.AddMember(*node, *sortSuccessCache[node], inDoc.GetAllocator());
       });
      result = true;
     } while (0);
    }
    return result;
   }

   if (inObj.IsArray()) {
    do {
     if (inObj.Empty()) {
      result = true;
      break;
     }
     std::vector<std::shared_ptr<rapidjson::Value>> sortCache;
     for (auto it = inObj.Begin(); it != inObj.End(); ++it) {
      if (it->IsArray() || it->IsObject()) {
       Sort(*it, inDoc);
      }
      else {//!@ Json array sort.
       auto temp = std::make_shared<rapidjson::Value>(rapidjson::Value::GenericValue(it->GetType()));
       *temp = *it;
       sortCache.emplace_back(temp);
      }
     }
     if (sortCache.empty()) {
      result = true;
      break;
     }
     std::sort(sortCache.begin(), sortCache.end(),
      [](auto& obj1, auto& obj2)->bool {
       return (*obj1) < (*obj2);
      });
     inObj.Erase(inObj.Begin(), inObj.End());
     std::for_each(sortCache.begin(), sortCache.end(),
      [&](const auto& node) {
       inObj.PushBack(*node, inDoc.GetAllocator());
      });
     result = true;
    } while (0);
   }
   else if (inObj.IsObject()) {
    do {
     if (inObj.ObjectEmpty()) {
      result = true;
      break;
     }
     std::vector<std::shared_ptr<rapidjson::Value>> sortCache;
     std::map<std::shared_ptr<rapidjson::Value>, std::shared_ptr<rapidjson::Value>> sortSuccessCache;
     for (auto it = inObj.MemberBegin(); it != inObj.MemberEnd(); ++it) {
      auto tempName = std::make_shared<rapidjson::Value>(rapidjson::Value::GenericValue(it->name.GetType()));
      *tempName = it->name;
      auto tempValue = std::make_shared<rapidjson::Value>(rapidjson::Value::GenericValue(it->value.GetType()));
      *tempValue = it->value;
      sortSuccessCache.emplace(std::make_pair(tempName, tempValue));
      sortCache.emplace_back(tempName);
     }
     if (sortCache.empty()) {
      result = true;
      break;
     }
     std::sort(sortCache.begin(), sortCache.end(),
      [&](auto& obj1, auto& obj2)->bool {
       return	(*obj1) < (*obj2);
      });
     inObj.RemoveAllMembers();
     std::for_each(sortCache.begin(), sortCache.end(),
      [&](const auto& node) {
       inObj.AddMember(*node, *sortSuccessCache[node], inDoc.GetAllocator());
      });
     for (auto it = inObj.MemberBegin(); it != inObj.MemberEnd(); ++it) {
      if (it->value.IsArray() || it->value.IsObject())
       Sort(it->value, inDoc);
     }
     result = true;
    } while (0);
   }
   return true;
  }
  static std::string toString(const rapidjson::Value& valObj) {
   std::string result;
   rapidjson::StringBuffer jbuffer;
   rapidjson::Writer<rapidjson::StringBuffer> jwriter(jbuffer);
   if (valObj.Accept(jwriter))
    result = std::string(jbuffer.GetString(), jbuffer.GetLength());
   return result;
  }
  static void Compress(rapidjson::Value& targetObj, rapidjson::Value& host) {
   do {
    if (!targetObj.IsObject())
     break;
    if (targetObj.ObjectEmpty())
     break;
    for (auto itObj = targetObj.MemberBegin(); itObj != targetObj.MemberEnd();) {
     auto name = itObj->name.GetString();
     if (itObj->value.IsArray()) {
      rapidjson::SizeType aysize = itObj->value.Size();
      if (aysize <= 0) {
       itObj = targetObj.EraseMember(itObj);
       continue;
      }
      for (rapidjson::SizeType i = 0; i < aysize; ++i) {
       if (itObj->value[i].IsObject()) {
        if (itObj->value[i].IsNull() || itObj->value[i].ObjectEmpty()) {
         itObj->value.RemoveMember(itObj->value[i]);
         continue;
        }
        else
         Compress(itObj->value[i], targetObj);
       }
      }
     }
     else if (itObj->value.IsObject()) {
      if (itObj->value.ObjectEmpty() || itObj->value.IsNull()) {
       itObj = targetObj.EraseMember(itObj);
       continue;
      }
      else
       Compress(itObj->value, targetObj);
     }
     ++itObj;
    }
    if (targetObj.ObjectEmpty())
     Compress(host, targetObj);
   } while (0);
  }

  static std::string Format(const std::string& jsonPak, const std::string& levelStr = "\t") {
   std::string result;
   if (jsonPak.empty())
    return "";
   int level = 0;
   for (std::string::size_type index = 0; index < jsonPak.size(); index++) {
    char c = jsonPak[index];
    if (level > 0 && '\n' == *std::prev(jsonPak.end()))
    {
     result += levelStr;
    }
    switch (c)
    {
    case '{':
    case '[':
     result = result + c + "\n";
     level++;
     result += levelStr;
     break;
    case ',':
     result = result + c + "\n";
     result += levelStr;
     break;
    case '}':
    case ']':
     result += "\n";
     level--;
     result += levelStr;
     result += c;
     break;
    default:
     result += c;
     break;
    }
   }
   return result;
  }
 };
}

/// /*新生®（上海）**/
/// /*2022_02_21T03:50:14.3956045Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___023AEF5D_F6B2_47C9_B549_C3D2702291AD__HEAD__
