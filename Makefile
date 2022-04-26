TARGET=proj2

default: all

all:	
	gcc -std=gnu99 -Wall -Wextra -Werror -pedantic $(TARGET).c -o $(TARGET) 

run: all
	./$(TARGET)