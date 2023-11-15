#pragma once
#include"glm/glm.hpp"

class Solid;
class Face;
class Loop;
class HalfEdge;
class Vertex;
class Edge;

class Vertex{
public:
	unsigned int m_Id;
	glm::vec3 m_Position;
	HalfEdge* m_HalfEdge;
	Vertex* m_PreV;
	Vertex* m_NextV;
	Vertex();
	Vertex(glm::vec3 pos);
	~Vertex();
	glm::vec3 operator -(Vertex v);
	
};

class HalfEdge {
public:
	Edge* m_Edge; //which edge belongs to
	Vertex* m_StartV;
	Vertex* m_EndV;
	Loop* m_Loop;
	HalfEdge* m_PreH;
	HalfEdge* m_NextH;
	HalfEdge* m_OppositeH;

	HalfEdge();
	~HalfEdge();


};

class Loop {
public:
	int m_Id;
	bool isInnerLoop;
	HalfEdge* m_HalfEdges;//all the halfedges who consists current loop
	Face* m_Face;
	Loop* m_PreL;
	Loop* m_NextL;

	Loop() ;
	~Loop() ;

	inline void SetInnerLoop(bool _state) {
		isInnerLoop = _state;
	}

	
};

class Edge {
public:
	HalfEdge* m_LeftH;
	HalfEdge* m_RightH;
	Edge* m_PreE;
	Edge* m_NextE;

	Edge() ;
	~Edge() ;


	
};

class Face {
public:
	int m_Id;
	Solid* m_Solid; //which solid belongs to
	Loop* m_Loops;
	Face* m_PreF;
	Face* m_NextF;
	int m_VerticesNumber = 0;
	Face() ;
	~Face() ;
	bool addLoopToFace(Loop* lp);
	int getAndSetVerticesNumber();

};

class Solid {
public:
	int m_Id;
	Face* m_Faces; //all the faces who consists current solid
	Edge* m_Edges; //all the edges who consists current solid
	Vertex* m_Vertices;//all the vertices who consists current solid
	Solid* m_PreS;
	Solid* m_NextS;
	int m_VertexNumber;
	int m_FaceNumber;
	int m_LoopNumber;
	int m_EdgeNumber;

	Solid() ;
	~Solid() ;
	bool addVertexToSolid(Vertex* v);		
	bool addEdgeToSolid(Edge* eg);			
	bool addFaceToSolid(Face* face);	


};