NAME		=	PartICLES

SRC_DIR		=	src/
OBJ_DIR		=	obj/
HEADER_DIR	=	include/

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
INCLUDE		=	-Iinclude -Icimgui -Icimgui/imgui -Icimgui/generator/output/

LDFLAGS		=	-DIMGUI_IMPL_API="extern \"C\"" -O2 #-fsanitize=address


UNAME		=	$(shell uname)
UNAME_P		=	$(shell uname -p)

ifeq ($(UNAME), Linux)
	LIBS		=	-lGLEW -lGL -lglfw -lcimgui -ldl -lm -pthread 
	INCLUDE		+=	-I/usr/include
	LDFLAGS		+=	-L/usr/lib -Llib #-Wl,-rpath,lib
else
	ifeq ($(UNAME_P), i386)
	LIBS		= # codam
	else
	LIBS		=	-lglew -lglfw -lcimgui -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	INCLUDE		+=	-I/opt/homebrew/include -Iinclude
	LDFLAGS		+=	-L/opt/homebrew/lib -Llib #-Wl,-rpath,@executable_path/lib
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


all: $(NAME)

$(NAME): $(OBJ)
	$(Q)mkdir -p $(@D)
	$(VECHO)
	$(VECHO) "\033[35mBuilding CuimGui lib\033[0m"
	$(Q)make -C lib
	$(VECHO) "\033[36mLinking binary file:     \033[0m$@ 🚨"
	$(VECHO)
	$(Q)$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(HEADERS)
	$(Q)mkdir -p $(@D)
	$(VECHO) "\033[34mCompiling object file:   \033[0m$@"
	$(Q)$(CC) -c $< $(CFLAGS) $(INCLUDE) -o $@

clean:
	$(VECHO) "\033[31mRemoving Particles object files\033[0m"
	$(VECHO)
	$(Q)rm -rf $(OBJ_DIR)

fclean: clean
	$(VECHO) "\033[31mRemoving binary\033[0m"
	$(VECHO)
	$(Q)rm -f $(NAME)

re: clean all

.DEFAULT_GOAL := all
.PHONY: all clean re

