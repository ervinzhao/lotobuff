#include <algorithm>
#include <iostream>
#include <fstream>

#include "src/proto_codec.h"

const char *protoFileName = "addressbook.proto";

int main(int argc, char *argv[]) {
    ProtoCodec loto;
    loto.map("", "examples/");
    bool result = loto.load(protoFileName);
    if(result == false) {
        std::cout<<loto.lastError()<<std::endl;
    }
    return 0;
}

