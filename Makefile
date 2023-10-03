
TEST	=	cmd
TARGET	=	libutil.a
OBJS	=	mem.o \
			ptrlst.o \
			str.o \
			hash.o \
			cmd.o

DEBUG	=	-g
EXT		=	-Wno-unused-variable -Wno-sign-compare
CONFIG	=	-DENA_TRACE -DUSE_GC -lgc
LIBDIRS	=	-L.
LIBS	=	-lutils -lgc
CARGS	=	$(CONFIG) -Wall -Wextra -Wpedantic

all: $(TARGET)

%.o:%.c
	gcc $(CARGS) $(DEBUG) -c -o $@ $<

$(TARGET): $(OBJS)
	ar crs $@ $(OBJS)

$(TEST): $(TARGET) $(OBJS)
	gcc $(CARGS) $(DEBUG) -o $(TEST) test.c $(LIBDIRS) $(LIBS)

clean:
	rm -f $(TEST) $(TARGET) $(OBJS)
