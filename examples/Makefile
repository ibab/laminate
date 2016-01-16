CXX=clang++
LIBS=$(shell pkg-config --libs protobuf)
CXXFLAGS=-I../include -std=c++14 $(shell pkg-config --cflags protobuf)

main: main.pb.o ../src/row_store.o main.o
	$(CXX) $(LIBS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

main.pb.h main.pb.cc: main.proto
	protoc --proto_path=. --cpp_out=. $^
