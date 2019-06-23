CFLAGS = -O3 -fexceptions
CPPFLAGS = -I../include -I../../fontlib/include
LDFLAGS =
FONTS = $(wildcard ../../fontlib/src/*.cpp)
OBJECTS = $(FONTS:.cpp=.o)
appname = test

all: $(appname)
clean:
	rm -f $(appname) *.o
.PHONY: all clean

sdl_cflags := $(shell pkg-config --cflags sdl2)
sdl_libs := $(shell pkg-config --libs sdl2)
override CFLAGS += $(sdl_cflags)
override LIBS += $(sdl_libs)

$(appname): Main.o $(OBJECTS)
	echo $(OBJECTS)
	g++ $(LDFLAGS) -o $@ $^ $(LIBS)

