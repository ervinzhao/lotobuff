# See README.txt.

.PHONY: all

all: test_loto

clean:
	rm -f test_loto

test_loto: loto.cpp test.cpp
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ loto.cpp test.cpp -o test_loto `pkg-config --cflags --libs protobuf`
