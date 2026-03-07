IDIR = ./include
SRC_DIRS  = ./src/main
CC = g++-13
CFLAGS= -o

TARGET_EXEC = final_project

SRCS = $(shell find $(SRC_DIRS) -name '*.cpp')

build: $(SRCS) $(IDIR)
	$(CC) $(CFLAGS) $(TARGET_EXEC) -I $(IDIR) $(SRCS)

run:
	./$(TARGET_EXEC)
clean:
	rm $(TARGET_EXEC)