FROM ubuntu:18.04

RUN apt-get update && \
	apt-get install -y build-essential git cmake autoconf libtool pkg-config

WORKDIR /src

COPY CMakeLists.txt main.cpp Test1.cpp Test1.h ./

RUN cmake . && make

ENTRYPOINT ["./test3"]
