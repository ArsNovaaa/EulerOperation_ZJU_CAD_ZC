#pragma once
#include<vector>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "glm/glm.hpp"

#include "Renderer.h"
#include"Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include"structure/HalfEdge.h"
#include"Texture.h"
#include"algorithm/eularOperation/EulerOperation.h"
#include"algorithm/triangulation/Triangulation.h"
#include"common/controls.h"

namespace test {

	class TestCadModel :public Test
	{
	public:
		TestCadModel(GLFWwindow* const window);
		~TestCadModel();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		Solid* CreateModel1();
		Solid* CreateModel2();
		Solid* CreateModel3();
		
	private:
		Solid* solids=nullptr;
		Solid* oldSolids = nullptr;
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;
		ImVec4 m_ClearColor;
		glm::mat4 m_Proj, m_View, m_Model, m_Mvp;
		glm::vec3 m_Translation;
		float m_viewField = 90.0f;
		GLFWwindow* const m_window;
		ImVec4 m_camPos;

		bool canSelfRotate = true;
		bool canRotate45 = false;
		bool canMouseRotate = false;
		int isOrthoCamera = 0;
	};



	std::vector<Vertex*> getVertexList(Solid* s);
	float* getVertexBufferData(std::vector<Vertex*> points,int& size);
	unsigned int* getIndexBufferData(Solid* s,unsigned int& indicesNumber);
	
}