TARGET=proj2

default: all

all:	
	gcc -std=gnu99 -Wall -Wextra -Werror -pedantic $(TARGET).c -o $(TARGET) 

run: all
	./$(TARGET) 3 6 100 100

run2: all
	 ./$(TARGET) 6 12 100 100

test: 
	gcc -std=gnu99 $(TARGET).c -o $(TARGET)