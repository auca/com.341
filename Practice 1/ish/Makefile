CFLAGS  += -g -O1 -std=gnu99 -D ISH_USE_STDLIB
LDFLAGS += -static

TARGET  = ish
OBJECTS = ish.o                   \
          ish_shell_utilities.o   \
          ish_cstring_utilities.o \
          ish_syscalls.o

.PHONY : all
all : $(TARGET)

$(TARGET) : $(OBJECTS)

$(OBJECTS) : %.o : %.c ish_common.h

.PHONY : clean
clean :
	rm -rf $(TARGET) $(OBJECTS)
