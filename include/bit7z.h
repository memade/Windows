#if !defined(INC_H___5CC4372A_8264_41B9_9472_3998700D6876__HEAD__)
#define INC_H___5CC4372A_8264_41B9_9472_3998700D6876__HEAD__

#include "bit7z.hpp"
#include "bit7zlibrary.hpp"

namespace shared {

 class Bit7zExtractObj {
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
  typedef std::function<bool(
   const std::uint64_t& target_total_size,
   const std::uint64_t& extract_total_size,
   const std::uint64_t& current_extract_total_size,
   const double& progress)> tfUncompressProgressCb;
  typedef std::function<void(std::wstring filename)> tfUncompressFilenameCb;
 public:
  Bit7zExtractObj(
   const std::wstring& dll7zPathname,
   const std::wstring& compress_filepathname,
   const std::wstring& uncompress_path,
   const tfUncompressProgressCb& uncompress_progress_cb = nullptr,
   const tfUncompressFilenameCb& uncompress_filename_cb = nullptr,
   const bool& exception_cleanup_flag = true) :
   m_CompressFilePathname(compress_filepathname),
   m_UncompressPath(uncompress_path),
   m_tfUncompressProgressCb(uncompress_progress_cb),
   m_tfUncompressFilenameCb(uncompress_filename_cb),
   m_Dll7zPathname(dll7zPathname),
   m_ExceptionCleanup(exception_cleanup_flag) {

  }
  ~Bit7zExtractObj() {  }
 public:
  bool Perform() {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_Mutex };
   try {
    do {
     if (m_Dll7zPathname.empty())
      break;
     if (m_UncompressPath.empty())
      break;
     if (m_CompressFilePathname.empty())
      break;
     if (!shared::Win::AccessW(m_Dll7zPathname))
      break;
     if (!shared::Win::AccessW(m_CompressFilePathname))
      break;
     auto input_file_size = shared::Win::File::ReadFileSizeW(m_CompressFilePathname);
     if (input_file_size <= 0)
      break;
     m_TargetDataTotalSize = input_file_size;
     if (!shared::Win::AccessW(m_UncompressPath))
      shared::Win::CreateDirectoryW(m_UncompressPath);
     if (!shared::Win::AccessW(m_UncompressPath))
      break;
     m_pBit7zLibobj = std::make_shared<bit7z::Bit7zLibrary>(m_Dll7zPathname);
     if (!m_pBit7zLibobj)
      break;
     FileType fileType = shared::Win::GetFileTypeByDiskW(m_CompressFilePathname);
     switch (fileType) {
     case FileType::_7Z: {
      m_pBitExtractor = std::make_shared<bit7z::BitExtractor>(*m_pBit7zLibobj, bit7z::BitFormat::SevenZip);
      m_pBitArchiveInfo = std::make_shared<bit7z::BitArchiveInfo>(*m_pBit7zLibobj, m_CompressFilePathname, bit7z::BitFormat::SevenZip);
     }break;
     case FileType::_RAR:
      [[fallthrough]];
     case FileType::_RAR_v4x: {
      m_pBitExtractor = std::make_shared<bit7z::BitExtractor>(*m_pBit7zLibobj, bit7z::BitFormat::Rar);
      m_pBitArchiveInfo = std::make_shared<bit7z::BitArchiveInfo>(*m_pBit7zLibobj, m_CompressFilePathname, bit7z::BitFormat::Rar);
     }break;
     case FileType::_RAR_v5: {
      m_pBitExtractor = std::make_shared<bit7z::BitExtractor>(*m_pBit7zLibobj, bit7z::BitFormat::Rar5);
      m_pBitArchiveInfo = std::make_shared<bit7z::BitArchiveInfo>(*m_pBit7zLibobj, m_CompressFilePathname, bit7z::BitFormat::Rar5);
     }break;
     case FileType::_ZIP: {
      m_pBitExtractor = std::make_shared<bit7z::BitExtractor>(*m_pBit7zLibobj, bit7z::BitFormat::Zip);
      m_pBitArchiveInfo = std::make_shared<bit7z::BitArchiveInfo>(*m_pBit7zLibobj, m_CompressFilePathname, bit7z::BitFormat::Zip);
     }break;
     case FileType::_ISO: {
      m_pBitExtractor = std::make_shared<bit7z::BitExtractor>(*m_pBit7zLibobj, bit7z::BitFormat::Iso);
      m_pBitArchiveInfo = std::make_shared<bit7z::BitArchiveInfo>(*m_pBit7zLibobj, m_CompressFilePathname, bit7z::BitFormat::Iso);
     }break;
     case FileType::_BZIP2: {
      m_pBitExtractor = std::make_shared<bit7z::BitExtractor>(*m_pBit7zLibobj, bit7z::BitFormat::BZip2);
      m_pBitArchiveInfo = std::make_shared<bit7z::BitArchiveInfo>(*m_pBit7zLibobj, m_CompressFilePathname, bit7z::BitFormat::BZip2);
     }break;
     case FileType::_CAB: {
      m_pBitExtractor = std::make_shared<bit7z::BitExtractor>(*m_pBit7zLibobj, bit7z::BitFormat::Cab);
      m_pBitArchiveInfo = std::make_shared<bit7z::BitArchiveInfo>(*m_pBit7zLibobj, m_CompressFilePathname, bit7z::BitFormat::Cab);
     }break;
     case FileType::_TAR: {
      m_pBitExtractor = std::make_shared<bit7z::BitExtractor>(*m_pBit7zLibobj, bit7z::BitFormat::Tar);
      m_pBitArchiveInfo = std::make_shared<bit7z::BitArchiveInfo>(*m_pBit7zLibobj, m_CompressFilePathname, bit7z::BitFormat::Tar);
     }break;
     case FileType::_GZIP: {
      m_pBitExtractor = std::make_shared<bit7z::BitExtractor>(*m_pBit7zLibobj, bit7z::BitFormat::GZip);
      m_pBitArchiveInfo = std::make_shared<bit7z::BitArchiveInfo>(*m_pBit7zLibobj, m_CompressFilePathname, bit7z::BitFormat::GZip);
     }break;
     default:
      break;
     }
     if (!m_pBitExtractor || !m_pBitArchiveInfo)
      break;
     m_ExtractDataTotalSize = m_pBitArchiveInfo->size();
     if (m_ExtractDataTotalSize <= 0)
      break;

     m_pBitExtractor->setProgressCallback(
      [this](std::uint64_t current_extract_total_size) {
       do {
        if (current_extract_total_size <= 0)
         break;
        if (!m_tfUncompressProgressCb)
         break;
        if (m_CurrentExtractTotalSizePrev.load() == current_extract_total_size)
         break;
        if (!m_tfUncompressProgressCb(m_TargetDataTotalSize, m_ExtractDataTotalSize, current_extract_total_size, ((1.0 * current_extract_total_size) / m_ExtractDataTotalSize)))
         throw bit7z::BitException("Use cancel or stop.");
        m_CurrentExtractTotalSizePrev.store(current_extract_total_size);
       } while (0);
      });
     m_pBitExtractor->setFileCallback(m_tfUncompressFilenameCb);
     m_pBitExtractor->extract(m_CompressFilePathname, m_UncompressPath);
    } while (0);
    result = true;
   }
   catch (const bit7z::BitException& ex) {
    std::cout << ex.what() << std::endl;
   }
   catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
   }
   if (m_ExceptionCleanup && !result)
    shared::Win::EmptyFolder(shared::IConv::WStringToMBytes(m_UncompressPath));
   return result;
  }
 private:
  const bool m_ExceptionCleanup;
  std::uint64_t m_ExtractDataTotalSize = 0;
  std::uint64_t m_TargetDataTotalSize = 0;
  std::atomic_uint64_t m_CurrentExtractTotalSizePrev = 0;
  std::shared_ptr<bit7z::Bit7zLibrary> m_pBit7zLibobj = nullptr;
  std::shared_ptr<bit7z::BitExtractor> m_pBitExtractor = nullptr;
  std::shared_ptr<bit7z::BitArchiveInfo> m_pBitArchiveInfo = nullptr;
  const std::wstring m_CompressFilePathname;
  const std::wstring m_UncompressPath;
  const std::wstring m_Dll7zPathname;
  const tfUncompressProgressCb m_tfUncompressProgressCb;
  const tfUncompressFilenameCb m_tfUncompressFilenameCb;
 };


}///namespace shared

#pragma comment(lib,"bit7z")
/// /*新生®（上海）**/
/// /*2022_09_29T11:44:22.9772571Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___5CC4372A_8264_41B9_9472_3998700D6876__HEAD__
