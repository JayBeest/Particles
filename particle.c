#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <GLFW/glfw3.h>
#include "cimgui.h"
#include "cimgui_impl.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <GL/gl.h>

#define NUM_PARTICLES 1000
#define AIR_RESISTANCE 0.99975
int GRAVITATIONAL_CONSTANT = 20;
int F_MULTIP = 100;
int CENTRI_MULTI = 10;

int WINDOW_WIDTH = 700;
int WINDOW_HEIGHT = 700;

typedef struct {
	float r;
	float g;
	float b;
	float a;
} paColor;


typedef struct {
	float x, y;
	float vx, vy;
	float mass;
	paColor color;
	
} Particle;

Particle *particles = NULL;

void drawRectangle(float x, float y, float width, float height, paColor color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

void render_particles(Particle particles[], int num_particles, int size)
{
	for (int i = 0; i < num_particles; i++)
	{
		glColor4f(particles[i].color.r, particles[i].color.g, particles[i].color.b, particles[i].color.a);
		glRectf(particles[i].x, particles[i].y, particles[i].x + size, particles[i].y + size);
		// drawRectangle(particles[i].x, particles[i].y, size, size, particles[i].color);
	}
}

void calculate_centripetal_force(Particle *p, float center_x, float center_y, float *fx, float *fy)
{
	float dx = p->x - center_x;
	float dy = p->y - center_y;
	float distance_squared = dx * dx + dy * dy;
	float distance = sqrtf(distance_squared);

	// if (distance < 1.0f) {
	//     distance = 1.0f;
	// }

	float force = CENTRI_MULTI * p->mass / (distance_squared * 0.99f);
	*fx = -force * (dx / distance);
	*fy = -force * (dy / distance);
}

void calculate_tangential_force(Particle *p, float center_x, float center_y, float *fx, float *fy)
{
	float dx = p->x - center_x;
	float dy = p->y - center_y;
	float distance = sqrtf(dx * dx + dy * dy);

	// if (distance < 1.0f) {
	//     distance = 1.0f;
	// }

	float force = F_MULTIP * (p->mass / distance);
	*fx = -force * (dy / distance);
	*fy = force * (dx / distance);
}

void apply_damping_force(Particle *p, float damping)
{
    p->vx *= damping;
    p->vy *= damping;
}

void calculate_gravitational_force(Particle *p1, Particle *p2, float *fx, float *fy)
{
	float dx = p2->x - p1->x;
	float dy = p2->y - p1->y;
	float distance_squared = dx * dx + dy * dy;
	float distance = sqrtf(distance_squared);

	if (distance < 1.0f) {
		distance = 1.0f;
	}

	float force = GRAVITATIONAL_CONSTANT * (p1->mass * p2->mass) / distance_squared;
	*fx = force * (dx / distance);
	*fy = force * (dy / distance);
}

void init_particles(Particle **particles)
{
	if (!*particles)
		*particles = calloc(NUM_PARTICLES, sizeof(Particle));
	for (int i = 0; i < NUM_PARTICLES; i++) {
		(*particles)[i].x = rand() % WINDOW_WIDTH;
		(*particles)[i].y = rand() % WINDOW_HEIGHT;
		(*particles)[i].vx = (rand() % 200) / 100.0f - 1.0f;
		(*particles)[i].vy = (rand() % 200) / 100.0f - 1.0f;
		// *particles[i].color = (SDL_Color){rand() % 256, rand() % 256, rand() % 256, 255};
		// *particles[i].vx = 0.0001;//(rand() % 200) / 100.0f - 1.0f;
		// *particles[i].vy = 0.0001;//(rand() % 200) / 100.0f - 1.0f;
		(*particles)[i].mass = (rand() % 200) / 10.0f + 3.0f;
		(*particles)[i].color = (paColor){(4 / (*particles)[i].mass), 0.345f, 0.529f, 1.0f};
	}
}

void update_particles(Particle particles[], int num_particles, float dt, GLFWwindow *window)
{
	
// float center_x = WINDOW_WIDTH / 2.0f;
// float center_y = WINDOW_HEIGHT / 2.0f;
double mouse_x, mouse_y;
glfwGetCursorPos(window, &mouse_x, &mouse_y);
float center_x = mouse_x;
float center_y = WINDOW_HEIGHT - mouse_y;
for (int i = 0; i < num_particles; i++)
{
	float fx, fy;

	calculate_centripetal_force(&particles[i], center_x, center_y, &fx, &fy);
	particles[i].vx += fx * dt;
	particles[i].vy += fy * dt;

	calculate_tangential_force(&particles[i], center_x, center_y, &fx, &fy);
	particles[i].vx += fx * dt;
	particles[i].vy += fy * dt;
}
	for (int i = 0; i < num_particles; i++) {
	for (int j = i + 1; j < num_particles; j++)
	{
		float fx, fy;
		calculate_gravitational_force(&particles[i], &particles[j], &fx, &fy);
		particles[i].vx += fx / particles[i].mass * dt;
		particles[i].vy += fy / particles[i].mass * dt;
		particles[j].vx -= fx / particles[j].mass * dt;
		particles[j].vy -= fy / particles[j].mass * dt;
		apply_damping_force(&particles[i], AIR_RESISTANCE);
    }
}
	for (int i = 0; i < num_particles; i++)
	{
		float x_inc = particles[i].vx * dt;
		float y_inc = particles[i].vy * dt;
		if (x_inc < WINDOW_WIDTH)
			particles[i].x += x_inc;
		if (y_inc < WINDOW_HEIGHT)
			particles[i].y += y_inc;

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

void window_size_callback(GLFWwindow* window, int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	srand(time(NULL));
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	#if __APPLE__
	// GL 3.2 Core + GLSL 150
	const char *glsl_version = "#version 150";
	#else
	// GL 3.2 + GLSL 130
	const char *glsl_version = "#version 130";
	#endif

	// just an extra window hint for resize
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Particles", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	// glfwSetWindowSizeLimits(window, 700, 700, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// setup imgui
	igCreateContext(NULL);

	// set docking
	ImGuiIO *ioptr = igGetIO();
	ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	//ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
#ifdef IMGUI_HAS_DOCK
	ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
	ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
#endif

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL2_Init();

	igStyleColorsDark(NULL);
	// ImFontAtlas_AddFontDefault(io.Fonts, NULL);

	bool showDemoWindow = false;
	bool showAnotherWindow = false;
	ImVec4 clearColor;
	clearColor.x = 0.45f;
	clearColor.y = 0.55f;
	clearColor.z = 0.60f;
	clearColor.w = 1.00f;

	init_particles(&particles);

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

		glfwSetWindowSizeCallback(window, window_size_callback);

		update_particles(particles, NUM_PARTICLES, 1.0f / 60.0f, window);
		render_particles(particles, NUM_PARTICLES, 2);
		

		// start imgui frame
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		igNewFrame();

		if (showDemoWindow)
			igShowDemoWindow(&showDemoWindow);

		// show a simple window that we created ourselves.
		{
			static float f = 0.0f;
			// static int counter = 0;

			igBegin("Controls", NULL, 0);
			// igText("This is some useful text");
			// igCheckbox("Demo window", &showDemoWindow);
			// igCheckbox("Another window", &showAnotherWindow);

			igSliderInt("TangiForce", &F_MULTIP, 1, 10000, "%.d", 0);
			igSliderInt("CentriForce", &CENTRI_MULTI, 1, 100000, "%.d", 0);
			igSliderInt("GravForce", &GRAVITATIONAL_CONSTANT, 1.0f, 1000, "%.d", 0);
			// igColorEdit3("clear color", (float *)&clearColor, 0);

			ImVec2 buttonSize;
			buttonSize.x = 0;
			buttonSize.y = 0;
			if (igButton("Reset particles", buttonSize))
				init_particles(&particles);
			// igSameLine(0.0f, -1.0f);
			// igText("counter = %d", counter);

			igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
			igEnd();
		}

		if (showAnotherWindow) 
		{
			igBegin("imgui Another Window", &showAnotherWindow, 0);
			igText("Hello from imgui");
			ImVec2 buttonSize;
			buttonSize.x = 0;
			buttonSize.y = 0;
			if (igButton("Close me", buttonSize))
			{
				showAnotherWindow = false;
			}
		igEnd();
		}

		// render
		
		igRender();
		glfwMakeContextCurrent(window);
		// glViewport(0, 0, (int)ioptr->DisplaySize.x, (int)ioptr->DisplaySize.y);
		// glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		// glClear(GL_COLOR_BUFFER_BIT);

		
		ImGui_ImplOpenGL2_RenderDrawData(igGetDrawData());
		#ifdef IMGUI_HAS_DOCK
		if (ioptr->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
			GLFWwindow *backup_current_window = glfwGetCurrentContext();
			igUpdatePlatformWindows();
			igRenderPlatformWindowsDefault(NULL, NULL);
			glfwMakeContextCurrent(backup_current_window);
		}
		#endif


		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(NULL);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
