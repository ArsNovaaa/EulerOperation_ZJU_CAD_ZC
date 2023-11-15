#include"Renderer.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"
#include"VertexArray.h"
#include"Shader.h"
#include"VertexBufferLayout.h"
#include"Texture.h"
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"imgui.h"
#include"imgui_impl_opengl3.h"
#include"imgui_impl_glfw.h"
#include"tests/TestClearColor.h"
#include"tests/Test.h"
#include"tests/TestTexture2D.h"
#include"tests/TestBezierCurve.h"
#include"tests/TestDataFitting.h"
#include"tests/TestCadModel.h"

int main(void)
{
    GLFWwindow* window;

    
    if (!glfwInit())
        return -1;

   
    window = glfwCreateWindow(1600, 1200, "Ars Nova", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    {

        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }


        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
       

        const char* glsl_version = "#version 130";
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();
        ImGui_ImplOpenGL3_Init(glsl_version);

        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;

     
        testMenu->RegisterTest<test::TestBezierCurve>("Bezier Curve",window);
        testMenu->RegisterTest<test::TestDataFitting>("Data Fitting");
        testMenu->RegisterTest<test::TestCadModel>("Euler Operation",window);

        while (!glfwWindowShouldClose(window))
        {
           
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
           
            
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

           
            if (currentTest)
            {
                
                
                ImGui::Begin("Test");
                currentTest->OnUpdate(0.0f);
                currentTest->OnRender();

                if (currentTest != testMenu && ImGui::Button("<-"))
                {
                    delete currentTest;
                    currentTest = testMenu;

                }
                currentTest->OnImGuiRender();
                
                ImGui::End();
                
            }


           
            ImGui::Render();
            
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            
            glfwSwapBuffers(window);
           
        }
        if (currentTest != testMenu)
            delete testMenu;
        delete currentTest;
        
            
    }
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
    
    return 0;
}