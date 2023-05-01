CC = gcc
CXX = g++
CFLAGS = -DGL_SILENCE_DEPRECATION -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW #-fsanitize=address
INCLUDES = -I/opt/homebrew/include -Icimgui -Icimgui/generator/output/
CXXFLAGS = -std=c++11 -I cimgui/imgui
LDFLAGS = -L. -lcimgui -L/opt/homebrew/lib -lglfw -lglew #-fsanitize=address
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
TARGET = particle
TARGETVBO = particle_vbo
IMPLFLAGS = -DIMGUI_IMPL_API="extern \"C\""

SRCS = particle_vbo.c
VBOSRC = particle_vbo.c

OBJS = $(SRCS:.c=.o)
VBOOBJS = $(VBOSRCS:.c=.o)
IMPLOBJS = $(IMPLSRC:.cpp=.o)

all: $(TARGET)

vbo: $(TARGETVBO)

$(TARGET): $(IMPLOBJS) $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@

$(VBOTARGET): $(IMPLOBJS) $(VBOOBJS)
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@

%.o: %.c 
	$(CC) -c $< $(CFLAGS) $(INCLUDES) -o $@

clean:
	rm -f $(OBJS) $(IMPLOBJS) $(TARGET)
	
re: clean all

.DEFAULT_GOAL := all
.PHONY: all clean re
