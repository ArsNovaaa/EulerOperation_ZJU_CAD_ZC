#include"TestCadModel.h"
#include<fstream>

namespace test {
	TestCadModel::TestCadModel(GLFWwindow* const window)
		: m_Proj(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 10.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_camPos(4.0f, 3.0f, 3.0f, 1.0f), m_window(window)
	{
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glDepthFunc(GL_LESS));
		
		
		//default  object
		float positions[] = {
				-2.0f, -2.0f, -2.0f,  0.583f, 0.700f, 0.014f,
	             2.0f, -2.0f, -2.0f,  0.609f, 0.100f, 0.436f,
	             2.0f,  2.0f, -2.0f,  0.327f, 0.400f, 0.844f,
	            -2.0f,  2.0f, -2.0f,  0.822f, 0.500f, 0.201f,
	            

	            -2.0f, -2.0f,  2.0f,  0.435f, 0.600f, 0.223f,
	             2.0f, -2.0f,  2.0f,  0.310f, 0.700f, 0.185f,
	             2.0f,  2.0f,  2.0f,  0.597f, 0.700f, 0.761f,
	            -2.0f,  2.0f,  2.0f,  0.559f, 0.400f, 0.730f
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,

			4, 5, 6,
			6, 7, 4,

			1, 2, 5,
			5, 2, 6,

			0, 3, 4,
			3, 4, 7,
			
			0, 1, 4,
			0, 1, 5,

			2, 3, 7,
			2, 6, 7
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 6 * 8 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);
		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 36);

