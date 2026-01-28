#include <GLFW/glfw3.h>
#include <math.h>
#include <vector>
#include "Pendulum.h"

const int SCREENWIDTH = 1000;
const int SCREENHEIGHT = 600;

// --- Trail storage ---
struct TrailPoint {
    float x, y;
};

std::vector<Pendulum> pendulums;

static void initPendulums(int count) {
	float offset = 0.0001f;
    pendulums.clear();
    for (int i = 0; i < count; i++) {
        Pendulum p(offset);
        pendulums.push_back(p);
		offset += 0.0001f;
    }
}

// --------------------------------------------------

static void update(float dt)
{
    for (Pendulum& p : pendulums)
    {
        p.updateMotion(dt);

        float px, py;
        p.getSmallBallPosition(
            SCREENWIDTH / 2.0f,
            SCREENHEIGHT / 2.0f,
            px, py
        );

        p.updateTrail(px, py);
    }
}


static void draw() {
    for (Pendulum& p : pendulums) {
        p.draw();
    }
}

// --------------------------------------------------

int main()
{
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(
        SCREENWIDTH, SCREENHEIGHT, "Double Pendulum", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // --- OpenGL setup ---
    glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREENWIDTH, SCREENHEIGHT, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	initPendulums(500);

    // --- Main loop ---
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        update(0.1f);
        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
