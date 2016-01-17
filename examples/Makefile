CXX=clang++
LIBS=$(shell pkg-config --libs protobuf) -lblosc
CXXFLAGS=-O0 -ggdb -I../include -std=c++14 $(shell pkg-config --cflags protobuf)

blosc: blosc.o ../src/blosc_stream.o
	$(CXX) $(LIBS) -o $@ $^

main: main.pb.o ../src/row_store.o ../src/blosc_stream.o main.o
	$(CXX) $(LIBS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

main.pb.h main.pb.cc: main.proto
	protoc --proto_path=. --cpp_out=. $^
