#include <algorithm>
#include <iostream>
#include <fstream>

#include "proto_codec.h"

const char *protoFileName = "/home/zhaohongjian/workspace/lotobuf/examples/addressbook.proto";
const char *dataFileName = "/home/zhaohongjian/workspace/lotobuf/examples/sample.data";

int main(int argc, char *argv[]) {
    ProtoCodec loto;
    loto.load(protoFileName);
    return 0;
}

