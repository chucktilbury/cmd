
TARGET	=	cmd
OBJS	=	cmd.o \
			mem.o \
			item.o \
			line.o \
			ptrlst.o \
			str.o

CARGS	=	-Wall -Wextra -Wpedantic
DEBUG	=	-g -DENA_TRACE
EXT	=	-Wno-unused-variable -Wno-sign-compare

all: $(TARGET)

%.o:%.c
	gcc $(CARGS) $(DEBUG) -c -o $@ $<

libcmd.a: $(OBJS)
	ar crs $@ $<

$(TARGET): $(OBJS)
	gcc $(CARGS) $(DEBUG) -o $(TARGET) -lcmd

clean:
	rm -f $(TARGET) $(OBJS) *.bak
