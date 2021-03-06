// g++ -Wall -std=c++11 -lGL -lGLEW -lglfw main.cpp -o colors

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream> // for debugging

GLfloat vertices[] = {
     0.0f,  0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f
};

const GLchar* vertexSource =
    "#version 140\n"
    "\n"
    "in vec2 position;\n"
    "in vec3 color;\n"
    "out vec3 fragColor;\n"
    "\n"
    "void main() {\n"
    "    fragColor = color;\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

const GLchar* fragSource =
    "#version 140\n"
    "\n"
    "uniform float time;\n"
    "in vec3 fragColor;\n"
    "out vec4 outColor;\n"
    "\n"
    "void main() {\n"
    "    outColor.x = (1 + sin(time)) / 2.0 * (1 - fragColor.x);\n"
    "    outColor.y = (1 + sin(time + 1.047)) / 2.0 * (1 - fragColor.y);\n"
    "    outColor.z = (1 + sin(time + 2.094)) / 2.0 * (1 - fragColor.z);\n"
    "    outColor.w = 1.0;\n"
    "}\n";

static void err_callback(int error, const char* description) {
    std::cout << error << ": " << description << std::endl;
}

static void init_glfw() {
    glfwInit();
    glfwSetErrorCallback(err_callback);

    // Should be using 3.2, but Sandy Bridge does not support it.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}

static void init_glew() {
    glewExperimental = GL_TRUE;
    glewInit();
}

static GLFWwindow* create_window(int width, int height, const char* title) {
    GLFWwindow* w = glfwCreateWindow(width, height, title,
                                     glfwGetPrimaryMonitor(), nullptr);
    glfwMakeContextCurrent(w);

    return w;
}

static GLuint compile_shader(GLenum type, const GLchar* source) {
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        std::cerr << buffer << std::endl;
    }

    return shader;
}

static GLuint link_program(GLuint vertexShader, GLuint fragShader) {
    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragShader);

    glBindFragDataLocation(program, 0, "outColor");
    glLinkProgram(program);

    return program;
}

int main() {
    init_glfw();
    GLFWwindow* window = create_window(1920, 1080, "OpenGL");
    init_glew();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragSource);

    GLuint program = link_program(vertexShader, fragmentShader);
    glUseProgram(program);

    GLint posAttrib = glGetAttribLocation(program, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colorAttrib = glGetAttribLocation(program, "color");
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(colorAttrib);

    GLint timeLocation = glGetUniformLocation(program, "time");

    while(!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1f(timeLocation, glfwGetTime());

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glDeleteProgram(program);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();
}
