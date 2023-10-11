
TEST	=	cmd
TARGET	=	libutil.a
OBJS	=	mem.o \
			ptrlst.o \
			str.o \
			hash.o \
			fileio.o \
			cmd.o

include ../common.make

all: $(TARGET)

%.o:%.c
	gcc $(BOPTS) -c -o $@ $<

$(TARGET): $(OBJS)
	ar crs $@ $(OBJS)

$(TEST): $(TARGET) $(OBJS)
	gcc $(LOPTS) -o $(TEST) test.c $(LIBDIRS) $(LIBS)

clean:
	rm -f $(TEST) $(TARGET) $(OBJS)

format: clean
	clang-format -i *.c *.h