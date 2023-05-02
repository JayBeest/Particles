TARGET = particle
CC = gcc
CXX = g++
CFLAGS = -DGL_SILENCE_DEPRECATION -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW -O2 -g#-fsanitize=address
INCLUDES = -I/opt/homebrew/include -Icimgui -Icimgui/generator/output/
CXXFLAGS = -std=c++11 -I cimgui/imgui
LDFLAGS = -L. -lcimgui -L/opt/homebrew/lib -lglfw -lglew -g#-fsanitize=address
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
IMPLFLAGS = -DIMGUI_IMPL_API="extern \"C\""

SRCS =	particles.c \
		init.c \
		shaders.c

OBJS = $(SRCS:.c=.o)
IMPLOBJS = $(IMPLSRC:.cpp=.o)


ifeq ("$(VERBOSE)","1")
Q :=
VECHO = @echo
else
Q := @
VECHO = @echo
endif


all: $(TARGET)

$(TARGET): $(IMPLOBJS) $(OBJS)
	$(VECHO)
	$(VECHO) "\033[35mBuilding CuimGui lib\033[0m"
	$(Q)make -C lib
	$(VECHO)
	$(VECHO)
	$(VECHO) "\033[36mLinking binary file:     \033[0m$@ 🚨"
	$(VECHO)
	$(Q)$(CC) $(LDFLAGS) $(LIBS) $^ -o $@

%.o: %.c
	$(VECHO) "\033[34mCompiling object file:   \033[0m$@"
	$(Q)$(CC) -c $< $(CFLAGS) $(INCLUDES) -o $@

clean:
	rm -f $(OBJS) $(IMPLOBJS) $(TARGET)

re: clean all
	$(VECHO) "\033[31mRemoving object files\033[0m"
	$(VECHO)

.DEFAULT_GOAL := all
.PHONY: all clean re
