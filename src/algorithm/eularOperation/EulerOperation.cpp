#include "EulerOperation.h"
#include "structure/HalfEdge.h"
#include<iostream>

#pragma once
namespace euler {
	

	Solid* MVFS(glm::vec3 pos)
	{
		Solid* solid = new Solid();
		Face* face = new Face();
		Vertex* vertex = new Vertex(pos);
		Loop* loop = new Loop();

		solid->addVertexToSolid(vertex);
		solid->addFaceToSolid(face);
		face->addLoopToFace(loop);
		
		return solid;
	}

	HalfEdge* MEV(Loop* lp, Vertex* sv, glm::vec3 pos)
	{
		Solid* solid = lp->m_Face->m_Solid;
	    HalfEdge* lhe, *rhe, *temph;
	    Edge* newEdge;
	    Vertex* ev;

	//  whether the start vertex is in the solid


	    lhe = new HalfEdge();
	    rhe = new HalfEdge();
	    newEdge = new Edge();
	    ev = new Vertex(pos);

	    newEdge->m_LeftH = lhe;
	    newEdge->m_RightH = rhe;
	    lhe->m_Edge = rhe->m_Edge = newEdge;

	    lhe->m_StartV = sv;
	    lhe->m_EndV = ev;

	    rhe->m_StartV = ev;
	    rhe->m_EndV = sv;

		lhe->m_OppositeH = rhe;
		rhe->m_OppositeH = lhe;
		
		lhe->m_Loop = lp;
		rhe->m_Loop = lp;

	    if (lp->m_HalfEdges == NULL)
	    {
			lhe->m_NextH = rhe;
			rhe->m_NextH = lhe;

			lhe->m_PreH = rhe;
			rhe->m_PreH = lhe;
			lp->m_HalfEdges = lhe;
	    }
	    else
	    {
		// insert the new edge -- 2 halfedges
			 temph = lp->m_HalfEdges;
			
			while (temph->m_EndV != sv)
				temph = temph->m_NextH;

				rhe->m_NextH = temph->m_NextH;
				temph->m_NextH->m_PreH = rhe;
				temph->m_NextH = lhe;
				lhe->m_PreH = temph;
				lhe->m_NextH = rhe;
				rhe->m_PreH = lhe;

	    }
		lhe->m_Loop = lp;
		rhe->m_Loop = lp;

		solid->addVertexToSolid(ev);
		solid->addEdgeToSolid(newEdge);

	    return lhe;
	}

	Loop* MEF(Loop* lp, Vertex* v1,Vertex* v2)
	{
		
		Loop* tempLoop = lp;
		// create physical edge and two half edge
		Edge* newEdge = new Edge();
		HalfEdge* lhe = new HalfEdge();
		HalfEdge* rhe= new HalfEdge();

		// ctreate relationship between edge and halfedge
		newEdge->m_LeftH = lhe;
		newEdge->m_RightH = rhe;
		lhe->m_Edge = newEdge;
		rhe->m_Edge = newEdge;
		lhe->m_OppositeH = rhe;
		rhe->m_OppositeH = lhe;

		
		lhe->m_StartV = v1;
		lhe->m_EndV = v2;
		rhe->m_StartV = v2;
		rhe->m_EndV = v1;

		HalfEdge* temph;
		HalfEdge* starth;
		
		starth = temph = lp->m_HalfEdges;
		while (temph->m_StartV != v1) {
			temph = temph->m_NextH;
			if (temph == starth) {
				std::cout << "Error : EulerOperation::mef cannot find v1  " <<std:: endl;
				return nullptr;
			}
		}
		HalfEdge* firsth = temph;

		bool visited = false;
		starth = temph = lp->m_HalfEdges;
		while (temph->m_StartV != v2) {
			temph = temph->m_NextH;
			if (temph == starth) {
				std::cout << "Error : EulerOperation::mef cannot find vertex_2  " <<std:: endl;
				return nullptr;
			}
			if (temph->m_StartV == v2 ) {
				visited = true;
			}
		}
		HalfEdge* secondh = temph;

		// simple case incomplete the opposite edge is not used
		rhe->m_NextH = firsth;
		rhe->m_PreH = secondh->m_PreH;
		lhe->m_NextH = secondh;
		lhe->m_PreH = firsth->m_PreH;
		firsth->m_PreH->m_NextH = lhe;
		firsth->m_PreH = rhe;
		secondh->m_PreH->m_NextH = rhe;
		secondh->m_PreH = lhe;

		// two loops ,one become outloop and another become innerloop
		lp->m_HalfEdges = lhe;
		Loop* newLoop = new Loop();
		newLoop->m_HalfEdges = rhe;


		// create the relationship of newface newloop and oldsolid
		Solid* oldSolid = lp->m_Face->m_Solid;
		Face* newFace = new Face();
		
		newLoop->m_Face = newFace;
		newFace->m_Solid = oldSolid;

		
		oldSolid->addEdgeToSolid(newEdge);
		oldSolid->addFaceToSolid(newFace);
		newFace->addLoopToFace(newLoop);

		return newLoop;

		
	}

