CXX=clang++
LIBS=$(shell pkg-config --libs protobuf) -lblosc
CXXFLAGS=-ggdb -O0 -I../include -std=c++14 $(shell pkg-config --cflags protobuf)

all: row_store blosc

blosc: blosc.o ../src/blosc_stream.o
	$(CXX) $(LIBS) -o $@ $^

row_store: main.pb.o ../src/row_store.o ../src/blosc_stream.o main.o
	$(CXX) $(LIBS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

main.pb.h main.pb.cc: main.proto
	protoc --proto_path=. --cpp_out=. $^
