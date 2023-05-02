CXX			=	c++

SRCS		=	../cimgui/cimgui.cpp \
				../cimgui/imgui/imgui.cpp \
				../cimgui/imgui/imgui_draw.cpp \
				../cimgui/imgui/imgui_demo.cpp \
				../cimgui/imgui/imgui_tables.cpp \
				../cimgui/imgui/imgui_widgets.cpp \
				../cimgui/imgui/backends/imgui_impl_opengl3.cpp \
				../cimgui/imgui/backends/imgui_impl_glfw.cpp \

OBJS		=	$(SRCS:.cpp=.o)

INCLUDE		=	-I../cimgui -I../cimgui/imgui -I../cimgui/generator/output

CXXFLAGS	=	-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1 -DIMGUI_IMPL_API="extern \"C\" "
CXXFLAGS	+=	-std=c++11 -O2 -Wall -Wextra -Werror

UNAME_S :=	$(shell uname -s)

ifeq ($(UNAME_S), Linux)
	NAME	=	libcimgui.so
	INCLUDE	+=	-I/usr/include
	CXXFLAGS+=	-DIMGUI_IMPL_OPENGL_LOADER_GL3W -DCIMGUI_USE_WSDL -Dcimgui_EXPORTS -fPIC #-g -fsanitize=address
	LDFLAGS	=	-lGL -lglfw3 -lrt -lm -lX11 -lpthread
	LD_PREF	=	-fPIC -shared
endif

ifeq ($(UNAME_S), Darwin)
	NAME	=	libcimgui.dylib
	INCLUDE	+=	-I/opt/homebrew/include
	
	CXXFLAGS+=  -DIMGUI_IMPL_OPENGL_LOADER_GL3W -DCIMGUI_USE_WSDL -Dcimgui_EXPORTS
	LDFLAGS	=	-lglfw -dynamiclib -framework OpenGL -L/opt/homebrew/lib
endif

TARGET		= 	$(BIN_DIR)$(NAME)

ifeq ("$(VERBOSE)","1")
Q :=
VECHO = @echo
else
Q := @
VECHO = @echo
endif

all: $(NAME)
	$(VECHO)

.cpp.o:
	$(VECHO) "\033[34mCompiling object file:   \033[0m$@"
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $< 

$(NAME): $(OBJS)
	$(VECHO)
	$(VECHO) "\033[36mLinking cimgui dynlib file:     \033[0m$@ 🚨"
	$(VECHO)
	$(Q)$(CXX) $(LD_PREF) -o $@ $(LDFLAGS) $(OBJS)

clean:
	$(VECHO) "\033[31mRemoving cimgui object files\033[0m"
	$(VECHO)
	$(Q)rm $(OBJS)

fclean: clean
	$(VECHO) "\033[31mRemoving cimgui library\033[0m"
	$(VECHO)
	$(Q)rm $(TARGET)

re: clean all
