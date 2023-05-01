#
# Cross Platform Makefile
# Compatible with Ubuntu 14.04.1 and macOS
CXX = g++

SRCS =	../cimgui/cimgui.cpp \
		../cimgui/imgui/imgui.cpp \
		../cimgui/imgui/imgui_draw.cpp \
		../cimgui/imgui/imgui_demo.cpp \
		../cimgui/imgui/imgui_tables.cpp \
		../cimgui/imgui/imgui_widgets.cpp \
		../cimgui/imgui/backends/imgui_impl_opengl2.cpp \
		../cimgui/imgui/backends/imgui_impl_glfw.cpp

OBJS = $(SRCS:.cpp=.o)

CXXFLAGS=-std=c++11 -O2

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"

	OUTPUTNAME = libcimgui.so
	CXXFLAGS += -I../cimgui/imgui/
	CXXFLAGS += -Wall
	CXXFLAGS += -shared -fPIC
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "macOS"

	OUTPUTNAME = libcimgui.dylib
	CXXFLAGS += -I/opt/homebrew/include -I../cimgui/imgui -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_WSDL -DIMGUI_IMPL_API="extern \"C\""
	CXXFLAGS += -Wall
	LINKFLAGS = -L/opt/homebrew/lib -lglfw -dynamiclib -framework OpenGL
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "Windows"

	OUTPUTNAME = libcimgui.dll
	CXXFLAGS += -I../cimgui/imgui/
	CXXFLAGS += -Wall
	CXXFLAGS += -shared
	LINKFLAGS = -limm32
	CFLAGS = $(CXXFLAGS)
endif

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all:imgui_example
	@echo Build complete for $(ECHO_MESSAGE)

imgui_example:$(OBJS)
	$(CXX) -o $(OUTPUTNAME) $(OBJS) $(CXXFLAGS) $(LINKFLAGS)

clean:
	rm $(OBJS)

re: clean all
