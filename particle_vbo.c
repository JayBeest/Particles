#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

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

void drawRectangle(GLuint shaderProgram, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat screenWidth, GLfloat screenHeight) {
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

int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	// Create a windowed mode window and its OpenGL context
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	
    GLFWwindow* window = glfwCreateWindow(640, 480, "Rectangle", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    
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
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

       int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Draw rectangles
        drawRectangle(shaderProgram, 100.0f, 100.0f, 200.0f, 200.0f, 1.0f, 0.0f, 0.0f, 1.0f, (GLfloat)width, (GLfloat)height);
        drawRectangle(shaderProgram, 400.0f, 200.0f, 100.0f, 300.0f, 0.0f, 1.0f, 0.0f, 1.0f, (GLfloat)width, (GLfloat)height);


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

    glfwTerminate();
    return 0;
}
