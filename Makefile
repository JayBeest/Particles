CC = gcc
CXX = g++
CFLAGS = -DGL_SILENCE_DEPRECATION -DCIMGUI_USE_OPENGL2 -DCIMGUI_USE_GLFW
INCLUDES = -I/opt/homebrew/include -Icimgui -Icimgui/generator/output/
CXXFLAGS = -std=c++11 -I cimgui/imgui
LDFLAGS = -L. -lcimgui -L/opt/homebrew/lib -lglfw
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
TARGET = particle
IMPLFLAGS = -DIMGUI_IMPL_API="extern \"C\""

SRCS = particle.c

OBJS = $(SRCS:.c=.o)
IMPLOBJS = $(IMPLSRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(IMPLOBJS) $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@

%.o: %.c 
	$(CC) -c $< $(CFLAGS) $(INCLUDES) -o $@

clean:
	rm -f $(OBJS) $(IMPLOBJS) $(TARGET)
	
re: clean all

.DEFAULT_GOAL := all
.PHONY: all clean re
