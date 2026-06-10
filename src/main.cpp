#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/common.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <fstream>
#include <sstream>
#include "shader.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "constants.h"
#include "ray.h"

using namespace glm;
using namespace std;
using Clock = std::chrono::high_resolution_clock;


struct Engine {
    GLFWwindow* window;
    int WIDTH = 800;
    int HEIGHT = 600;
    float width = 1e11;
    float height = 7.5e10;

    Engine() {
        if (!glfwInit()) {
            cerr << "GLFW init failed\n";
            exit(EXIT_FAILURE);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "BlackHoleSimulator", nullptr, nullptr);
        if (!window) {
            cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();
        if (glewErr != GLEW_OK) {
            cerr << "Failed to initialize GLEW: "
                 << (const char*)glewGetErrorString(glewErr)
                 << "\n";
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glViewport(0, 0, WIDTH, HEIGHT);
    }
};
Engine engine;

struct BlackHole {
    vec2 position;
    double mass;
    double r_s;

    BlackHole(vec2 pos, float m) : position(pos), mass(m) { r_s = 2.0 * G * mass / (c*c); }
};
BlackHole SagA(vec2(0.0f, 0.0), 8.54e36);


vector<Ray> rays;

vector<vec2> createCircle(float radius, int segments) {
    vector<vec2> v;

    for (int i = 0; i <= segments; i++) {
        float a = 2.0f * M_PI * i /segments;
        v.push_back({cos(a) * radius, sin(a) * radius});
    }

    return v;
}

int main()
{
    for (float y = -engine.height; y < engine.height; y+=1e10) {
        rays.push_back(Ray(vec2(-engine.width, y), vec2(1.0, 0.0), SagA.r_s));
    }

    initGPU();

    mat4 projection = ortho(
        -engine.width, engine.width,
        -engine.height, engine.height,
        -1.0f, 1.0f
    );

    vector<Vertex> verticesBH;
    vector<Vertex> verticesRay;

    while (!glfwWindowShouldClose(engine.window)) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glPointSize(5.0f);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
        
        // ========== RAYS ==========
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(1.0f);

        for (auto& ray : rays) {
            ray.step(SagA.r_s, 1.0f);
            //geodesic(ray, SagA.r_s);
            
            verticesRay.clear();
            size_t totalVertices = ray.trail.size();

            for (int i = 0; i < totalVertices; i++) {
                vec2 v = ray.trail[i];
                float t = (float)i / (float)(totalVertices - 1);
                verticesRay.push_back({v.x, v.y, t });
            }            

            renderRay(verticesRay);
            //ray.step(SagA.r_s, 1);
        }

        // ========== BLACK HOLE ==========
        verticesBH.clear();
        for (auto& v : createCircle(SagA.r_s, 100)) {
            verticesBH.push_back({v.x, v.y, 1.0f});
        }

        renderBH(verticesBH, SagA.position);


        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}