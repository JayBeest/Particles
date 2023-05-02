CXX = g++

SRCS =	../cimgui/cimgui.cpp \
		../cimgui/imgui/imgui.cpp \
		../cimgui/imgui/imgui_draw.cpp \
		../cimgui/imgui/imgui_demo.cpp \
		../cimgui/imgui/imgui_tables.cpp \
		../cimgui/imgui/imgui_widgets.cpp \
		../cimgui/imgui/backends/imgui_impl_opengl3.cpp \
		../cimgui/imgui/backends/imgui_impl_glfw.cpp

OBJS = $(SRCS:.cpp=.o)

CXXFLAGS=-std=c++11 -O2

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"

	NAME = libcimgui.so
	CXXFLAGS += -I../cimgui/imgui/
	CXXFLAGS += -Wall
	CXXFLAGS += -shared -fPIC
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "macOS"

	NAME = libcimgui.dylib
	CXXFLAGS += -I/opt/homebrew/include -I../cimgui/imgui -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_WSDL -DIMGUI_IMPL_API="extern \"C\""
	CXXFLAGS += -Wall
	LINKFLAGS = -L/opt/homebrew/lib -lglfw -dynamiclib -framework OpenGL
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "Windows"

	NAME = libcimgui.dll
	CXXFLAGS += -I../cimgui/imgui/
	CXXFLAGS += -Wall
	CXXFLAGS += -shared
	LINKFLAGS = -limm32
	CFLAGS = $(CXXFLAGS)
endif

ifeq ("$(VERBOSE)","1")
Q :=
VECHO = @echo
else
Q := @
VECHO = @echo
endif


.cpp.o:
	$(VECHO) "\033[34mCompiling object file:   \033[0m$@"
	$(Q)$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(NAME)
	$(Q)cp ./libcimgui.dylib ../
	@echo Build complete for $(ECHO_MESSAGE)

$(NAME): $(OBJS)
	$(Q)$(CXX) $(CXXFLAGS) $(LINKFLAGS) $(OBJS) -o $@

clean:
	$(VECHO) "\033[31mRemoving CimGui object files\033[0m"
	$(VECHO)
	$(Q)rm $(OBJS) $(NAME)

re: clean all
