#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "particles.h"

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


static void checkShaderCompilation(GLuint shader)
{
	GLint	success;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
	}
}

static void checkProgramLinking(GLuint program)
{
	GLint success;

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
	}
}

void	compile_shaders(t_shaders *shaders)
{
	shaders->vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(shaders->vertexShader, 1, &vertexSource, NULL);
	glCompileShader(shaders->vertexShader);
	checkShaderCompilation(shaders->vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	checkShaderCompilation(fragmentShader);
}

void	create_shader_program(t_shaders *shaders)
{
	shaders->shaderProgram = glCreateProgram();
	glAttachShader(shaders->shaderProgram, shaders->vertexShader);
	glAttachShader(shaders->shaderProgram, shaders->fragmentShader);
	glLinkProgram(shaders->shaderProgram);
	checkProgramLinking(shaders->shaderProgram);
	glUseProgram(shaders->shaderProgram);
}

void	create_vbo(t_shaders *shaders)
{
	// Create VBO
	GLfloat	vertices[] = {
			-0.5f, 0.5f,
			0.5f, 0.5f,
			0.5f, -0.5f,
			-0.5f, -0.5f,
	};
	glGenBuffers(1, &shaders->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, shaders->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create VAO
	glGenVertexArrays(1, &shaders->vao);
	glBindVertexArray(shaders->vao);

	// Specify layout
	GLint	posAttrib;
	posAttrib = glGetAttribLocation(shaders->shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Create EBO
	GLuint	elements[] = {
			0, 1,
			2, 2,
			3, 0,
	};
	glGenBuffers(1, &shaders->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,shaders->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

void	init_shaders(t_shaders *shaders)
{
	compile_shaders(shaders);
	create_shader_program(shaders);
	create_vbo(shaders);
}