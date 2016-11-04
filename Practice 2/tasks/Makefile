CFLAGS  += -O3 -std=c99 -D_GNU_SOURCE
LDLIBS  += -lncurses

TARGET  = tasks
OBJECT  = tasks.o
SOURCES = tasks.c task_info.h

.PHONY : all
all : $(TARGET)

$(TARGET) : $(OBJECT)

$(OBJECT) : $(SOURCES)

.PHONY : clean
clean :
	rm -rf $(TARGET) $(OBJECT)

