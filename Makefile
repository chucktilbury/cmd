
TARGET	=	cmd
OBJS	=	cmd.o \
			mem.o \
			test.o

CARGS	=	-Wall -Wextra -Wpedantic
DEBUG	=	-g -DENA_TRACE
EXT	=	-Wno-unused-variable -Wno-sign-compare

all: $(TARGET)

%.o:%.c
	gcc $(CARGS) $(DEBUG) -c -o $@ $<

$(TARGET): $(OBJS)
	gcc $(CARGS) $(DEBUG) -o $(TARGET) $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS) *.bak
