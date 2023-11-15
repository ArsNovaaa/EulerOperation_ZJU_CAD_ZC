#include "TestDataFitting.h"
#include "glm/glm.hpp"
#include"structure/CanvasPoint.h"
#include"imgui.h"
#include"Eigen/eigen"

float PAI(const std::vector<CanvasPoint>& P, int j, float x)
{
	int n = P.size();
	float numerator = 1;
	float denominator = 1;
	for (int k = 0; k < n; ++k)
	{
		if (j == k)
		{
			continue;
		}
		numerator *= (x - P[k].x);
		denominator *= (P[j].x - P[k].x);
	}
	return numerator / denominator;
}

float Polynomial(const std::vector<CanvasPoint>& P, float x)
{
	int n = P.size();
	float sum = 0;
	for (int j = 0; j < n; ++j)
	{
		sum += P[j].y * PAI(P, j, x);
	}
	return sum;
}

float Gauss(const std::vector<CanvasPoint>& P, float x)
{
	int n = P.size();
	if (n == 0)
	{
		return 0;
	}
	float theta = 100;
	Eigen::MatrixXf A(n, n);
	for (int row = 0; row < n; ++row)
	{
		for (int col = 0; col < n; ++col)
		{
			A(row, col) = (std::exp(-(P[row].x - P[col].x) * (P[row].x - P[col].x) / (2 * theta * theta)));
		}
	}
	Eigen::VectorXf b(n);
	for (int i = 0; i < n; ++i)
	{
		b(i) = P[i].y;
	}
	Eigen::VectorXf a = A.colPivHouseholderQr().solve(b);
	float result = 0;
	for (int j = 0; j < n; ++j)
	{
		result += a[j] * (std::exp(-(x - P[j].x) * (x - P[j].x) / (2 * theta * theta)));
	}
	return result;
}

float LeastSquares(const std::vector<CanvasPoint>& P, float x, int m)
{
	int n = P.size();
	if (n == 0)
	{
		return 0;
	}
	if (m >= n)
	{
		m = n - 1;
	}
	Eigen::MatrixXf X(n, m);
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			X(i, j) = std::powf(P[i].x, j);
		}
	}
	Eigen::VectorXf Y(n);
	for (int i = 0; i < n; ++i)
	{
		Y(i) = P[i].y;
	}
	Eigen::VectorXf Theta = (X.transpose() * X).inverse() * X.transpose() * Y;
	//Eigen::VectorXf Theta = X.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(Y);
	float result = 0;
	for (int j = 0; j < m; ++j)
	{
		result += Theta[j] * std::powf(x, j);
	}
	return result;
}

float RidgetRegression(const std::vector<CanvasPoint>& P, float x, float lamda, int m)
{
	int n = P.size();
	if (n == 0)
	{
		return 0;
	}
	if (m >= n)
	{
		m = n - 1;
	}
	Eigen::MatrixXf X(n, m);
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			X(i, j) = std::powf(P[i].x, j);
		}
	}
	Eigen::VectorXf Y(n);
	for (int i = 0; i < n; ++i)
	{
		Y(i) = P[i].y;
	}
	Eigen::MatrixXf I(m, m);
	I.setIdentity();
	Eigen::VectorXf Theta = (X.transpose() * X + I * lamda).inverse() * X.transpose() * Y;
	float result = 0;
	for (int j = 0; j < m; ++j)
	{
		result += Theta[j] * std::powf(x, j);
	}
	return result;
}

namespace test {

	TestDataFitting::TestDataFitting()
	{
	}

	TestDataFitting::~TestDataFitting()
	{
	}

	void TestDataFitting::OnUpdate(float deltaTime)
	{
	}

	void TestDataFitting::OnRender()
	{
	}

