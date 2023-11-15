#pragma once

#include"imgui.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include <memory>
#include"structure/HalfEdge.h"
namespace euler {




		Solid* MVFS(glm::vec3 pos);								//make vertex face solid
		HalfEdge* MEV(Loop* lp, Vertex* sv, glm::vec3 pos);	// make edge vertex
		Loop* MEF(Loop* lp, Vertex* v1, Vertex* v2);		// make edge face
		Loop* KEMR(Loop* lp, Vertex* v1, Vertex* v2); //make edge kill ring
		void KFMRH(Loop* outerLoop, Loop* innerLoop);				// make face kill ring and hole
		Solid* Sweep(Face* f, glm::vec3 direction,float distance);
		void InfoSolid(Solid* solid);
		void InfoFace(Face* face);
		

}