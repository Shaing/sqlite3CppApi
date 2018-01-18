Q               := @
CC              := g++ 
SRCS            := $(wildcard *.cpp)
OBJS            := $(SRCS:.cpp=.o)
TARGET          := sqlite3API_sample
LIBS			:= 

CFLAGS          += -std=c++11 -Wall -I ./include
LDFLAGS			+= -l sqlite3
#LDFLAGS		+= -static-libgcc -static-libstdc++

.PHONY: all clean distclean

all: .depend $(TARGET) 

%.o: %.cpp Makefile
	$(Q)echo "  Compiling '$<' ..."
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	$(Q)echo "  Building '$@' ..."
	$(Q)$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(Q)strip $(TARGET)

clean distclean: 
	$(Q)echo "  Cleaning '$(TARGET)' ..."
	$(Q)rm -f .depend *~ *.o $(TARGET) *.bak *.res

.depend dep depend:
	$(Q)echo "  Generating '$@' ..."
	$(Q)$(CC) $(CFLAGS) -M *.cpp > $@

ifeq (.depend, $(wildcard .depend))
    include .depend
endif

