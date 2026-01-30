#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <math.h>

// -------- shared controls (defined in main.cpp) --------
extern bool g_showPendulums;
extern bool g_showTrails;
extern bool g_pause;

extern float g_l1;
extern float g_l2;
extern float g_m1;
extern float g_m2;
extern float g_gravity;
// ------------------------------------------------------

class Pendulum
{
public:
    const float PI = 3.14159265358979323846f;

    float l1 = 100.0f;
    float l2 = 100.0f;
    float m1 = 30.0f;
    float m2 = 10.0f;

    float theta1 = 0.0f;
    float theta2 = PI / 3.0f;
    float omega1 = 0.0f;
    float omega2 = 0.0f;
    float accel1 = 0.0f;
    float accel2 = 0.0f;

    struct Color { float r, g, b; };
    Color trailColor{ 1, 1, 1 };

    struct TrailPoint { float x, y; };
    std::vector<TrailPoint> trail;
    const int MAX_TRAIL = 100;

    Pendulum(float offset, float hue)
    {
        theta2 += offset;
        trailColor = {
            fabsf(sinf(hue)),
            fabsf(sinf(hue + 2.1f)),
            fabsf(sinf(hue + 4.2f))
        };
    }

	Pendulum(float initial_theta1, float initial_theta2, float hue) {
		theta1 = initial_theta1;
		theta2 = initial_theta2;
        trailColor = {
            fabsf(sinf(hue)),
            fabsf(sinf(hue + 2.1f)),
            fabsf(sinf(hue + 4.2f))
        };
    }

    void updateMotionRK4(float dt)
    {
        if (g_pause) return;

        l1 = g_l1; l2 = g_l2;
        m1 = g_m1; m2 = g_m2;

        // helper lambda to compute accelerations
        auto accel = [&](float th1, float th2, float w1, float w2, float& a1, float& a2) {
            float num1 = -g_gravity * (2 * m1 + m2) * sin(th1);
            float num2 = -m2 * g_gravity * sin(th1 - 2 * th2);
            float num3 = -2 * sin(th1 - th2) * m2;
            float num4 = w2 * w2 * l2 + w1 * w1 * l1 * cos(th1 - th2);
            float den = l1 * (2 * m1 + m2 - m2 * cos(2 * th1 - 2 * th2));
            a1 = (num1 + num2 + num3 * num4) / den;

            num1 = 2 * sin(th1 - th2);
            num2 = w1 * w1 * l1 * (m1 + m2);
            num3 = g_gravity * (m1 + m2) * cos(th1);
            num4 = w2 * w2 * l2 * m2 * cos(th1 - th2);
            den = l2 * (2 * m1 + m2 - m2 * cos(2 * th1 - 2 * th2));
            a2 = (num1 * (num2 + num3 + num4)) / den;
            };

        // store original state
        float th1 = theta1, th2 = theta2;
        float w1 = omega1, w2 = omega2;
        float a1, a2;

        // --- k1 ---
        accel(th1, th2, w1, w2, a1, a2);
        float k1_th1 = w1;
        float k1_th2 = w2;
        float k1_w1 = a1;
        float k1_w2 = a2;

        // --- k2 ---
        accel(th1 + 0.5f * k1_th1 * dt, th2 + 0.5f * k1_th2 * dt,
            w1 + 0.5f * k1_w1 * dt, w2 + 0.5f * k1_w2 * dt, a1, a2);
        float k2_th1 = w1 + 0.5f * k1_w1 * dt;
        float k2_th2 = w2 + 0.5f * k1_w2 * dt;
        float k2_w1 = a1;
        float k2_w2 = a2;

        // --- k3 ---
        accel(th1 + 0.5f * k2_th1 * dt, th2 + 0.5f * k2_th2 * dt,
            w1 + 0.5f * k2_w1 * dt, w2 + 0.5f * k2_w2 * dt, a1, a2);
        float k3_th1 = w1 + 0.5f * k2_w1 * dt;
        float k3_th2 = w2 + 0.5f * k2_w2 * dt;
        float k3_w1 = a1;
        float k3_w2 = a2;

        // --- k4 ---
        accel(th1 + k3_th1 * dt, th2 + k3_th2 * dt,
            w1 + k3_w1 * dt, w2 + k3_w2 * dt, a1, a2);
        float k4_th1 = w1 + k3_w1 * dt;
        float k4_th2 = w2 + k3_w2 * dt;
        float k4_w1 = a1;
        float k4_w2 = a2;

        // --- combine ---
        theta1 += dt / 6.0f * (k1_th1 + 2 * k2_th1 + 2 * k3_th1 + k4_th1);
        theta2 += dt / 6.0f * (k1_th2 + 2 * k2_th2 + 2 * k3_th2 + k4_th2);
        omega1 += dt / 6.0f * (k1_w1 + 2 * k2_w1 + 2 * k3_w1 + k4_w1);
        omega2 += dt / 6.0f * (k1_w2 + 2 * k2_w2 + 2 * k3_w2 + k4_w2);
    }

    void updateTrail(float x, float y)
    {
		if (g_pause) return;
        trail.push_back({ x, y });
        if ((int)trail.size() > MAX_TRAIL)
            trail.erase(trail.begin());
    }

    void drawTrail()
    {
        if (!g_showTrails || trail.size() < 2) return;

        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < trail.size(); i++)
        {
            float a = (float)i / trail.size();
            glColor4f(trailColor.r, trailColor.g, trailColor.b, a);
            glVertex2f(trail[i].x, trail[i].y);
        }
        glEnd();
    }

    void draw(float cx, float cy)
    {
        drawTrail();
        if (!g_showPendulums) return;

        float x2 = cx + l1 * sin(theta1);
        float y2 = cy - l1 * cos(theta1);
        float x3 = x2 + l2 * sin(theta2);
        float y3 = y2 - l2 * cos(theta2);

        glColor3f(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2f(cx, cy); glVertex2f(x2, y2);
        glVertex2f(x2, y2); glVertex2f(x3, y3);
        glEnd();

        glPointSize(6);
        glBegin(GL_POINTS);
        glVertex2f(x2, y2);
        glVertex2f(x3, y3);
        glEnd();
    }
};
