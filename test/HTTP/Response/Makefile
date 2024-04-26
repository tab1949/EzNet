ROOT = ../../..
SRC = parse_benchmark.cpp ${ROOT}/src/HTTP/HTTP_Response.cpp ${ROOT}/src/HTTP/HTTP_Header.cpp
TAR = test

${TAR} : ${SRC}
	g++ ${SRC} -o ${TAR} -I ../../../include/tab -O2