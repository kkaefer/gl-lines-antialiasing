

#if defined(__linux__)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>




#include "mat4.hpp"
#include "png.hpp"

#include <memory>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <cmath>

void printShaderInfoLog(GLuint shader) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        auto msg = std::unique_ptr<GLchar[]>(new GLchar[length]);
        glGetShaderInfoLog(shader, length, &length, msg.get());
        fprintf(stderr, "Failed to compile shader: %s\n", msg.get());
    }
}

void printProgramInfoLog(GLuint program) {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        auto msg = std::unique_ptr<GLchar[]>(new GLchar[length]);
        glGetProgramInfoLog(program, length, &length, msg.get());
        fprintf(stderr, "Failed to validate program: %s\n", msg.get());
    }
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        printShaderInfoLog(shader);
        exit(1);
    }

    return shader;
}

GLuint createProgram(const char* vertexShader, const char* fragmentShader) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLuint p = glCreateProgram();
    glAttachShader(p, fs);
    glAttachShader(p, vs);

    // Link program
    GLint status;
    glLinkProgram(p);
    glGetProgramiv(p, GL_LINK_STATUS, &status);
    if (status == 0) {
        printProgramInfoLog(p);
        abort();
    }

    // Validate program
    glValidateProgram(p);
    glGetProgramiv(p, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        printProgramInfoLog(p);
        exit(1);
    }
    return p;
}

int main(void) {
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    const int width = 640;
    const int height = 480;

    static float offset_x = 0;
    static float offset_y = 0;

    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int, int action, int) {
        if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) {
            glfwSetWindowShouldClose(w, true);
        }

        if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
            if (key == GLFW_KEY_LEFT) {
                offset_x -= 0.05;
                fprintf(stderr, "offset: %f/%f\n", offset_x, offset_y);
            }
            if (key == GLFW_KEY_RIGHT) {
                offset_x += 0.05;
                fprintf(stderr, "offset: %f/%f\n", offset_x, offset_y);
            }
            if (key == GLFW_KEY_UP) {
                offset_y -= 0.05;
                fprintf(stderr, "offset: %f/%f\n", offset_x, offset_y);
            }
            if (key == GLFW_KEY_DOWN) {
                offset_y += 0.05;
                fprintf(stderr, "offset: %f/%f\n", offset_x, offset_y);
            }
        }
    });

    glfwMakeContextCurrent(window);

    const char* outlineVertexShader = R"outline_vert(
#version 120
attribute vec2 a_pos;
uniform mat4 u_matrix;

varying vec2 v_pos;

void main() {
    vec4 pos = u_matrix * vec4(a_pos.xy, 0, 1);
    gl_Position = pos;
    v_pos = vec2(a_pos.x, 480 - a_pos.y);
}
)outline_vert";

    const char* outlineFragmentShader = R"outline_frag(
#version 120
uniform vec4 u_color;
uniform vec2 u_offset;

varying vec2 v_pos;

void main() {
    vec2 xy = v_pos - gl_FragCoord.xy + u_offset;
    float alpha = length(xy);
    gl_FragColor = vec4(alpha, alpha, alpha, 1);
}
)outline_frag";

    const auto program = createProgram(outlineVertexShader, outlineFragmentShader);
    assert(program);
    const auto a_pos = glGetAttribLocation(program, "a_pos");
    assert(a_pos >= 0);
    const auto u_matrix = glGetUniformLocation(program, "u_matrix");
    assert(u_matrix >= 0);
    const auto u_world = glGetUniformLocation(program, "u_world");
    // assert(u_world >= 0);
    const auto u_color = glGetUniformLocation(program, "u_color");
    const auto u_offset = glGetUniformLocation(program, "u_offset");
    // assert(u_color >= 0);


    fprintf(stderr, "program: %d\n", program);
    fprintf(stderr, "a_pos: %d\n", a_pos);
    fprintf(stderr, "u_matrix: %d\n", u_matrix);
    fprintf(stderr, "u_world: %d\n", u_world);
    fprintf(stderr, "u_color: %d\n", u_color);
    fprintf(stderr, "u_offset: %d\n", u_offset);

    std::vector<uint16_t> points;


    for (double i = 0; i < 2 * M_PI; i += M_PI / 32) {

        points.push_back(320 + 50 * std::cos(i));
        points.push_back(240 + 50 * std::sin(i));
        points.push_back(320 + 300 * std::cos(i));
        points.push_back(240 + 300 * std::sin(i));

    }

    for (int i = -500; i < 1500; i += 50) {
    }

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(points.size() * sizeof(uint16_t)), points.data(), GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray((GLuint)a_pos);
    glVertexAttribPointer((GLuint)a_pos, 2, GL_SHORT, GL_FALSE, 0, nullptr);

    mat4 pMatrix;
    matrix::ortho(pMatrix, 0, width, height, 0, 0, 1);

    mat4 mvMatrix;
    matrix::identity(mvMatrix);

    mat4 mvpMatrix;
    matrix::multiply(mvpMatrix, pMatrix, mvMatrix);

    glUseProgram(program);


    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mvpMatrix.data());
    if (u_world >= 0) glUniform2f(u_world, width, height);
    glUniform4f(u_color, 1, 1, 1, 1);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(220.0/255.0, 220.0/255.0, 220.0/255.0, 1);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, width, height);

        glBindVertexArray(vao);
        glLineWidth(10);
        glDrawArrays(GL_LINES, 0, points.size() / 2);


        if (u_offset >= 0) glUniform2f(u_offset, offset_x, offset_y);

        const auto pixels = std::unique_ptr<uint32_t[]>(new uint32_t[width * height]);

        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());

        const int stride = width * 4;
        const auto tmp = std::unique_ptr<char[]>(new char[stride]);
        char *rgba = reinterpret_cast<char *>(pixels.get());
        for (int i = 0, j = height - 1; i < j; i++, j--) {
            std::memcpy(tmp.get(), rgba + i * stride, stride);
            std::memcpy(rgba + i * stride, rgba + j * stride, stride);
            std::memcpy(rgba + j * stride, tmp.get(), stride);
        }

        write_file("out.png", compress_png(width, height, rgba));

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwTerminate();
    return 0;
}
