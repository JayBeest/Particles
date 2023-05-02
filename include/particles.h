#ifndef PARTICLES_H
# define PARTICLES_H


# define MAX_PARTICLES 50000
# define GRAV_MAX_PARTICLES 3500

# include <GLFW/glfw3.h>
# include <stdbool.h>

# define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
# include <cimgui.h>

typedef struct s_paColor
{
	float	r;
	float	g;
	float	b;
	float	a;
}			t_paColor;

typedef struct s_particle
{
	float	x;
	float	y;
	float	vx;
	float 	vy;
	float	mass;
	t_paColor color;
}			t_particle;

typedef struct s_paSettings
{
	t_particle	*particles;

	bool		mouse_toggle;
	bool		grav_toggle;

	bool		showDemoWindow;
	bool		showAnotherWindow;

	ImVec4 		baseColor;

	int			cur_max_particles;
	int			num_particles;
	float		p_size;

	float		air_resistance;
	int			grav_constant;
	int			force_multiplier;
	int			centri_multiplier;
	float		wall_bounce;

	int			window_width;
	int			window_height;

	int			mouse_x;
	int			mouse_y;
}			t_paSettings;

typedef struct s_glfw_cimgui
{
	GLFWwindow	*window;
	ImGuiIO		*ioptr;

}			t_glfw_cimgui;

//typedef struct s_shaders
//{
//	GLuint	vertexShader;
//	GLuint	fragmentShader;
//	GLuint	shaderProgram;
//
////	GLfloat	vertices[8];
//	GLuint	vbo;
//	GLuint	vao;
////	GLint	posAttrib;
////	GLuint	elements[6];
//	GLuint	ebo;
//
//}			t_shaders;

void	window_size_callback(GLFWwindow* window, int width, int height);
void	key_callback(GLFWwindow* window, int key, int scancode, \
		int action, int mods);


#endif
