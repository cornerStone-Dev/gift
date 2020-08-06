
all: bin tool_output tool src gift.o

gift.o: florida_c/bin/flct hashTable/hashTable.h src/source.c \
tool_output/giftLex.c
	gcc -Os -march=native -fno-builtin-strlen -fno-builtin-memmove \
	-fno-builtin-memset -fno-stack-protector -c -o gift.o src/source.c \
	-Wall -Wextra -Wno-unused-function
	size gift.o

tool_output/giftLex.c: fl_src/giftLex.re
	re2c -W fl_src/giftLex.re -o tool_output/giftLex.c

src/source.c: fl_src/*.c
	./florida_c/bin/flct -dsrc

bin:
	mkdir bin

tool:
	mkdir tool

src:
	mkdir src

tool_output:
	mkdir tool_output

florida_c/bin/flct:
	git clone https://github.com/cornerStone-Dev/florida_c.git
	(cd florida_c && make)

hashTable/hashTable.h:
	git clone https://github.com/cornerStone-Dev/hashTable.git

test: bin/giftTest
	time ./bin/giftTest

bin/giftTest: giftTest.c gift.o 
	gcc -O2 -march=native giftTest.c -s -o bin/giftTest gift.o -Wall

clean:
	rm -f gift.o
	rm -f tool_output/giftLex.c
