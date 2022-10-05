#include "stdafx.h"

namespace shared {
#if ENABLE_OPENSSL
 const std::map<char, char> GoogleChromeExtensionsIDMapping = {
{'0', 'a'},
{'1', 'b'},
{'2', 'c'},
{'3', 'd'},
{'4', 'e'},
{'5', 'f'},
{'6', 'g'},
{'7', 'h'},
{'8', 'i'},
{'9', 'j'},
{'a', 'k'},
{'b', 'l'},
{'c', 'm'},
{'d', 'n'},
{'e', 'o'},
{'f', 'p'},
 };

 bool Encryption::ChromeExtensionsIDGenerate(std::string& outRSAPrivate_pkcs8, std::string& outRSAPublic_pkcs8, std::string& outRSAPublicDER, std::string& outExtensionsID) {
  bool result = false;
  outRSAPrivate_pkcs8.clear();
  outRSAPublic_pkcs8.clear();
  outRSAPublicDER.clear();
  outExtensionsID.clear();
  do {
   if (!Rsa::Generate(Rsa::EnType::EN_TYPE_PKCS8, "", outRSAPublic_pkcs8, outRSAPrivate_pkcs8))
    break;

   if (!Rsa::DERPublic(Rsa::EnType::EN_TYPE_PKCS8, outRSAPublic_pkcs8, "", outRSAPublicDER))
    break;

   std::string sha256;
   if (!SHA256(outRSAPublicDER, true, sha256))
    break;

   outExtensionsID.resize(32, 0);
   for (size_t i = 0; i < outExtensionsID.size(); ++i) {
    auto found = GoogleChromeExtensionsIDMapping.find(sha256[i]);
    if (found != GoogleChromeExtensionsIDMapping.end())
     outExtensionsID[i] = found->second;
   }

   std::string keyPublicDERBase64;
   if (Base64Encode(outRSAPublicDER, keyPublicDERBase64, false))
    outRSAPublicDER = keyPublicDERBase64;
   else
    break;

   result = true;
  } while (0);
  if (!result) {
   outRSAPrivate_pkcs8.clear();
   outRSAPublic_pkcs8.clear();
   outRSAPublicDER.clear();
   outExtensionsID.clear();
  }
  return result;
 }

 bool Encryption::ChromeExtensionsIDGet(const std::string& inRSAPrivate_pkcs8, std::string& outRSAPublic_pkcs8, std::string& outRSAPublicDER, std::string& outExtensionsID) {
  bool result = false;
  outRSAPublic_pkcs8.clear();
  outRSAPublicDER.clear();
  outExtensionsID.clear();
  do {
   if (inRSAPrivate_pkcs8.empty())
    break;

   if (!Rsa::PrivateToPublic(Rsa::EnType::EN_TYPE_PKCS8, "", inRSAPrivate_pkcs8, outRSAPublic_pkcs8))
    break;

   if (!Rsa::DERPublic(Rsa::EnType::EN_TYPE_PKCS8, outRSAPublic_pkcs8, "", outRSAPublicDER))
    break;

   std::string sha256;
   if (!SHA256(outRSAPublicDER, true, sha256))
    break;

   outExtensionsID.resize(32, 0);
   for (size_t i = 0; i < outExtensionsID.size(); ++i) {
    auto found = GoogleChromeExtensionsIDMapping.find(sha256[i]);
    if (found != GoogleChromeExtensionsIDMapping.end())
     outExtensionsID[i] = found->second;
   }

   std::string keyPublicDERBase64;
   if (Base64Encode(outRSAPublicDER, keyPublicDERBase64, false))
    outRSAPublicDER = keyPublicDERBase64;
   else
    break;

   result = true;
  } while (0);
  if (!result) {
   outRSAPublic_pkcs8.clear();
   outRSAPublicDER.clear();
   outExtensionsID.clear();
  }
  return result;
 }
#endif
}///namespace shared