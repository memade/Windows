#if !defined(AFX_SK_H__C6CE2BC6_7126_460F_861B_BED4D53D78FC__HEAD__)
#define AFX_SK_H__C6CE2BC6_7126_460F_861B_BED4D53D78FC__HEAD__

#include "Message.h"

namespace FIX44
{
	class SourceRawData : public Message
	{
	public:
		SourceRawData() : Message(MsgType()) {}
		SourceRawData(const FIX::Message& m) : Message(m) {}
		SourceRawData(const Message& m) : Message(m) {}
		SourceRawData(const SourceRawData& m) : Message(m) {}
		static FIX::MsgType MsgType() { return FIX::MsgType("SRD"); }

		SourceRawData(
			const FIX::CommandID& commandID,
			const FIX::RawData& rawData,
			const FIX::RawDataLength& rawDataLength,
			const FIX::SourceDataLength& sourceDataLength,
			const FIX::SourceDataLastFlag& lastFlag)
			: Message(MsgType())
		{
			set(commandID);
			set(rawData);
			set(rawDataLength);
			set(sourceDataLength);
			set(lastFlag);
		}

		SourceRawData(
			const unsigned long long& command,
			const std::string& packet)
			: Message(MsgType())
		{
			set(FIX::CommandID(std::to_string(command)));
			set(FIX::SourceDataLastFlag(true));
			if (!packet.empty())
			{
				set(FIX::RawData(packet));
				set(FIX::RawDataLength(static_cast<std::uint32_t>(packet.size())));
				set(FIX::SourceDataLength(static_cast<std::uint32_t>(packet.size())));
			}
		}

		SourceRawData(
			const unsigned long long& command,
			const std::string& packet,
			const bool& last)
			: Message(MsgType())
		{
			set(FIX::CommandID(std::to_string(command)));
			set(FIX::SourceDataLastFlag(last));
			if (!packet.empty())
			{
				set(FIX::RawData(packet));
				set(FIX::RawDataLength(static_cast<std::uint32_t>(packet.size())));
				set(FIX::SourceDataLength(static_cast<std::uint32_t>(packet.size())));
			}
		}

		SourceRawData(const unsigned long long& command)
			: Message(MsgType())
		{
			set(FIX::CommandID(std::to_string(command)));
			set(FIX::SourceDataLastFlag(true));
		}

		FIELD_SET(*this, FIX::CommandID);
		FIELD_SET(*this, FIX::RawData);
		FIELD_SET(*this, FIX::RawDataLength);
		FIELD_SET(*this, FIX::SourceDataLength);
		FIELD_SET(*this, FIX::SourceDataLastFlag);
	};

}

//! @/*新生联创®（上海）*/
//! @/*Fri Nov 20 10:34:05 UTC+0800 2020*/
//! @/*___www.skstu.com___*/
#endif/*AFX_SK_H__C6CE2BC6_7126_460F_861B_BED4D53D78FC__HEAD__*/

