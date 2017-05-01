# See README.txt.

.PHONY: all

all: test_loto lotobuf

clean:
	rm -f test_loto
	rm -f lotobuf.so

test_loto: src/proto_codec.h src/proto_codec.cpp test/test.cpp
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ -I. src/proto_codec.cpp test/test.cpp -o test_loto `pkg-config --cflags --libs protobuf`

lotobuf: src/lotobuf.cpp src/proto_codec.h src/proto_codec.cpp
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ -ggdb3 src/proto_codec.cpp src/lotobuf.cpp -shared -fPIC -o lotobuf.so `pkg-config --cflags --libs protobuf`
