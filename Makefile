TARGET = particles
CC = gcc
CXX = c++
CFLAGS = -DGL_SILENCE_DEPRECATION -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW -O2 #-fsanitize=address
INCLUDES =  -Icimgui -Icimgui/generator/output/
CXXFLAGS = -std=c++11 -I cimgui/imgui

IMPLFLAGS = -DIMGUI_IMPL_API="extern \"C\""

SRCS =	particles.c \
		init.c \
		render.c \
		forces.c \
		update.c \
		gui.c \
		colors.c
#		shaders.c

OBJS = $(SRCS:.c=.o)
IMPLOBJS = $(IMPLSRC:.cpp=.o)

UNAME		=	$(shell uname)
UNAME_P		=	$(shell uname -p)

ifeq ($(UNAME), Linux)
	LIBS		=	-lGLEW -lGL -lglfw -lcimgui -ldl -lm -pthread 
	INCLUDES	+=	-I/usr/include
	LDFLAGS		=	-L/usr/lib -Llib -Wl,-rpath,lib # -O2 -fsanitize=address
else
	ifeq ($(UNAME_P), i386)
		LIBS		=	
	else
		LIBS		=	-lglew -lglfw -lcimgui -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
		INCLUDES	+=	-I/opt/homebrew/include
		LDFLAGS		+=	-L/opt/homebrew/lib -L. # -O2 -fsanitize=address
	endif
endif



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
	$(Q)$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

%.o: %.c
	$(VECHO) "\033[34mCompiling object file:   \033[0m$@"
	$(Q)$(CC) -c $< $(CFLAGS) $(INCLUDES) -o $@

clean:
	$(VECHO) "\033[31mRemoving Particles object files\033[0m"
	$(VECHO)
	$(Q)rm -f $(OBJS) $(IMPLOBJS) $(TARGET)

re: clean all
	$(VECHO) "\033[31mRemoving object files\033[0m"
	$(VECHO)

.DEFAULT_GOAL := all
.PHONY: all clean re
