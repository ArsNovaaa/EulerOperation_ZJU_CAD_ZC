#include "HalfEdge.h"
#include<iostream>

Vertex::Vertex()
{
}

Vertex::Vertex(glm::vec3 pos = glm::vec3(0, 0, 0)):m_Id(0),m_Position(pos),m_HalfEdge(nullptr),m_NextV(nullptr),
m_PreV(nullptr)
{
	
}


Vertex::~Vertex()
{
	std::cout << "Vertex" << m_Id << " has been deleted!" << std::endl;
}

glm::vec3 Vertex::operator-(Vertex v)
{
	glm::vec3 result(0.0f,0.0f,0.0f);
	result.x = m_Position.x - v.m_Position.x;
	result.y = m_Position.y - v.m_Position.y;
	result.z = m_Position.z - v.m_Position.z;
	return result;
}


HalfEdge::HalfEdge():m_Edge(nullptr), m_StartV(nullptr), m_EndV(nullptr), m_Loop(nullptr),
m_PreH(nullptr), m_NextH(nullptr), m_OppositeH(nullptr)
{
}

HalfEdge::~HalfEdge()
{
	
}

Loop::Loop() :m_Id(0), m_Face(nullptr), m_HalfEdges(nullptr),m_NextL(nullptr),m_PreL(nullptr),isInnerLoop(false)
{
}

Loop::~Loop()
{
	std::cout << "Loop" << m_Id << " has been deleted!" << std::endl;
}
Edge::Edge():m_LeftH(nullptr), m_NextE(nullptr), m_PreE(nullptr), m_RightH(nullptr)
{

}
Edge::~Edge()
{

}

Face::Face():m_Id(0), m_Loops(nullptr), m_NextF(nullptr), m_PreF(nullptr),
m_Solid(nullptr)
{

}

Face::~Face()
{
	std::cout << "Face" << m_Id << " has been deleted!" << std::endl;
}

Solid::Solid():m_Id(0),m_FaceNumber(0),m_LoopNumber(0),m_VertexNumber(0),m_Edges(nullptr),
m_Vertices(nullptr),m_Faces(nullptr),m_NextS(nullptr),m_PreS(nullptr),m_EdgeNumber(0)
{

}

Solid::~Solid()
{
	std::cout << "Solid" << m_Id << " has been deleted!" << std::endl;
}

bool Solid::addVertexToSolid(Vertex* v)
{
	if (v == NULL)
	{
		std::cout << "Error: No Valid Vertex Pointer !" << std::endl;
		return false;
	}
	if (m_Vertices == nullptr)
	{
		m_Vertices = v;
		m_VertexNumber++;
		v->m_Id = m_VertexNumber;
		return true;
	}

	Vertex* tempv = m_Vertices;
	while (tempv)
	{
		if (tempv->m_NextV== NULL)
			break;
		tempv = tempv->m_NextV;
	}
	tempv->m_NextV = v;
	v->m_PreV = tempv;
	m_VertexNumber++;
	v->m_Id = m_VertexNumber;
	return true;
	
}

bool Solid::addEdgeToSolid(Edge* eg)
{


	if (eg == NULL)
	{
		std::cout << "Error: No Valid Edge Pointer ! " << std::endl;
		return false;
	}

	if (m_Edges == NULL)
	{
		m_Edges = eg;
		m_EdgeNumber++;
		return true;
	}

	Edge* tempeg = m_Edges;
	while (tempeg)
	{
		if (tempeg->m_NextE == NULL)
			break;
		tempeg = tempeg->m_NextE;
	}

	tempeg->m_NextE = eg;
	eg->m_PreE = tempeg;
	m_EdgeNumber++;
	return true;
}

bool Solid::addFaceToSolid(Face* face)
{
	if (face == NULL)
	{
		std::cout << "Error: No Valid Face Pointer ! " << std::endl;
		return false;
	}

	if (m_Faces == NULL)
	{
		m_Faces = face;
		face->m_Solid= this;
		m_FaceNumber++;
		face->m_Id = m_FaceNumber;
		return true;
	}

	Face* tempf = m_Faces;
	while (tempf)
	{
		if (tempf->m_NextF == NULL)
			break;
		tempf = tempf->m_NextF;
	}

	tempf->m_NextF = face;
	face->m_PreF = tempf;
	face->m_Solid = this;
	m_FaceNumber++;
	face->m_Id = m_FaceNumber;
	return true;

}

bool Face::addLoopToFace(Loop* lp)
{
	if (lp == NULL)
	{
		std::cout << "Error: No Valid Loop Pointer ! " << std::endl;
		return false;
	}

	if (m_Loops == NULL)
	{
		m_Loops = lp;
		lp->m_Face = this;
		return true;
	}
	else
	{
		Loop* templp = m_Loops;
		while (templp)
		{
			if (templp->m_NextL== nullptr)
				break;
			templp = templp->m_NextL;
		}

		templp->m_NextL = lp;
		lp->m_PreL = templp;
	}

	lp->m_Face = this;
	return true;
}

int Face::getAndSetVerticesNumber()
{
	Loop* lp = this->m_Loops;
	while (lp != nullptr)
	{
		HalfEdge* he = lp->m_HalfEdges;
		Vertex* startV = he->m_StartV;
		he = he->m_NextH;
		this->m_VerticesNumber++;
		while (he->m_StartV != startV)
		{
			this->m_VerticesNumber++;
			he = he->m_NextH;
		}
		lp = lp->m_NextL;
	}
	return this->m_VerticesNumber;
}
