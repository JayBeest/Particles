#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "cimgui.h"
#include "cimgui_impl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "particles.h"
#include "init.h"
#include "shaders.h"


void drawRectangle(GLuint * shaderProgram, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, t_paColor color, GLfloat screenWidth, GLfloat screenHeight) {
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


void render_particles(t_shaders * shaders, t_paSettings *settings)
{
	for (int i = 0; i < settings->num_particles; i++)
	{
		drawRectangle(&shaders->shaderProgram, settings->particles[i].x, \
			settings->particles[i].y, settings->p_size, settings->p_size, \
			settings->particles[i].color, settings->window_width, settings->window_height);
	}
}

void calculate_centripetal_force(t_particle *p, float centri_multip, float center_x, float center_y, float *fx, float *fy)
{
	float dx = p->x - center_x;
	float dy = p->y - center_y;
	float distance_squared = dx * dx + dy * dy;
	float distance = sqrtf(distance_squared);

	// if (distance < 1.0f) {
	//     distance = 1.0f;
	// }

	float force = centri_multip * p->mass / (distance_squared * 0.99f);
	*fx = -force * (dx / distance);
	*fy = -force * (dy / distance);
}

void calculate_tangential_force(t_particle *p, float force_multip, float center_x, float center_y, float *fx, float *fy)
{
	float dx = p->x - center_x;
	float dy = p->y - center_y;
	float distance = sqrtf(dx * dx + dy * dy);

	// if (distance < 1.0f) {
	//     distance = 1.0f;
	// }

	float force = force_multip * (p->mass / distance);
	*fx = -force * (dy / distance);
	*fy = force * (dx / distance);
}

void apply_damping_force(t_particle *p, float damping)
{
    p->vx *= damping;
    p->vy *= damping;
}

void calculate_gravitational_force(t_particle *p1, t_particle *p2, float *fx, float *fy, float grav_const)
{
	float dx = p2->x - p1->x;
	float dy = p2->y - p1->y;
	float distance_squared = dx * dx + dy * dy;
	float distance = sqrtf(distance_squared);

	if (distance < 1.0f) {
		distance = 1.0f;
	}

	float force = grav_const * (p1->mass * p2->mass) / distance_squared;
	*fx = force * (dx / distance);
	*fy = force * (dy / distance);
}

//void	update_colors(void)
//{
//	for (int i = 0; i < MAX_PARTICLES; i++)
//	{
//		(particles)[i].color = (paColor){(4 / (particles)[i].mass) + baseColor.x, baseColor.y, baseColor.z, baseColor.w};
//	}
//}



void update_particles(t_glfw_cimgui *glfw_cimgui, t_paSettings *settings, double deltaTime)
{
	static int center_x;
	static int center_y;
	double mouse_x, mouse_y;

	glfwGetCursorPos(glfw_cimgui->window, &mouse_x, &mouse_y);

	if (settings->mouse_toggle)
	{
		 center_x = mouse_x;
		 center_y = settings->window_height - mouse_y;
	}

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
		static int count = 0;
		if (settings->grav_toggle)
		{
			for (int i = 0; i < settings->num_particles; i++)
			{
				count++;
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

void	toggle(bool *toggle_switch)
{
	if (*toggle_switch == true)
		*toggle_switch = false;
	else
		*toggle_switch = true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
//		if (*toggle_switch == true)
//			*toggle_switch = false;
//		else
//			*toggle_switch = true;
	}
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	// int og_w = window_width;
	// int og_h = window_height;

	// window_width = width;
	// window_height = height;
	// if (og_w > width || og_h > height)
	// 	init_particles();
}

void ffdrawRectangle(GLuint shaderProgram, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat screenWidth, GLfloat screenHeight) {
	GLint posOffsetLocation = glGetUniformLocation(shaderProgram, "posOffset");
	glUniform2f(posOffsetLocation, posX, posY);

	GLint sizeLocation = glGetUniformLocation(shaderProgram, "size");
	glUniform2f(sizeLocation, width, height);

	GLint screenSizeLocation = glGetUniformLocation(shaderProgram, "screenSize");
	glUniform2f(screenSizeLocation, screenWidth, screenHeight);

	GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
	glUniform4f(colorLocation, r, g, b, a);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	srand(time(NULL));

	t_paSettings	settings;
	t_glfw_cimgui	glfw_cimgui;
	t_shaders		shaders;

	bzero(&settings, sizeof(t_paSettings));
	bzero(&glfw_cimgui, sizeof(t_glfw_cimgui));
	bzero(&shaders, sizeof(t_shaders));
	init_settings(&settings);
	init_glfw(&settings, &glfw_cimgui);
	init_cimgui(&glfw_cimgui);
	init_glew();
//	init_shaders(&shaders);


//////////////begin shaders

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



/////////////end shaders

	init_particles(&settings);


    // Main loop

	double lastFrameTime = glfwGetTime();


	while (!glfwWindowShouldClose(glfw_cimgui.window))
	{
		// Clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int width, height;
		glfwGetWindowSize(glfw_cimgui.window, &width, &height);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastFrameTime; 
		lastFrameTime = currentTime;

		update_particles(&glfw_cimgui, &settings, deltaTime);
		render_particles(&shaders, &settings);

		ffdrawRectangle(shaders.shaderProgram, 100.0f, 100.0f, 200.0f, 200.0f, 1.0f, 0.0f, 0.0f, 1.0f, (GLfloat)width, (GLfloat)height);
		ffdrawRectangle(shaders.shaderProgram, 400.0f, 200.0f, 100.0f, 300.0f, 0.0f, 1.0f, 0.0f, 1.0f, (GLfloat)width, (GLfloat)height);

		// start imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		igNewFrame();

		if (settings.showDemoWindow)
			igShowDemoWindow(&settings.showDemoWindow);

		// show a simple window that we created ourselves.
		{
			static float f = 0.0f;
			// static int counter = 0;

			igBegin("Controls", NULL, 0);
			// igText("This is some useful text");
			// igCheckbox("Demo window", &showDemoWindow);
			// igCheckbox("Another window", &showAnotherWindow);
			igCheckbox("GravToggle", &settings.grav_toggle);

			igSliderInt("TangiForce", &settings.force_multiplier, -7500, 7500, "%.d", 0);
			igSliderInt("CentriForce", &settings.centri_multiplier, -250000, 250000, "%.d", 0);
			igSliderInt("GravForce", &settings.grav_constant, 1, 1000, "%.d", 0);
			igSliderFloat("WallBounce", &settings.wall_bounce, 0.0f, 2.0f, "%.2f", 0);
			igSliderFloat("AirRez", &settings.air_resistance, 1.0f, 0.5f, "%.5f", 0);
			igSliderFloat("P-Size", &settings.p_size, 0.5f, 50.0f, "%.5f", 0);
			igSliderInt("P-Amount", &settings.num_particles, 1, settings.cur_max_particles, "%.d", 0);
			igColorEdit4("clear color", (float *)&settings.baseColor, 0);

			static t_paColor ff;
				igPushID_Int(1);
				igVSliderFloat("##v", (ImVec2){18, 120}, &(settings.baseColor.x), 0, 1, "", 0);
				igPopID();
				igSameLine(0.0f, -1.0f);
				igPushID_Int(2);
				igVSliderFloat("##v", (ImVec2){18, 120}, &(settings.baseColor.y), 0, 1, "", 0);
				igPopID();
				igSameLine(0.0f, -1.0f);
				igPushID_Int(3);
				igVSliderFloat("##v", (ImVec2){18, 120}, &(settings.baseColor.z), 0, 1, "", 0);
				igPopID();
				igSameLine(0.0f, -1.0f);
			
			ImVec2 buttonSize;
			buttonSize.x = 0;
			buttonSize.y = 0;
			if (igButton("Reset particles", buttonSize))
				init_particles(&settings);
			// igSameLine(0.0f, -1.0f);
			// igText("counter = %d", counter);

			// igText("Application %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
			igEnd();
		}

		static ImVec4 old_color; 
		if (old_color.x == 0)
		{
			old_color.x = settings.baseColor.x;
			old_color.y = settings.baseColor.y;
			old_color.z = settings.baseColor.z;
		}
		if (settings.baseColor.x != old_color.x || old_color.y != old_color.y || old_color.z != old_color.z)
		{
			old_color = (ImVec4)settings.baseColor;
//			update_colors();
		}


		if (settings.grav_toggle)
		{
			settings.cur_max_particles = GRAV_MAX_PARTICLES;
			if (settings.num_particles > GRAV_MAX_PARTICLES)
				settings.num_particles = GRAV_MAX_PARTICLES;
		}
		else
		{
			settings.cur_max_particles = MAX_PARTICLES;
		}

		if (settings.showAnotherWindow)
		{
			igBegin("imgui Another Window", &settings.showAnotherWindow, 0);
			igText("Hello from imgui");
			ImVec2 buttonSize;
			buttonSize.x = 0;
			buttonSize.y = 0;
			if (igButton("Close me", buttonSize))
			{
				settings.showAnotherWindow = false;
			}
		igEnd();
		}
		char title[128];
		snprintf(title, 100, "Particles  -  %-3.fms/frame (%-3.f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
		glfwSetWindowTitle(glfw_cimgui.window, title);

		// render
		
		igRender();
		glfwMakeContextCurrent(glfw_cimgui.window);
		// glViewport(0, 0, (int)ioptr->DisplaySize.x, (int)ioptr->DisplaySize.y);
		// glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		// glClear(GL_COLOR_BUFFER_BIT);

		
		ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

		#ifdef IMGUI_HAS_DOCK
		if (glfw_cimgui.ioptr->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow *backup_current_window = glfwGetCurrentContext();
			igUpdatePlatformWindows();
			igRenderPlatformWindowsDefault(NULL, NULL);
			glfwMakeContextCurrent(backup_current_window);
		}
		#endif


        // Swap buffers and poll for events
        glfwSwapBuffers(glfw_cimgui.window);
        glfwPollEvents();
    }

    // Clean up
    glDeleteBuffers(1, &shaders.ebo);
    glDeleteBuffers(1, &shaders.vbo);
    glDeleteVertexArrays(1, &shaders.vao);

    glDeleteShader(shaders.vertexShader);
    glDeleteShader(shaders.fragmentShader);
    glDeleteProgram(shaders.shaderProgram);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(NULL);

	glfwDestroyWindow(glfw_cimgui.window);
    glfwTerminate();
	free(settings.particles);
    return 0;
}
