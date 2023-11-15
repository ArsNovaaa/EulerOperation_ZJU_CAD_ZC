#pragma once
#include"Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include <memory>

namespace test {
	 
  class TestBezierCurve :public Test
 {
    public:
	  TestBezierCurve(GLFWwindow* const window);
	 ~TestBezierCurve();

	void OnUpdate(float deltaTime) override;
	void OnRender() override;
	void OnImGuiRender() override;
	
	
    
   private:
	   std::unique_ptr<VertexArray> m_VAO;
	   std::unique_ptr<VertexBuffer> m_VertexBuffer;
	   std::unique_ptr<IndexBuffer> m_IndexBuffer;
	   std::unique_ptr<Shader> m_Shader;
	   GLFWwindow *const m_window;

 };
  void mouse_callback(GLFWwindow* window, double x, double y);
  void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
}
