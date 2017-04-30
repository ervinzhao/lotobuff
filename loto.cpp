#include <algorithm>
#include <iostream>
#include <fstream>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
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
        void AddError(const std::string & filename, int line, int column, const std::string & message) {}
};


const char *protoFileName = "/home/zhaohongjian/workspace/proto/protobuf-3.0.0/examples/addressbook.proto";
const char *dataFileName = "/home/zhaohongjian/workspace/proto/protobuf-3.0.0/examples/sample.data";

int main(int argc, char *argv[]) {

    google::protobuf::compiler::DiskSourceTree dst;
    ErrorCollector ec;
    google::protobuf::compiler::Importer importer(&dst, &ec);

    const google::protobuf::FileDescriptor *fileDescriptor = NULL;
    std::string fileName(protoFileName);
    fileDescriptor = importer.Import(fileName);
    const google::protobuf::Descriptor *descriptor = fileDescriptor->FindMessageTypeByName(std::string("AddressBook"));

    google::protobuf::DynamicMessageFactory messageFactory;
    const google::protobuf::Message *messageBase = messageFactory.GetPrototype(descriptor);

    google::protobuf::Message *message = messageBase->New();

    std::ifstream f;
    f.open(dataFileName, std::ios::binary | std::ios::in);
    message->ParseFromIstream(&f);
    return 0;
}

