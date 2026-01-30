#include <GLFW/glfw3.h>
#include <vector>
#include "Pendulum.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

// ---------- globals ----------
const int WIDTH = 1000;
const int HEIGHT = 600;
const float PI = 3.14159265358979323846f;

bool g_pause = false;
bool g_reverse = false;
bool g_showPendulums = false;
bool g_showTrails = true;

float g_thetaOffset = 0.012f;
float g_hueOffset = 0.007f;
float g_theta1 = 0.0f;
float g_theta2 = PI / 3.0f;
float g_l1 = 100.0f;
float g_l2 = 100.0f;
float g_m1 = 30.0f;
float g_m2 = 10.0f;
float g_gravity = -9.81f;

int g_count = 250;

std::vector<Pendulum> pendulums;

static void initPendulums(int count)
{
    pendulums.clear();

    for (int i = 0; i < count; i++)
    {
        Pendulum p(g_theta1, g_theta2 + i * g_thetaOffset, i * g_hueOffset);
        pendulums.push_back(p);
    }
}

int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Double Pendulum", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // ---- OpenGL ----
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---- ImGui ----
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();
    ImGui::StyleColorsDark();

    initPendulums(g_count);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        // --- ImGui frame ---
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Simulation Controls");
		ImGui::Checkbox("Pause", &g_pause);
		ImGui::Checkbox("Reverse", &g_reverse);
        ImGui::Checkbox("Show Pendulums", &g_showPendulums);
        ImGui::Checkbox("Show Trails", &g_showTrails);

		ImGui::Separator();

        if (ImGui::SliderFloat("Angle offset", &g_thetaOffset, 0.0001, 0.5)) {
			initPendulums(g_count);
        }
        if (ImGui::SliderFloat("Hue offset", &g_hueOffset, 0.0001, 0.1)) {
			initPendulums(g_count);
        }
        if (ImGui::SliderAngle("Initial O1", &g_theta1, -180.0, 180.0)) {
			initPendulums(g_count);
        }
        if (ImGui::SliderAngle("Initial O2", &g_theta2, -180.0, 180.0)) {
			initPendulums(g_count);
        }
        ImGui::SliderFloat("L1", &g_l1, 20, 300);
        ImGui::SliderFloat("L2", &g_l2, 20, 300);
        ImGui::SliderFloat("M1", &g_m1, 1, 100);
        ImGui::SliderFloat("M2", &g_m2, 1, 100);
        ImGui::SliderFloat("Gravity", &g_gravity, -30, 30);

        if (ImGui::SliderInt("Count", &g_count, 1, 500))
            initPendulums(g_count);

        if (ImGui::Button("Reset"))
            initPendulums(g_count);
        ImGui::End();

        // --- Simulation ---
        for (auto& p : pendulums)
        {
			float dt = g_reverse ? -0.01f : 0.01f;
			for (int i = 0; i < 5; i++)
                p.updateMotionRK4(dt);
            float cx = WIDTH / 2.0f;
            float cy = HEIGHT / 2.0f;
            float x = cx + g_l1 * sin(p.theta1) + g_l2 * sin(p.theta2);
            float y = cy - g_l1 * cos(p.theta1) - g_l2 * cos(p.theta2);
            p.updateTrail(x, y);
            p.draw(cx, cy);
        }

        // --- Render ImGui ---
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