		m_Shader = std::make_unique<Shader>("res/shaders/CadModelTest.glsl");
		m_Shader->Bind();

	}

	TestCadModel::~TestCadModel()
	{
		m_VertexBuffer->Unbind();
		m_Shader->UnBind();
		m_IndexBuffer->Unbind();
		m_VAO->UnBind();
	}

	void TestCadModel::OnUpdate(float deltaTime)
	{
		double currentTime = glfwGetTime();
		m_Proj = isOrthoCamera ? glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 10.0f) 
			:glm::perspective(glm::radians(m_viewField), 4.0f / 3.0f, 0.1f, 100.0f);
		m_View  = glm::lookAt(
			glm::vec3(m_camPos.x, m_camPos.y, m_camPos.z), // Camera is at (4,3,3), in World Space
			glm::vec3(0, 0, 0),                      // and looks at the origin
			glm::vec3(0, 1, 0)                       // Head is up (set to 0,-1,0 to look upside-down)
		);

		glm::vec3 rotationAxis = glm::vec3(0, 1, 0);
		glm::mat4 RotationMatrix45 = canRotate45 ? glm::rotate((float)3.14f / 4, glm::vec3(1, 0, 0)) : glm::mat4(1.0);
		glm::mat4 RotationMatrix = canSelfRotate ? glm::rotate((float)currentTime, rotationAxis) : glm::mat4(1.0);

		
		// Compute the MVP matrix from keyboard and mouse input
		// computeMatricesFromInputs();
		// glm::mat4 ProjectionMatrix = getProjectionMatrix();
		// glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 m_Model = glm::mat4(1.0f);
		m_Model = m_View * RotationMatrix * RotationMatrix45;
		glm::mat4 m_View = getView(m_window, canMouseRotate);
		m_Mvp = m_Proj * m_View * m_Model;



		if (solids != oldSolids)
		{
			m_Shader->UnBind();
			m_VAO->UnBind();
			m_IndexBuffer->Unbind();
			m_VertexBuffer->Unbind();
			//VBO Data
			int vertexBufferSize = 0;
			std::vector<Vertex*> vList = getVertexList(solids);
		
			float* pos = getVertexBufferData(vList, vertexBufferSize);

			//VAO setting
			m_VAO = std::make_unique<VertexArray>();
			m_VertexBuffer = std::make_unique<VertexBuffer>(pos, vertexBufferSize * sizeof(float));
			VertexBufferLayout newlayout;
			newlayout.Push<float>(3);
			newlayout.Push<float>(3);
			m_VAO->AddBuffer(*m_VertexBuffer, newlayout);
			//IBO setting
			unsigned int indexBufferSize = 0;
			unsigned int* indexBufferData = getIndexBufferData(solids, indexBufferSize);
			m_IndexBuffer = std::make_unique<IndexBuffer>(indexBufferData, indexBufferSize);

			

			m_Shader = std::make_unique<Shader>("res/shaders/CadModelPureColor.glsl");
			m_Shader->Bind();
			m_ClearColor = ImVec4(1.0f, 0.5f, 0.31f, 1.00f);
			//m_Shader->SetUniform4f("objectColor", m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w);

			oldSolids = solids;

		}
	



		
	}

	void TestCadModel::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		
		Renderer renderer;

		
		
		{
			
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", m_Mvp);	
			//if (solids)
			//{
				//m_Shader->SetUniform4f("objectColor", m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w);
			//}
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		
		
	
	}

	void TestCadModel::OnImGuiRender()
	{
		if(solids)euler::InfoSolid(solids);
		
		
		//ImGui::ColorEdit3("object color", (float*)&m_ClearColor);
		ImGui::RadioButton("perspective camera", &isOrthoCamera,0);
		ImGui::SameLine();
		ImGui::RadioButton("ortho camera", &isOrthoCamera,1);
		ImGui::Checkbox("Enable Mouse Rotate", &canMouseRotate);
		ImGui::Checkbox("Enable Cube Rotate", &canSelfRotate);
		ImGui::Checkbox("Enable Rotate 45", &canRotate45);
		ImGui::SliderFloat("viewField", &m_viewField, 0.0f, 90.0f, "viewField = %.3f");
		ImGui::SliderFloat("x", &m_camPos.x, -15.0f, 15.0f, "camPos.x = %.3f");
		ImGui::SliderFloat("y", &m_camPos.y, -15.0f, 15.0f, "camPos.y = %.3f");
		ImGui::SliderFloat("z", &m_camPos.z, -15.0f, 15.0f, "camPos.z = %.3f");

		if (ImGui::Button("create model 1"))
		{
			solids = CreateModel1();
		}
		if (ImGui::Button("create model 2"))
		{
			solids = CreateModel2();
		}
		if (ImGui::Button("create model 3"))
		{
			solids = CreateModel3();
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
	}

	Solid* TestCadModel::CreateModel1()
	{
		
		Solid* newSolid;
		newSolid = euler::MVFS(glm::vec3(-2.0f, -2.0f, -2.0f));

		Loop* loop = newSolid->m_Faces->m_Loops;
		
		Vertex* v1 = newSolid->m_Vertices;
		
		HalfEdge* he1 = euler::MEV(loop,v1, glm::vec3(2.0f, -2.0f, -2.0f));
		Vertex* v2 = he1->m_EndV;
		
		HalfEdge* e2 = euler::MEV(loop,v2, glm::vec3(2.0f, 2.0f, -2.0f));
		Vertex* v3 = e2->m_EndV;
		
		HalfEdge* he3 = euler::MEV(loop,v3,glm::vec3(-2.0f, 2.0f, -2.0f));
		Vertex* v4 = he3->m_EndV;
		
		// Make out ring
		Loop* insideLoop = euler::MEF(loop,v4,v1);

		
		//Make first in loop
		HalfEdge* firstInLoopHe1 = euler::MEV(insideLoop, v1, glm::vec3(-1.0f, -1.0f, -2.0f));
		Vertex* inV1= firstInLoopHe1->m_EndV;

		
		Loop* inLoop1 = euler::KEMR(insideLoop, v1, inV1);

		HalfEdge* inHe1 = euler::MEV(inLoop1, inV1, glm::vec3(1.0f, -1.0f, -2.0f));
		Vertex* inV2 = inHe1->m_EndV;
		

		HalfEdge* inHe2 = euler::MEV(inLoop1, inV2, glm::vec3(1.0f, 1.0f, -2.0f));
		Vertex* inV3 = inHe2->m_EndV;
		

		HalfEdge* inHe3 = euler::MEV(inLoop1, inV3, glm::vec3(-1.0f, 1.0f, -2.0f));
		Vertex* inV4 = inHe3->m_EndV;
		

		Loop* innerLoop = euler::MEF(inLoop1, inV4, inV1);
		
		//ASSERT(insideLoop->m_NextL == insideLoop);

		euler::Sweep(loop->m_Face->m_NextF, glm::vec3(0.0f, 0.0f, 1.0f), 4.0f);
		Solid* news = euler::Sweep(loop->m_Face->m_NextF->m_NextF, glm::vec3(0.0f, 0.0f, 1.0f), 4.0f);
		euler::KFMRH(insideLoop, innerLoop);

		return news;

	}
	Solid* TestCadModel::CreateModel2()
	{

		Solid* newSolid;
		newSolid = euler::MVFS(glm::vec3(-4.0f, -4.0f, -2.0f));

		Loop* loop = newSolid->m_Faces->m_Loops;
		Vertex* v1 = newSolid->m_Vertices;
		HalfEdge* he1 = euler::MEV(loop, v1, glm::vec3(4.0f, -4.0f, -2.0f));
		Vertex* v2 = he1->m_EndV;

		HalfEdge* e2 = euler::MEV(loop, v2, glm::vec3(4.0f, 4.0f, -2.0f));
		Vertex* v3 = e2->m_EndV;

		HalfEdge* he3 = euler::MEV(loop, v3, glm::vec3(-4.0f, 4.0f, -2.0f));
		Vertex* v4 = he3->m_EndV;
		// Make out ring
		Loop* insideLoop = euler::MEF(loop, v4, v1);

		//Make first in loop
		HalfEdge* firstInLoopHe1 = euler::MEV(insideLoop, v1, glm::vec3(0.0f, 3.0f, -2.0f));
		Vertex* inV1 = firstInLoopHe1->m_EndV;

		Loop* inLoop1 = euler::KEMR(insideLoop, v1, inV1);

		HalfEdge* inHe1 = euler::MEV(inLoop1, inV1, glm::vec3( 0.31f,2.34f, -2.0f));
		Vertex* inV2 = inHe1->m_EndV;

		HalfEdge* inHe2 = euler::MEV(inLoop1, inV2, glm::vec3( 1.31f,  2.34f, -2.0f));
		Vertex* inV3 = inHe2->m_EndV;

		HalfEdge* inHe3 = euler::MEV(inLoop1, inV3, glm::vec3( 0.467f,  1.80f, -2.0f));
		Vertex* inV4 = inHe3->m_EndV;

		HalfEdge* inHe4 = euler::MEV(inLoop1, inV4, glm::vec3( 0.893f,  0.895f, -2.0f));
		Vertex* inV5 = inHe4->m_EndV;

		HalfEdge* inHe5 = euler::MEV(inLoop1, inV5, glm::vec3( 0.0f,  1.556f, -2.0f));
		Vertex* inV6 = inHe5->m_EndV;

		HalfEdge* inHe6 = euler::MEV(inLoop1, inV6, glm::vec3( -0.893f,  0.895f, -2.0f));
		Vertex* inV7 = inHe6->m_EndV;

		HalfEdge* inHe7 = euler::MEV(inLoop1, inV7, glm::vec3( -0.467f,  1.80f, -2.0f));
		Vertex* inV8 = inHe7->m_EndV;

		HalfEdge* inHe8 = euler::MEV(inLoop1, inV8, glm::vec3( -1.31f,  2.34f, -2.0f));
		Vertex* inV9 = inHe8->m_EndV;

		HalfEdge* inHe9 = euler::MEV(inLoop1, inV9, glm::vec3( -0.31f,  2.34f, -2.0f));
		Vertex* inV10 = inHe9->m_EndV;


		Loop* innerLoop = euler::MEF(inLoop1, inV10, inV1);

		//ASSERT(insideLoop->m_NextL == insideLoop);

		euler::Sweep(loop->m_Face->m_NextF, glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
		Solid* news = euler::Sweep(loop->m_Face->m_NextF->m_NextF, glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
		euler::KFMRH(insideLoop, innerLoop);

		return news;

	}
	Solid* TestCadModel::CreateModel3()
	{

		Solid* newSolid;
		newSolid = euler::MVFS(glm::vec3(-4.0f, -4.0f, -2.0f));

		Loop* loop = newSolid->m_Faces->m_Loops;
		Vertex* v1 = newSolid->m_Vertices;
		HalfEdge* he1 = euler::MEV(loop, v1, glm::vec3(4.0f, -4.0f, -2.0f));
		Vertex* v2 = he1->m_EndV;

		HalfEdge* e2 = euler::MEV(loop, v2, glm::vec3(4.0f, 4.0f, -2.0f));
		Vertex* v3 = e2->m_EndV;

		HalfEdge* he3 = euler::MEV(loop, v3, glm::vec3(-4.0f, 4.0f, -2.0f));
		Vertex* v4 = he3->m_EndV;
		// Make out ring
		Loop* insideLoop = euler::MEF(loop, v4, v1);

		//Make first in loop
		HalfEdge* firstInLoopHe1 = euler::MEV(insideLoop, v1, glm::vec3(0.0f, 3.0f, -2.0f));
		Vertex* inV1 = firstInLoopHe1->m_EndV;

		Loop* inLoop1 = euler::KEMR(insideLoop, v1, inV1);

		HalfEdge* inHe1 = euler::MEV(inLoop1, inV1, glm::vec3(0.31f, 2.34f, -2.0f));
		Vertex* inV2 = inHe1->m_EndV;

		HalfEdge* inHe2 = euler::MEV(inLoop1, inV2, glm::vec3(1.31f, 2.34f, -2.0f));
		Vertex* inV3 = inHe2->m_EndV;

		HalfEdge* inHe3 = euler::MEV(inLoop1, inV3, glm::vec3(0.467f, 1.80f, -2.0f));
		Vertex* inV4 = inHe3->m_EndV;

		HalfEdge* inHe4 = euler::MEV(inLoop1, inV4, glm::vec3(0.893f, 0.895f, -2.0f));
		Vertex* inV5 = inHe4->m_EndV;

		HalfEdge* inHe5 = euler::MEV(inLoop1, inV5, glm::vec3(0.0f, 1.556f, -2.0f));
		Vertex* inV6 = inHe5->m_EndV;

		HalfEdge* inHe6 = euler::MEV(inLoop1, inV6, glm::vec3(-0.893f, 0.895f, -2.0f));
		Vertex* inV7 = inHe6->m_EndV;

		HalfEdge* inHe7 = euler::MEV(inLoop1, inV7, glm::vec3(-0.467f, 1.80f, -2.0f));
		Vertex* inV8 = inHe7->m_EndV;

		HalfEdge* inHe8 = euler::MEV(inLoop1, inV8, glm::vec3(-1.31f, 2.34f, -2.0f));
		Vertex* inV9 = inHe8->m_EndV;

		HalfEdge* inHe9 = euler::MEV(inLoop1, inV9, glm::vec3(-0.31f, 2.34f, -2.0f));
		Vertex* inV10 = inHe9->m_EndV;

		Loop* innerLoop = euler::MEF(inLoop1, inV10, inV1);

		//second innerloop
		HalfEdge* secondInLoopHe1 = euler::MEV(insideLoop, v1, glm::vec3(-1.0f, -3.0f, -2.0f));
		Vertex* in2V1 = secondInLoopHe1->m_EndV;


		Loop* inLoop2 = euler::KEMR(insideLoop, v1, in2V1);

		HalfEdge* in2He1 = euler::MEV(inLoop2, in2V1, glm::vec3(1.0f, -3.0f, -2.0f));
		Vertex* in2V2 = in2He1->m_EndV;


		HalfEdge* in2He2 = euler::MEV(inLoop2, in2V2, glm::vec3(1.0f, -1.0f, -2.0f));
		Vertex* in2V3 = in2He2->m_EndV;


		HalfEdge* in2He3 = euler::MEV(inLoop2, in2V3, glm::vec3(-1.0f, -1.0f, -2.0f));
		Vertex* in2V4 = in2He3->m_EndV;


		Loop* innerLoop2 = euler::MEF(inLoop2, in2V4, in2V1);

		euler::Sweep(loop->m_Face->m_NextF, glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
		euler::Sweep(loop->m_Face->m_NextF->m_NextF, glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
		Solid* news = euler::Sweep(loop->m_Face->m_NextF->m_NextF->m_NextF, glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
		euler::KFMRH(insideLoop, innerLoop);
		euler::KFMRH(insideLoop, innerLoop2);
		return news;

	}

	std::vector<Vertex*> getVertexList(Solid* s)
	{
		std::vector<Vertex*> points;
		Vertex* v = s->m_Vertices;
		while (v != nullptr)
		{
			points.push_back(v);
			v = v->m_NextV;
		}
		return points;
	}

	float* getVertexBufferData(std::vector<Vertex*> points,int& size)
	{
		float* pos = new float[points.size() * 6];
		glm::vec3 colorSet[8] = { { 0.583f, 0.700f, 0.014f },
					              { 0.609f, 0.100f, 0.436f },
		                          { 0.327f, 0.400f, 0.844f },
				                  { 0.822f, 0.500f, 0.201f },
				                  { 0.435f, 0.600f, 0.223f },
				                  { 0.310f, 0.700f, 0.185f },
				                  { 0.597f, 0.700f, 0.761f },
				                  { 0.559f, 0.400f, 0.730f }

		};

		int colorNumber = 0;
		for (auto it = points.begin(); it != points.end(); it++)
		{
			int colorType = colorNumber % 8;
			int index=((*it)->m_Id-1)*6;
			pos[index] = (*it)->m_Position.x;
			pos[index+1] = (*it)->m_Position.y;
			pos[index+2] = (*it)->m_Position.z;
			pos[index+3] = colorSet[colorType].x;
		    pos[index+4] = colorSet[colorType].y;
			pos[index+5] = colorSet[colorType].z;
			colorNumber++;
		}

		size = points.size() * 6;
		return pos;
	}


	unsigned int* getIndexBufferData(Solid* s,unsigned int &indicesNumber)
	{
		

		std::vector<triangulation::TriangleIntices> triList;//get all face's triangulation into one list
		int size = 0;
		Face* f = s->m_Faces;
		while (f != nullptr)
		{
			std::vector<triangulation::TriangleIntices> tempTriList;
			int size = f->getAndSetVerticesNumber();
			
			tempTriList = triangulation::MyTrangulation(f, size);//every face's triangulation list
			for (triangulation::TriangleIntices temp : tempTriList)
			{
				triList.push_back(temp);
				temp.print();
			}

			
			f = f->m_NextF;
		}
		
		unsigned int* indexBufferData = new unsigned int[triList.size() * 3];
		int index = 0;
		
		for (triangulation::TriangleIntices tri : triList)
		{
			indexBufferData[index] = (tri.vertex3[0]->m_Id-1);
			indexBufferData[index+1] =  (tri.vertex3[1]->m_Id-1);
			indexBufferData[index+2] =  (tri.vertex3[2]->m_Id-1);
			index += 3;
		}

		indicesNumber = triList.size() * 3;
		
		return indexBufferData;
	}


}