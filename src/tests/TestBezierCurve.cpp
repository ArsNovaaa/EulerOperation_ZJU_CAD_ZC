#include "TestBezierCurve.h"
#include"imgui.h"
double mouse_X = 0;
double mouse_Y = 0;
bool if_change_ctrl = 0;
namespace test
{
	TestBezierCurve::TestBezierCurve(GLFWwindow* const window)
		:m_window(window)
	{
		
	}
	TestBezierCurve::~TestBezierCurve()
	{

	}
	void TestBezierCurve::OnUpdate(float deltaTime)
	{

	}

	void TestBezierCurve::OnRender()
	{
	
	
	}
	void TestBezierCurve::OnImGuiRender()
	{
		
	
	}
	
	void mouse_callback(GLFWwindow* window, double x, double y)
	{
		
		mouse_X = x;
		mouse_Y = y;
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{

	}
}




