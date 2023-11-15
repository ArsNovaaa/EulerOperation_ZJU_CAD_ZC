#pragma once

#include "Test.h"
#include"structure/CanvasPoint.h"
#include "Texture.h"
#include<vector>
#include <memory>
#include"imgui.h"

namespace test {

	class TestDataFitting : public Test
	{
	public:
		TestDataFitting();
		~TestDataFitting();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		std::vector<ImVec2> LagrangeResults;
		std::vector<ImVec2> GaussResults;
		std::vector<ImVec2> LeastSquaresResults;
		std::vector<ImVec2> RidgeRegressionResults;
		std::vector<CanvasPoint> m_DataPoints;
		bool opt_enable_grid{ true };
		bool opt_enable_context_menu{ true };
		bool opt_lagrange{ true };
		bool opt_gauss{ false };
		bool opt_least_squares{ false };
		bool opt_ridge_regression{ false };
		int LeastSquaresM = 4;
		float RidgeRegressionLamda = 0.1;
		float scrolling[2] = {0.0f,0.0f};

	
	};

}
