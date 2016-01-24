CXX=clang++
LIBS=$(shell pkg-config --libs protobuf) -lblosc
CXXFLAGS=-O3 -g -fno-omit-frame-pointer -I../include -std=c++14 $(shell pkg-config --cflags protobuf)

all: row_store blosc

blosc: blosc.o ../src/blosc_stream.o
	$(CXX) $(LIBS) -o $@ $^

row_store: main.pb.o ../src/row_store.o ../src/blosc_stream.o row_store.o
	$(CXX) $(LIBS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

main.pb.h main.pb.cc: main.proto
	protoc --proto_path=. --cpp_out=. $^
