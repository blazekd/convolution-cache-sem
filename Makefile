CC=g++
CFLAGS= -O1 -Wall -Werror -lm -g

run: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp
	valgrind --tool=cachegrind --I1=32768,8,64 --D1=32768,8,64 --LL=1048576,16,64 ./main ./vit_normal.ppm

test: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp
	valgrind --tool=cachegrind --I1=32768,8,64 --D1=32768,8,64 --LL=1048576,16,64 ./main ./test.ppm

small: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp
	valgrind --tool=cachegrind --I1=32768,8,64 --D1=32768,8,64 --LL=1048576,16,64 ./main ./vit_small.ppm

compile: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp

