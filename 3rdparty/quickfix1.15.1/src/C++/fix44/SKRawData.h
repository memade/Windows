#if !defined(AFX_SK_H__C6CE2BC6_7126_460F_861B_BED4D53D78FC__HEAD__)
#define AFX_SK_H__C6CE2BC6_7126_460F_861B_BED4D53D78FC__HEAD__

#include "Message.h"

namespace FIX44
{
    class SKRawDataRead : public Message
    {
    public:
        SKRawDataRead() : Message(MsgType()) {}
        SKRawDataRead(const FIX::Message& m) : Message(m) {}
        SKRawDataRead(const Message& m) : Message(m) {}
        SKRawDataRead(const SKRawDataRead& m) : Message(m) {}
        static FIX::MsgType MsgType() { return FIX::MsgType("SKR"); }

        FIELD_SET(*this, FIX::CommandID);
        FIELD_SET(*this, FIX::RawData);
        FIELD_SET(*this, FIX::RawDataLength);
    };

    typedef class SKRawDataWrite : public Message
    {
    public:
        SKRawDataWrite() : Message(MsgType()) {}
        SKRawDataWrite(const FIX::Message& m) : Message(m) {}
        SKRawDataWrite(const Message& m) : Message(m) {}
        SKRawDataWrite(const SKRawDataWrite& m) : Message(m) {}
        static FIX::MsgType MsgType() { return FIX::MsgType("SKW"); }

        SKRawDataWrite(
            const FIX::CommandID& commandID,
            const FIX::RawData& rawData,
            const FIX::RawDataLength& rawDataLength,
            const FIX::SourceDataLength& sourceDataLength)
            : Message(MsgType())
        {
            set(commandID);
            set(rawData);
            set(rawDataLength);
            set(sourceDataLength);
        }

        SKRawDataWrite(const FIX::CommandID& commandID)
            : Message(MsgType())
        {
            set(commandID);
        }

        SKRawDataWrite(const unsigned long long& command)
            : Message(MsgType())
        {
            set(FIX::CommandID(std::to_string(command)));
        }

        FIELD_SET(*this, FIX::CommandID);
        FIELD_SET(*this, FIX::RawData);
        FIELD_SET(*this, FIX::RawDataLength);
        FIELD_SET(*this, FIX::SourceDataLength);
    }Response;

}

//! @/*新生联创®（上海）*/
//! @/*Fri Nov 20 10:34:05 UTC+0800 2020*/
//! @/*___www.skstu.com___*/
#endif/*AFX_SK_H__C6CE2BC6_7126_460F_861B_BED4D53D78FC__HEAD__*/

