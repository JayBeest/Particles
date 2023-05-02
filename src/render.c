#include <GL/glew.h>
#include "particles.h"

static void drawRectangle(GLuint * shaderProgram, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, t_paColor color, GLfloat screenWidth, GLfloat screenHeight) {
	GLint posOffsetLocation = glGetUniformLocation(*shaderProgram, "posOffset");
	glUniform2f(posOffsetLocation, posX, posY);

	GLint sizeLocation = glGetUniformLocation(*shaderProgram, "size");
	glUniform2f(sizeLocation, width, height);

	GLint screenSizeLocation = glGetUniformLocation(*shaderProgram, "screenSize");
	glUniform2f(screenSizeLocation, screenWidth, screenHeight);

	GLint colorLocation = glGetUniformLocation(*shaderProgram, "color");
	glUniform4f(colorLocation, color.r, color.g, color.b, color.a);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void render_particles(GLuint shaderProgram, t_paSettings *settings)
{
	for (int i = 0; i < settings->num_particles; i++)
	{
		drawRectangle(&shaderProgram, settings->particles[i].x, \
			settings->particles[i].y, settings->p_size, settings->p_size, \
			settings->particles[i].color, settings->window_width, settings->window_height);
	}
}