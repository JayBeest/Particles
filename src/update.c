#include <GLFW/glfw3.h>
#include <math.h>
#include "particles.h"
#include "forces.h"

void	update_mouse(GLFWwindow *window, t_paSettings *settings, int *x, int *y)
{
	double	mouse_x;
	double	mouse_y;

	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	if (settings->mouse_toggle)
	{
		*x = (int)mouse_x;
		*y = (int)(settings->window_height - mouse_y);
	}
}

void	update_swirl(t_paSettings *settings, double deltaTime, int center_x, int center_y)
{
	for (int i = 0; i < settings->num_particles; i++)
	{
		float fx, fy;
		calculate_centripetal_force(&settings->particles[i], settings->centri_multiplier, center_x, center_y, &fx, &fy);
		settings->particles[i].vx += fx * deltaTime;

		settings->particles[i].vy += fy * deltaTime;
		calculate_tangential_force(&settings->particles[i], settings->force_multiplier, center_x, center_y, &fx, &fy);
		apply_damping_force(&settings->particles[i], settings->air_resistance);
		settings->particles[i].vx += fx * deltaTime;
		settings->particles[i].vy += fy * deltaTime;
	}
}

void	update_gravity(t_paSettings *settings, double deltaTime)
{
	for (int i = 0; i < settings->num_particles; i++)
	{
		for (int j = i + 1; j < settings->num_particles; j++)
		{
			float fx, fy;
			calculate_gravitational_force(&settings->particles[i], &settings->particles[j], &fx, &fy, settings->grav_constant);
			settings->particles[i].vx += fx / settings->particles[i].mass * deltaTime;
			settings->particles[i].vy += fy / settings->particles[i].mass * deltaTime;
			settings->particles[j].vx -= fx / settings->particles[j].mass * deltaTime;
			settings->particles[j].vy -= fy / settings->particles[j].mass * deltaTime;
			apply_damping_force(&settings->particles[i], settings->air_resistance);
		}
	}
}

void	update_motion_color(t_paSettings *settings, double deltaTime)
{
	for (int i = 0; i < settings->num_particles; i++)
	{
		float x_inc = settings->particles[i].vx * deltaTime;
		float y_inc = settings->particles[i].vy * deltaTime;
		if (x_inc < settings->window_width)
			settings->particles[i].x += x_inc;
		if (y_inc < settings->window_height)
			settings->particles[i].y += y_inc;

		if (settings->particles[i].x < 0 || settings->particles[i].x >= settings->window_width ) {
			settings->particles[i].vx = -settings->particles[i].vx * settings->wall_bounce;
			settings->particles[i].vy = settings->particles[i].vy * settings->wall_bounce;
		}
		if (settings->particles[i].y < 0 || settings->particles[i].y >= settings->window_height ) {
			settings->particles[i].vy = -settings->particles[i].vy * 0.9f;
			settings->particles[i].vx = settings->particles[i].vx * 0.9f;
		}
		settings->particles[i].color.g = (sqrtf(settings->particles[i].vy * settings->particles[i].vy + settings->particles[i].vx * settings->particles[i].vx) / 150);
	}

}

void	update_particles(t_glfw_cimgui *glfw_cimgui, t_paSettings *settings)
{
	static int center_x;
	static int center_y;
	static double lastFrameTime;
	double currentTime;
	double deltaTime;

	currentTime = glfwGetTime();
	deltaTime = currentTime - lastFrameTime;
	lastFrameTime = currentTime;
	update_mouse(glfw_cimgui->window, settings, &center_x, &center_y);
	update_swirl(settings, deltaTime, center_x, center_y);
	if (settings->grav_toggle)
		update_gravity(settings, deltaTime);
	update_motion_color(settings, deltaTime);
}