	Loop * KEMR(Loop* lp, Vertex* v1, Vertex* v2)
	{
		
		Loop* tempLoop = lp;
		// find the physical edge (v1 to v2)and its halfedge 
		HalfEdge* temph;
		for (temph = lp->m_HalfEdges; !(v1 == temph->m_StartV && v2 == temph->m_EndV); temph = temph->m_NextH) {}
		Edge* edge = temph->m_Edge;

		// reconstruct the halfedge relationship
		temph->m_NextH->m_PreH = temph->m_OppositeH->m_PreH;
		temph->m_OppositeH->m_PreH->m_NextH = temph->m_NextH;
		temph->m_PreH->m_NextH = temph->m_OppositeH->m_NextH;
		temph->m_OppositeH->m_NextH->m_PreH = temph->m_PreH;

		// split two loops 
		lp->m_HalfEdges = temph->m_PreH;
		Loop* newLoop = new Loop();
		newLoop->m_HalfEdges = temph->m_NextH;
		if (temph->m_NextH == temph->m_OppositeH)
		{
			newLoop->m_HalfEdges = nullptr;
			
		}

		// bind newloop and face
		Face* oldFace = lp->m_HalfEdges->m_OppositeH->m_Loop->m_Face;
		newLoop->SetInnerLoop(true);
		newLoop->m_Face = oldFace;
		oldFace->addLoopToFace(newLoop);

	

		return newLoop;
	}

	
	void KFMRH(Loop* outter_loop, Loop* inner_loop)
	{
		Face* face_1 = outter_loop->m_Face;
		Face* face_2 = inner_loop->m_Face;

		Loop* lp;
		for (lp = face_1->m_Loops; nullptr != lp->m_NextL; lp = lp->m_NextL)
		{
		}
		lp->m_NextL = inner_loop;
		inner_loop->m_PreL = lp;
		inner_loop->m_Face= face_1;

		inner_loop->isInnerLoop = true;

		Solid* oldSolid = face_1->m_Solid;
		Face* f = oldSolid->m_Faces;
		oldSolid->m_FaceNumber--;
		if (f == face_2)
		{
			oldSolid->m_Faces = f->m_NextF;
			oldSolid->m_Faces->m_PreF = f->m_PreF;
		}

		else
		{
			while (f->m_NextF != face_2)
			{
				f = f->m_NextF;
			}
			f->m_NextF= face_2->m_NextF;
			f->m_NextF->m_PreF= f;
		}
		delete face_2;

		return;
	}
		
	

	Solid* Sweep(Face* face, glm::vec3 direction, float distance)
	{
		Solid* solid = face->m_Solid;
		Loop* loop;
		HalfEdge* he;
		for (loop = face->m_Loops; loop != nullptr; loop = loop->m_NextL)
		{
			he = loop->m_HalfEdges;
			Vertex* startV = he->m_StartV;
			glm::vec3 newPosition = startV->m_Position + distance * direction;
			

			HalfEdge* firstConnectHe = MEV( loop,startV, newPosition);
			Vertex* upVertex_1 = firstConnectHe->m_EndV;

			he = he->m_NextH;
			Vertex* v = he->m_StartV;
			while (v != startV)
			{
				glm::vec3 newPosition = v->m_Position + distance * direction;
				

				HalfEdge* connectHe = MEV( loop,v, newPosition);
				Vertex* upVertex_2 = connectHe->m_EndV;
				MEF( loop, upVertex_1, upVertex_2);
				upVertex_1 = upVertex_2;
				he = he->m_NextH;
				v = he->m_StartV;
			}
			MEF(loop, upVertex_1,firstConnectHe->m_EndV);
		}
		return solid;
	}

	void InfoSolid(Solid* solid)
	{
		ImGui::Begin("Log System");
		ImGui::Text("---------------Solid Info---------------");
		ImGui::Text(  "Face number: %1i", solid->m_FaceNumber );
		Face* f = solid->m_Faces;
		Face* startf = solid->m_Faces;
		while (f!= nullptr)
		{
			ImGui::Text("---------------Face ID %1i ---------------", f->m_Id);
			InfoFace(f);
			f=f->m_NextF;
			if (f == startf)
				break;

		}

		ImGui::Text("Vertex number: %1i", solid->m_VertexNumber);
		ImGui::End();
	}

	void InfoFace(Face* face)
	{
		Loop* lp = face->m_Loops;
		int loopNum = 1;
		while (lp!=nullptr)
		{
			ImGui::Text("---Loop%1i---", loopNum);
			HalfEdge* he = lp->m_HalfEdges;
			Vertex* start = he->m_StartV;
			ImGui::Text("Vertex %1i---", start->m_Id);
			he = he->m_NextH;
			while (he->m_StartV!=start)
			{
				ImGui::Text("Vertex %1i ---", he->m_StartV->m_Id);
				he= he->m_NextH;
			}
			std::cout << std::endl;
			lp = lp->m_NextL;
			loopNum++;
		}
	
	}

	

}
