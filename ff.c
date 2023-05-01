void update_particles(Particle particles[], int num_particles, double dt, GLFWwindow *window)
{
	for (int i = 0; i < num_particles; i++) {
		particles[i].x += (particles[i].vx * dt);
		particles[i].y += (particles[i].vy * dt);

		if (particles[i].x < 0 || particles[i].x >= WINDOW_WIDTH ) {
			particles[i].vx = -particles[i].vx * 0.9f;
			particles[i].vy = particles[i].vy * 0.9f;
		}
		if (particles[i].y < 0 || particles[i].y >= WINDOW_HEIGHT ) {
			particles[i].vy = -particles[i].vy * 0.9f;
			particles[i].vx = particles[i].vx * 0.9f;
		}
		particles[i].color.g = (sqrtf(particles[i].vy * particles[i].vy + particles[i].vx * particles[i].vx) / 150);
	}
}
	
int main(int argc, char **argv)
{ ....
while (!glfwWindowShouldClose(window))
	{
		int width, height;

		glfwPollEvents();

		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width/2, 0, height/2, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		update_particles(particles, NUM_PARTICLES, 1.0f / 60.0f, window);
		render_particles(particles, NUM_PARTICLES, 2);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void render_particles(Particle particles[], int num_particles, int size)
{
	for (int i = 0; i < num_particles; i++)
	{
		glColor4f(particles[i].color.r, particles[i].color.g, particles[i].color.b, particles[i].color.a);
		if (particles[i].x >= 0 && particles[i].x < WINDOW_WIDTH && particles[i].y >= 0 && particles[i].y < WINDOW_HEIGHT)
			glRectf(particles[i].x, particles[i].y, particles[i].x + size, particles[i].y + size);
		// drawRectangle(particles[i].x, particles[i].y, size, size, particles[i].color);
	}
}