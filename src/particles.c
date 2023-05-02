#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"

#include "particles.h"
#include "init.h"
#include "render.h"
#include "update.h"
#include "gui.h"

const GLchar* vertexSource =
		"#version 150 core\n"
		"in vec2 position;"
		"uniform vec2 posOffset;"
		"uniform vec2 size;"
		"uniform vec2 screenSize;"
		"void main() {"
		"   vec2 normSize = size / screenSize * 2.0;"
		"   vec2 normOffset = posOffset / screenSize * 2.0 - vec2(1.0, 1.0);"
		"   gl_Position = vec4(position * normSize + normOffset, 0.0, 1.0);"
		"}";

const GLchar* fragmentSource =
		"#version 150 core\n"
		"out vec4 outColor;"
		"uniform vec4 color;"
		"void main() {"
		"   outColor = color;"
		"}";

t_paSettings	*g_settings;
t_glfw_cimgui	*g_glfw_cimgui;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		if (g_settings->mouse_toggle == true)
			g_settings->mouse_toggle = false;
		else
			g_settings->mouse_toggle = true;
	}
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	int og_w = g_settings->window_width;
	int og_h = g_settings->window_width;

	g_settings->window_width = width;
	g_settings->window_height = height;
	if (og_w > width || og_h > height)
		init_particles(g_settings);
}

void checkShaderCompilation(GLuint shader) {
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
	}
}

void checkProgramLinking(GLuint program) {
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
	}
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	srand(time(NULL));

	t_paSettings	settings;
	t_glfw_cimgui	glfw_cimgui;

	bzero(&settings, sizeof(t_paSettings));
	bzero(&glfw_cimgui, sizeof(t_glfw_cimgui));
	init_settings(&settings);
	init_glfw(&settings, &glfw_cimgui);
	init_cimgui(&glfw_cimgui);
	init_glew();
	init_particles(&settings);
	g_settings = &settings;
	g_glfw_cimgui = &glfw_cimgui;

	// Compile shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	checkShaderCompilation(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	checkShaderCompilation(fragmentShader);

	// Create shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkProgramLinking(shaderProgram);
	glUseProgram(shaderProgram);

	// Create VBO
	GLfloat vertices[] = {
			-0.5f,  0.5f,
			0.5f,  0.5f,
			0.5f, -0.5f,
			-0.5f, -0.5f,
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create VAO
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Specify layout
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Create EBO
	GLuint elements[] = {
			0, 1, 2,
			2, 3, 0,
	};

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Main loop
	while (!glfwWindowShouldClose(glfw_cimgui.window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		update_particles(&glfw_cimgui, &settings);
		render_particles(shaderProgram, &settings);

		start_imgui_frame(&settings, &glfw_cimgui);

		glfwSwapBuffers(glfw_cimgui.window);
		glfwPollEvents();
	}

	// Clean up
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(NULL);

	glfwDestroyWindow(glfw_cimgui.window);
	glfwTerminate();
	free(settings.particles);
	return 0;
}