	void TestDataFitting::OnImGuiRender()
	{
		ImGui::Checkbox("Enable grid", &opt_enable_grid);
		ImGui::Checkbox("Enable context menu", &opt_enable_context_menu);
		ImGui::Text("Mouse Left: click to add points,\nMouse Right: drag to scroll, click for context menu.");

		ImGui::Checkbox("Lagrange", &opt_lagrange);
		ImGui::Checkbox("Gauss", &opt_gauss);

		ImGui::Checkbox("LeastSquares", &opt_least_squares);
		ImGui::SameLine(200);
		ImGui::InputInt("m", &LeastSquaresM);

		ImGui::Checkbox("RidgetRegression", &opt_ridge_regression);
		ImGui::SameLine(200);
		ImGui::InputFloat("lamda", &RidgeRegressionLamda, 0.01, 1);

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Draw border and background color
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
		draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

		// This will catch our interactions
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held
		const ImVec2 origin(canvas_p0.x + scrolling[0], canvas_p0.y + scrolling[1]); // Lock scrolled origin
		const CanvasPoint mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

		// Add first and second point
		if (is_hovered  && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			m_DataPoints.push_back(mouse_pos_in_canvas);

			float Xmin = 999999;
			float Xmax = -99999;
			for (size_t i = 0; i < m_DataPoints.size(); ++i)
			{
				if (m_DataPoints[i].x < Xmin)
				{
					Xmin = m_DataPoints[i].x;
				}
				if (m_DataPoints[i].x > Xmax)
				{
					Xmax = m_DataPoints[i].x;
				}
			}
			LagrangeResults.clear();
			GaussResults.clear();
			LeastSquaresResults.clear();
			RidgeRegressionResults.clear();
			for (int x = Xmin - 1; x < Xmax + 2; ++x)
			{
				LagrangeResults.push_back(ImVec2(origin.x + x, origin.y + Polynomial(m_DataPoints, x)));
				GaussResults.push_back(ImVec2(origin.x + x, origin.y + Gauss(m_DataPoints, x)));
				LeastSquaresResults.push_back(ImVec2(origin.x + x, origin.y + LeastSquares(m_DataPoints, x, LeastSquaresM)));
				RidgeRegressionResults.push_back(ImVec2(origin.x + x, origin.y + RidgetRegression(m_DataPoints, x, RidgeRegressionLamda, LeastSquaresM)));
			}
		}
		const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
		// Context menu (under default mouse threshold)
		ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
		if (opt_enable_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
			ImGui::OpenPopupOnItemClick("context");
		if (ImGui::BeginPopup("context"))
		{
	
			if (ImGui::MenuItem("Remove one", NULL, false, m_DataPoints.size() > 0)) { m_DataPoints.resize(m_DataPoints.size() - 1); }
			if (ImGui::MenuItem("Remove all", NULL, false, m_DataPoints.size() > 0)) { m_DataPoints.clear(); }
			ImGui::EndPopup();
		}

		// Draw grid + all lines in the canvas
		draw_list->PushClipRect(canvas_p0, canvas_p1, true);
		if (opt_enable_grid)
		{
			const float GRID_STEP = 64.0f;
			for (float x = fmodf(scrolling[0], GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
				draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
			for (float y = fmodf(scrolling[1], GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
				draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
		}
		for (int n = 0; n < m_DataPoints.size(); n++)
		{
			draw_list->AddCircleFilled(ImVec2(origin.x + m_DataPoints[n].x, origin.y + m_DataPoints[n].y), 4.0f, IM_COL32(255, 255, 0, 255));
		}
		
		if (opt_lagrange)
		{
			draw_list->AddPolyline(LagrangeResults.data(), LagrangeResults.size(), IM_COL32(64, 128, 255, 255), false, 1.0f);
		}
		if (opt_gauss)
		{
			draw_list->AddPolyline(GaussResults.data(), GaussResults.size(), IM_COL32(128, 255, 255, 255), false, 1.0f);
		}
		if (opt_least_squares)
		{
			draw_list->AddPolyline(LeastSquaresResults.data(), LeastSquaresResults.size(), IM_COL32(255, 128, 128, 255), false, 1.0f);
		}
		if (opt_ridge_regression)
		{
			draw_list->AddPolyline(RidgeRegressionResults.data(), RidgeRegressionResults.size(), IM_COL32(255, 64, 64, 255), false, 1.0f);
		}

		draw_list->PopClipRect();
	}


	
}







	
	

