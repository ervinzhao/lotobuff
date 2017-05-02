#include <iostream>
#include <sstream>
#include "proto_codec.h"

ProtoCodec::ProtoCodec()
    : m_importer(&m_dst, &m_ec)
{
    m_fileDescriptor = NULL;
}

ProtoCodec::~ProtoCodec()
{

}

void ProtoCodec::map(const std::string &vpath, const std::string &dpath)
{
    m_dst.MapPath(vpath, dpath);
}


bool ProtoCodec::load(const char *filename)
{
    m_ec.reset();
    bool result;
    m_fileDescriptor = m_importer.Import(filename);
    if(m_fileDescriptor == NULL) {
        result = false;
    } else {
        int count = m_fileDescriptor->message_type_count();
        for(int i = 0; i < count; i++) {
            const protobuf::Descriptor *d = m_fileDescriptor->message_type(i);
            const protobuf::Message *messageBase = m_messageFactory.GetPrototype(d);

            const std::string &name = d->name();
            m_messagePool[name] = messageBase;
        }
        result = true;
    }
    return result;
}

const google::protobuf::Message *ProtoCodec::getMessageType(const char *messageName)
{
    std::string key(messageName);
    MessagePoolType::iterator iter = m_messagePool.find(key);
    if(iter == m_messagePool.end()) {
        return NULL;
    }
    const protobuf::Message *messageBase = iter->second;
    return messageBase;
}

protobuf::Message *ProtoCodec::decode(const protobuf::Message *messageBase,
                                              const std::__cxx11::string &messageContent)
{
    protobuf::Message *message = messageBase->New();
    bool ret = message->ParseFromString(messageContent);
    if(ret == false) {
        delete message;
        return NULL;
    }
    return message;
}

bool ProtoCodec::encode(protobuf::Message *message, std::__cxx11::string *output)
{
    bool ret = message->IsInitialized();
    if(ret == false)
        return ret;
    ret = message->SerializePartialToString(output);
    return ret;
}


void ErrorCollector::AddError(const std::__cxx11::string &filename, int line, int column, const std::__cxx11::string &message)
{
    if(m_used == false) {
        m_used = true;
        if(line == -1) {
            m_error = message;
        } else {
            std::stringstream ss;
            ss<<"Line: "<<line<<", Column: "<<column<<", "<<message<<"\n";
            m_error = ss.str();
        }
    }
}
