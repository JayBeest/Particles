#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "cimgui.h"
#include "cimgui_impl.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define MAX_PARTICLES 50000
#define GRAV_MAX_PARTICLES 3500
int CUR_MAX_PARTICLES = 3500;
int NUM_PARTICLES = 512;
float P_SIZE = 2;

float AIR_RESISTANCE = 0.99975f;
int GRAVITATIONAL_CONSTANT = 32;
int F_MULTIP = 128;
int CENTRI_MULTI = 128;
float WALL_BOUNCE = 0.9f;

int WINDOW_WIDTH = 512;
int WINDOW_HEIGHT = 512;

bool mouse_toggle = true;
bool grav_toggle = true;

typedef struct {
	float r;
	float g;
	float b;
	float a;
} paColor;

ImVec4 baseColor = {0.0f, 0.345f, 0.529f, 1.0f};
// baseColor.x = 0.0f;
// baseColor.y = 0.345f;
// baseColor.z = 0.529f;
// baseColor.w = 1.0f;

typedef struct {
	float x, y;
	float vx, vy;
	float mass;
	paColor color;
	
} Particle;


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

void drawRectangle(GLuint shaderProgram, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, paColor color, GLfloat screenWidth, GLfloat screenHeight) {
    GLint posOffsetLocation = glGetUniformLocation(shaderProgram, "posOffset");
    glUniform2f(posOffsetLocation, posX, posY);

    GLint sizeLocation = glGetUniformLocation(shaderProgram, "size");
    glUniform2f(sizeLocation, width, height);

    GLint screenSizeLocation = glGetUniformLocation(shaderProgram, "screenSize");
    glUniform2f(screenSizeLocation, screenWidth, screenHeight);

    GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform4f(colorLocation, color.r, color.g, color.b, color.a);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// Particle *particles = NULL;
Particle *particles;

void render_particles(GLuint shaderProgram, Particle particles[], int num_particles, float size)
{
	for (int i = 0; i < num_particles; i++)
	{
	// 	glColor4f(particles[i].color.r, particles[i].color.g, particles[i].color.b, particles[i].color.a);
	// 	glRectf(particles[i].x, particles[i].y, particles[i].x + size, particles[i].y + size);
		drawRectangle(shaderProgram, particles[i].x, particles[i].y, size, size, particles[i].color, WINDOW_WIDTH, WINDOW_HEIGHT);
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

void	update_colors(void)
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		(particles)[i].color = (paColor){(4 / (particles)[i].mass) + baseColor.x, baseColor.y, baseColor.z, baseColor.w};
	}
}

void init_particles()
{
	if (!particles)
		particles = calloc(MAX_PARTICLES, sizeof(Particle));
	for (int i = 0; i < MAX_PARTICLES; i++) {
		(particles)[i].x = rand() % WINDOW_WIDTH;
		(particles)[i].y = rand() % WINDOW_HEIGHT;
		(particles)[i].vx = (rand() % 200) / 100.0f;
		(particles)[i].vy = (rand() % 200) / 100.0f;
		// *particles[i].color = (SDL_Color){rand() % 256, rand() % 256, rand() % 256, 255};
		// *particles[i].vx = 0.0001;//(rand() % 200) / 100.0f - 1.0f;
		// *particles[i].vy = 0.0001;//(rand() % 200) / 100.0f - 1.0f;
		(particles)[i].mass = (rand() % 200) / 10.0f + 3.0f;
		(particles)[i].color = (paColor){(4 / (particles)[i].mass) + baseColor.x, baseColor.y, baseColor.z, baseColor.w};

	}
}

void update_particles(Particle particles[], int num_particles, double deltaTime, GLFWwindow *window)
{
double mouse_x, mouse_y;
glfwGetCursorPos(window, &mouse_x, &mouse_y);
static int center_x;
static int center_y;

if (mouse_toggle)
{
	 center_x = mouse_x;
	 center_y = WINDOW_HEIGHT - mouse_y;
}

for (int i = 0; i < num_particles; i++)
{
	float fx, fy;

	calculate_centripetal_force(&particles[i], center_x, center_y, &fx, &fy);
	particles[i].vx += fx * deltaTime;
	particles[i].vy += fy * deltaTime;

	calculate_tangential_force(&particles[i], center_x, center_y, &fx, &fy);
	apply_damping_force(&particles[i], AIR_RESISTANCE);
	particles[i].vx += fx * deltaTime;
	particles[i].vy += fy * deltaTime;
}
	static int count = 0;
	if (grav_toggle)
	{
		for (int i = 0; i < num_particles; i++)
		{
			count++;
			for (int j = i + 1; j < num_particles; j++)
			{
				float fx, fy;
				calculate_gravitational_force(&particles[i], &particles[j], &fx, &fy);
				particles[i].vx += fx / particles[i].mass * deltaTime;
				particles[i].vy += fy / particles[i].mass * deltaTime;
				particles[j].vx -= fx / particles[j].mass * deltaTime;
				particles[j].vy -= fy / particles[j].mass * deltaTime;
				apply_damping_force(&particles[i], AIR_RESISTANCE);
			}
		}
	}
	for (int i = 0; i < num_particles; i++)
	{
		float x_inc = particles[i].vx * deltaTime;
		float y_inc = particles[i].vy * deltaTime;
		if (x_inc < WINDOW_WIDTH)
			particles[i].x += x_inc;
		if (y_inc < WINDOW_HEIGHT)
			particles[i].y += y_inc;

		if (particles[i].x < 0 || particles[i].x >= WINDOW_WIDTH ) {
			particles[i].vx = -particles[i].vx * WALL_BOUNCE;
			particles[i].vy = particles[i].vy * WALL_BOUNCE;
		}
		if (particles[i].y < 0 || particles[i].y >= WINDOW_HEIGHT ) {
			particles[i].vy = -particles[i].vy * 0.9f;
			particles[i].vx = particles[i].vx * 0.9f;
		}
		particles[i].color.g = (sqrtf(particles[i].vy * particles[i].vy + particles[i].vx * particles[i].vx) / 150);
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
        toggle(&mouse_toggle);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	int og_w = WINDOW_WIDTH;
	int og_h = WINDOW_HEIGHT;
	
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
	if (og_w > width || og_h > height)
		init_particles();
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

	// Create a windowed mode window and its OpenGL context
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	#if __APPLE__
	// GL 3.2 Core + GLSL 150
	const char *glsl_version = "#version 330";
	#else
	// GL 3.2 + GLSL 130
	const char *glsl_version = "#version 130";
	#endif

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Particles", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	
    // Make the window's context current
    glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetKeyCallback(window, key_callback);
    
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
	ImGui_ImplOpenGL3_Init(glsl_version);

	igStyleColorsDark(NULL);
	// ImFontAtlas_AddFontDefault(io.Fonts, NULL);

	bool showDemoWindow = true;
	bool showAnotherWindow = false;

	init_particles();

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

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

	double lastFrameTime = glfwGetTime();


	while (!glfwWindowShouldClose(window))
	{
		// Clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastFrameTime; 
		lastFrameTime = currentTime;
		update_particles(particles, NUM_PARTICLES, deltaTime, window);
		render_particles(shaderProgram, particles, NUM_PARTICLES, P_SIZE);

		// start imgui frame
		ImGui_ImplOpenGL3_NewFrame();
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
			igCheckbox("GravToggle", &grav_toggle);

			igSliderInt("TangiForce", &F_MULTIP, -7500, 7500, "%.d", 0);
			igSliderInt("CentriForce", &CENTRI_MULTI, -250000, 250000, "%.d", 0);
			igSliderInt("GravForce", &GRAVITATIONAL_CONSTANT, 1, 1000, "%.d", 0);
			igSliderFloat("WallBounce", &WALL_BOUNCE, 0.0f, 2.0f, "%.2f", 0);
			igSliderFloat("AirRez", &AIR_RESISTANCE, 1.0f, 0.5f, "%.5f", 0);
			igSliderFloat("P-Size", &P_SIZE, 0.5f, 50.0f, "%.5f", 0);
			igSliderInt("P-Amount", &NUM_PARTICLES, 1, CUR_MAX_PARTICLES, "%.d", 0);
			igColorEdit4("clear color", (float *)&baseColor, 0);

			static paColor ff;
				igPushID_Int(1);
				igVSliderFloat("##v", (ImVec2){18, 120}, &(baseColor.x), 0, 1, "", 0);
				igPopID();
				igSameLine(0.0f, -1.0f);
				igPushID_Int(2);
				igVSliderFloat("##v", (ImVec2){18, 120}, &(baseColor.y), 0, 1, "", 0);
				igPopID();
				igSameLine(0.0f, -1.0f);
				igPushID_Int(3);
				igVSliderFloat("##v", (ImVec2){18, 120}, &(baseColor.z), 0, 1, "", 0);
				igPopID();
				igSameLine(0.0f, -1.0f);
			
			ImVec2 buttonSize;
			buttonSize.x = 0;
			buttonSize.y = 0;
			if (igButton("Reset particles", buttonSize))
				init_particles();
			// igSameLine(0.0f, -1.0f);
			// igText("counter = %d", counter);

			// igText("Application %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
			igEnd();
		}

		static ImVec4 old_color; 
		if (old_color.x == 0)
		{
			old_color.x = baseColor.x;
			old_color.y = baseColor.y;
			old_color.z = baseColor.z;
		}
		if (baseColor.x != old_color.x || old_color.y != old_color.y || old_color.z != old_color.z)
		{
			old_color = (ImVec4)baseColor;
			update_colors();
		}


		if (grav_toggle)
		{
			CUR_MAX_PARTICLES = GRAV_MAX_PARTICLES;
			if (NUM_PARTICLES > GRAV_MAX_PARTICLES)
				NUM_PARTICLES = GRAV_MAX_PARTICLES;
		}
		else
		{
			CUR_MAX_PARTICLES = MAX_PARTICLES;
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
		char title[128];
		snprintf(title, 100, "Particles  -  %-3.fms/frame (%-3.f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
		glfwSetWindowTitle(window, title);

		// render
		
		igRender();
		glfwMakeContextCurrent(window);
		// glViewport(0, 0, (int)ioptr->DisplaySize.x, (int)ioptr->DisplaySize.y);
		// glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		// glClear(GL_COLOR_BUFFER_BIT);

		
		ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
		#ifdef IMGUI_HAS_DOCK
		if (ioptr->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
			GLFWwindow *backup_current_window = glfwGetCurrentContext();
			igUpdatePlatformWindows();
			igRenderPlatformWindowsDefault(NULL, NULL);
			glfwMakeContextCurrent(backup_current_window);
		}
		#endif


        // Swap buffers and poll for events
        glfwSwapBuffers(window);
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

	glfwDestroyWindow(window);
    glfwTerminate();
	free(particles);
    return 0;
}
