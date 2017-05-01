#ifndef _PROTO_CODEC_H_
#define _PROTO_CODEC_H_

#include <map>
#include <algorithm>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

class ErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector
{
    public:
        ErrorCollector()  {}
        ~ErrorCollector() {}
        void AddError(const std::string & filename, int line, int column, const std::string & message);
};

using namespace google;

class ProtoCodec {
public:
    ProtoCodec();
    ~ProtoCodec();

    void map(const std::string &vpath, const std::string &dpath);
    bool load(const char *filename);
    const protobuf::Message *getMessageType(const char *messageName);
    protobuf::Message *decode(const protobuf::Message *messageBase,
                              const std::string &messageContent);
    bool encode(protobuf::Message *message, std::string *output);

private:
    protobuf::compiler::DiskSourceTree m_dst;
    ErrorCollector m_ec;

    protobuf::compiler::Importer m_importer;
    const protobuf::FileDescriptor *m_fileDescriptor;
    protobuf::DynamicMessageFactory m_messageFactory;

    typedef std::map<const std::string, const protobuf::Message *> MessagePoolType;
    MessagePoolType m_messagePool;
};

#endif // _PROTO_CODEC_H_
