NAME		=	PartICLES

SRC_DIR		=	src/
OBJ_DIR		=	obj/
BIN_DIR		=	bin/
HEADER_DIR	=	include/
BIN			=	$(BIN_DIR)$(NAME)

SRC		=		particles.c \
				init.c \
				render.c \
				forces.c \
				update.c \
				gui.c \
				colors.c \

HEADERS		=	$(addprefix $(HEADER_DIR), $(SRC:%.c=%.h))

OBJ 		=	$(addprefix $(OBJ_DIR), $(SRC:%.c=%.o))

CFLAGS		=	-DGL_SILENCE_DEPRECATION -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW -O2 #-fsanitize=address
INCLUDES	=	-Icimgui -Icimgui/imgui -Icimgui/generator/output/

LDFLAGS		=	-DIMGUI_IMPL_API="extern \"C\""


UNAME		=	$(shell uname)
UNAME_P		=	$(shell uname -p)

ifeq ($(UNAME), Linux)
	LIBS		=	-lGLEW -lGL -lglfw -lcimgui -ldl -lm -pthread 
	INCLUDES	+=	-I/usr/include
	LDFLAGS		=	-L/usr/lib -Llib -Wl,-rpath,../lib # -O2 -fsanitize=address
else
	ifeq ($(UNAME_P), i386)
	LIBS		= # codam
	else
	LIBS		=	-lglew -lglfw -lcimgui -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	INCLUDES	+=	-I/opt/homebrew/include -Iinclude
	LDFLAGS		+=	-L/opt/homebrew/lib -Llib # -O2 -fsanitize=address
	endif
endif


ifeq ("$(VERBOSE)","1")
Q :=
VECHO = @echo
else
Q := @
VECHO = @echo
endif

test:
	echo $(HEADERS)
	echo $(OBJ)


all: $(BIN)

$(BIN): $(OBJ)
	echo $(OBJ)
	$(VECHO)
	$(VECHO) "\033[35mBuilding CuimGui lib\033[0m"
	$(Q)make -C lib
	$(VECHO)
	$(VECHO)
	$(VECHO) "\033[36mLinking binary file:     \033[0m$@ 🚨"
	$(VECHO)
	$(Q)$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(HEADERS)
	$(Q)mkdir -p $(@D)
	$(VECHO) "\033[34mCompiling object file:   \033[0m$@"
	$(Q)$(CC) -c $< $(CFLAGS) $(INCLUDES) -o $@

clean:
	$(VECHO) "\033[31mRemoving Particles object files\033[0m"
	$(VECHO)
	$(Q)rm -rf obj
	$(Q)rm $(TARGET)

re: clean all
	$(VECHO) "\033[31mRemoving object files\033[0m"
	$(VECHO)

.DEFAULT_GOAL := all
.PHONY: all clean re

