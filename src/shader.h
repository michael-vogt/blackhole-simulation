#pragma once

#include <GL/glew.h>
#include <cstddef>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

using namespace std;

GLuint rayVAO, rayVBO;
GLuint bhVAO, bhVBO;
GLuint shaderProgram;

GLint projLoc;
GLint offsetLoc;
GLint colorLoc;

struct Vertex {
    float x, y;
    float alpha;
};

const char* vertexShaderSrc = R"(
#version 120
attribute vec2 aPos;
attribute float aAlpha;
uniform mat4 uProjection;
uniform vec2 uOffset;
varying float vAlpha;
void main()
{
    vAlpha = aAlpha;
    gl_Position = uProjection * vec4(aPos + uOffset, 0.0, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 120
varying float vAlpha;
uniform vec3 uColor;
void main()
{
    gl_FragColor = vec4(uColor, vAlpha);
}
)";


GLuint compileShader(GLenum type, const char* src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader Error: " << log << "\n";
    }

    return shader;
}

GLuint createProgram()
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        cout << "Program Error:\n" << log << endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void initGPU() {
    // ---------- RAYS ----------
    glGenVertexArrays(1, &rayVAO);
    glGenBuffers(1, &rayVBO);

    glBindVertexArray(rayVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 10000, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // ---------- BLACK HOLE ----------
    //vector<float> circle = createCircle(SagA.r_s, 100);

    glGenVertexArrays(1, &bhVAO);
    glGenBuffers(1, &bhVBO);

    glBindVertexArray(bhVAO);

    glBindBuffer(GL_ARRAY_BUFFER, bhVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 10000, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    shaderProgram = createProgram();
    projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    offsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
    colorLoc = glGetUniformLocation(shaderProgram, "uColor");
}

void renderRay(vector<Vertex> vertices) {
    glBindVertexArray(rayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    GLint aPosLoc = glGetAttribLocation(shaderProgram, "aPos");
    GLint aAlphaLoc = glGetAttribLocation(shaderProgram, "aAlpha");

    glEnableVertexAttribArray(aPosLoc);
    glVertexAttribPointer(aPosLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(aAlphaLoc);
    glVertexAttribPointer(aAlphaLoc, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, alpha));

    glUniform2f(offsetLoc, 0.0f, 0.0f);
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

    glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
}

void renderBH(vector<Vertex> vertices, glm::vec2 offset = {0.0f, 0.0f}, glm::vec3 color = {1.0f, 0.0f, 0.0f}) {
    glBindVertexArray(bhVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bhVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    GLint aPosLoc = glGetAttribLocation(shaderProgram, "aPos");
    GLint aAlphaLoc = glGetAttribLocation(shaderProgram, "aAlpha");

    glEnableVertexAttribArray(aPosLoc);
    glVertexAttribPointer(aPosLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(aAlphaLoc);
    glVertexAttribPointer(aAlphaLoc, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, alpha));

    glUniform2f(offsetLoc, offset.x, offset.y);
    glUniform3f(colorLoc, color.x, color.y, color.z);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
}
