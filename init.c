# define CIMGUI_DEFINE_ENUMS_AND_STRUCTS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cimgui.h"
#include "cimgui_impl.h"

#include "particles.h"

void	init_settings(t_paSettings *settings)
{
	settings->baseColor = (ImVec4){0.0f, 0.345f, 0.529f, 1.0f};

	settings->mouse_toggle = true;
	settings->grav_toggle = true;

	settings->showDemoWindow = true;
	settings->showAnotherWindow = false;

	settings->cur_max_particles = 3500;
	settings->num_particles = 512;
	settings->p_size = 2;

	settings->air_resistance = 0.99975f;
	settings->grav_constant = 32;
	settings->force_multiplier = 128;
	settings->centri_multiplier = 128;
	settings->wall_bounce = 0.9f;

	settings->window_width = 512;
	settings->window_height = 512;
}

void init_particles(t_paSettings *settings)
{
	if (!settings->particles)
	{
		settings->particles = calloc(MAX_PARTICLES, sizeof(t_particle));
		if (!settings->particles)
			exit(EXIT_FAILURE);
	}
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		(settings->particles)[i].x = rand() % settings->window_width;
		(settings->particles)[i].y = rand() % settings->window_height;
		(settings->particles)[i].vx = (rand() % 200) / 100.0f;
		(settings->particles)[i].vy = (rand() % 200) / 100.0f;
		(settings->particles)[i].mass = (rand() % 200) / 10.0f + 3.0f;
		(settings->particles)[i].color = (t_paColor){(4 / settings->particles[i].mass) + settings->baseColor.x, settings->baseColor.y, settings->baseColor.z, settings->baseColor.w};

	}
}

void	init_glfw(t_paSettings *settings, t_glfw_cimgui * glfw_cimgui)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}
	// Create a windowed mode window and its OpenGL context
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	glfw_cimgui->window = glfwCreateWindow(settings->window_width, settings->window_height, "Particles", NULL, NULL);
	if (!glfw_cimgui->window) {
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Make the window's context current
	glfwMakeContextCurrent(glfw_cimgui->window);
	glfwSwapInterval(1);
//	glfwSetWindowSizeCallback(glfw_cimgui->window, window_size_callback); ////<<<<----!!!
	glfwSetInputMode(glfw_cimgui->window, GLFW_STICKY_KEYS, GLFW_TRUE);


//	glfwSetKeyCallback(glfw_cimgui->window, key_callback); <<<=====!!!

}

void	init_cimgui(t_glfw_cimgui * glfw_cimgui)
{
	// setup imgui
	igCreateContext(NULL);

	// set docking
	glfw_cimgui->ioptr = igGetIO();
	glfw_cimgui->ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	//ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	#ifdef IMGUI_HAS_DOCK
		glfw_cimgui->ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
		glfw_cimgui->ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
	#endif

	ImGui_ImplGlfw_InitForOpenGL(glfw_cimgui->window, true);

	#if __APPLE__
		// GL 3.2 Core + GLSL 150
		const char *glsl_version = "#version 330";
	#else
		// GL 3.2 + GLSL 130
			const char *glsl_version = "#version 130";
	#endif

	ImGui_ImplOpenGL3_Init(glsl_version);

	igStyleColorsDark(NULL);
	// ImFontAtlas_AddFontDefault(io.Fonts, NULL);
}

void	init_glew(void)
{
	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(EXIT_FAILURE);
	}
}