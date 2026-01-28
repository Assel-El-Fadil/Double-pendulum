#include <GLFW/glfw3.h>
#include <math.h>

const int SCREENWIDTH = 1000;
const int SCREENHEIGHT = 600;
const float PI = 3.14159265358979323846f;

const float GRAVITY = -9.81f;

float l1 = 200.0f;
float l2 = 100.0f;
float m1 = 40.0f;
float m2 = 10.0f;
float theta1 = PI / 4;
float theta2 = PI / 2;
float omega1 = 0.0f;
float omega2 = 0.0f;
float accel1 = 0.0f;
float accel2 = 0.0f;


static void drawPendulum(float x, float y)
{
    float x2 = x + l1 * sin(theta1);
    float y2 = y - l1 * cos(theta1);

	float x3 = x2 + l2 * sin(theta2);
	float y3 = y2 - l2 * cos(theta2);

    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x2, y2);
    glEnd();

    glBegin(GL_POINTS);
    glVertex2f(x2, y2);
    glEnd();

    glBegin(GL_LINES);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();

    glBegin(GL_POINTS);
    glVertex2f(x3, y3);
	glEnd();
}

static void draw()
{
    drawPendulum(SCREENWIDTH / 2.0f, SCREENHEIGHT / 4.0f);
}

static void updatePhysics(float dt)
{
    float num1 = -GRAVITY * (2 * m1 + m2) * sin(theta1);
    float num2 = -m2 * GRAVITY * sin(theta1 - 2 * theta2);
    float num3 = -2 * sin(theta1 - theta2) * m2;
    float num4 = omega2 * omega2 * l2 + omega1 * omega1 * l1 * cos(theta1 - theta2);
    float den = l1 * (2 * m1 + m2 - m2 * cos(2 * theta1 - 2 * theta2));
    accel1 = (num1 + num2 + num3 * num4) / den;

    num1 = 2 * sin(theta1 - theta2);
    num2 = (omega1 * omega1 * l1 * (m1 + m2));
    num3 = GRAVITY * (m1 + m2) * cos(theta1);
    float num4_2 = omega2 * omega2 * l2 * m2 * cos(theta1 - theta2);
    den = l2 * (2 * m1 + m2 - m2 * cos(2 * theta1 - 2 * theta2));
    accel2 = (num1 * (num2 + num3 + num4_2)) / den;

    omega1 += accel1 * dt;
    omega2 += accel2 * dt;
    theta1 += omega1 * dt;
    theta2 += omega2 * dt;
}

int main()
{
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(
        SCREENWIDTH, SCREENHEIGHT, "Physics Ball", NULL, NULL);

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

    glPointSize(6.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // --- Main loop ---
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        glColor3f(0.2f, 0.6f, 1.0f);
		updatePhysics(0.16f);
        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
