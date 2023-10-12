
TEST	=	cmd
TARGET	=	libutil.a
OBJS	=	mem.o \
			ptrlst.o \
			str.o \
			hash.o \
			fileio.o \
			except.o \
			error.o \
			logging.o \
			cmd.o

WARNS	=	-Wno-unused-variable -Wno-sign-compare
LIBS	=	-lutil -lgc
LDIR	=	-L.
IDIR	=	-I.
DEBUG	=	-g
CONFIG	=	-DENA_TRACE -DUSE_GC
CARGS	=	-Wall -Wextra -Wpedantic $(DEBUG) $(CONFIG)

BOPTS	=	$(CARGS) $(IDIR)
LOPTS	=	$(CARGS) $(LDIR) $(IDIR)

all: $(TARGET)

%.o:%.c
	gcc $(BOPTS) -c -o $@ $<

$(TARGET): $(OBJS) util.h
	ar crs $@ $(OBJS)

cmd_test: $(TARGET) cmd_test.c
	gcc $(LOPTS) -o cmd_test cmd_test.c $(LIBDIRS) $(LIBS)

except_test: $(TARGET) except_test.c
	gcc $(LOPTS) -o except_test except_test.c $(LIBDIRS) $(LIBS)

hash_test: $(TARGET) hash_test.c
	gcc $(LOPTS) -o hash_test hash_test.c $(LIBDIRS) $(LIBS)

tests: cmd_test except_test hash_test

clean:
	rm -f except_test cmd_test hash_test $(TARGET) $(OBJS)

format: clean
	clang-format -i *.c *.h
