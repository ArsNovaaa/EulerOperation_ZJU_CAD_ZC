
#include"Triangulation.h"
#include<Windows.h>

using Point = std::array<float, 2>;

namespace triangulation
{
	const float epsilon = 1e-6;//the allowed error range
	bool IsPointInTriangle(Vertex v, Vertex tv1, Vertex tv2, Vertex tv3)
	{
		bool flag = true;
		glm::vec3 e1 = tv1 - v;
		glm::vec3 e2 = tv2 - v;
		glm::vec3 e3 = tv3 - v;
		float r1 = CrossProduct(e1, e2);
		float r2 = CrossProduct(e2, e3);
		float r3 = CrossProduct(e3, e1);

		if ((r1 >= 0 && r2 >= 0 && r3 >= 0) || (r1 <= 0 && r2 <= 0 && r3 <= 0))
			return flag;
		else
			flag = false;

		return flag;
	}

	float CrossProduct(glm::vec3 vecter1, glm::vec3 vector2)
	{
		return (vecter1.y * vector2.z - vecter1.z * vector2.y) - (vecter1.x * vector2.z - vecter1.z * vector2.x) + (vecter1.x * vector2.y - vecter1.y * vector2.x);
			
	}
	
	std::vector<TriangleIntices> MyTrangulation(Face* f, int size)
	{
		
		//get all triangles element list with three vertex information 
		std::vector<TriangleIntices> triList; 
		//different case
		if (size <= 2)
			return triList;

		if (size == 3)
		{
			HalfEdge* he = f->m_Loops->m_HalfEdges;
			Vertex* v1 = he->m_StartV;
			Vertex* v2 = he->m_NextH->m_StartV;
			Vertex* v3 = he->m_NextH->m_NextH->m_StartV;

			TriangleIntices tri = TriangleIntices(v1, v2, v3);
			triList.push_back(tri);
			return triList;
		}

		if (size == 4)
		{
			HalfEdge* he = f->m_Loops->m_HalfEdges;
			Vertex* v1 = he->m_StartV;
			Vertex* v2 = he->m_NextH->m_StartV;
			Vertex* v3 = he->m_NextH->m_NextH->m_StartV;
			Vertex* v4 = he->m_NextH->m_NextH->m_NextH->m_StartV;
			
			TriangleIntices tri1 = TriangleIntices(v1, v2, v3);
			TriangleIntices tri2 = TriangleIntices(v4, v3, v1);
			triList.push_back(tri1);
			triList.push_back(tri2);
			
			return triList;
		}

		if (size >= 5)
		{
			std::vector<VPMapping> vpMappingList; //the mapping relationship of topological vertices that on the same plane and its geometry information(3d and 2d  )
			Loop* lp = f->m_Loops;
			std::vector<Vertex*> verticesList;
			HalfEdge* he = lp->m_HalfEdges;
			Vertex * startV = he->m_StartV;
			verticesList.push_back(startV);
			he = he->m_NextH;

			while (he->m_StartV!=startV)
			{
				verticesList.push_back(he->m_StartV);
				he = he->m_NextH;
			}
			//no innerloop
			if (lp->m_NextL == nullptr)
			{   
				
				for (Vertex *v : verticesList)
				{
					VPMapping vpmap;
					vpmap.v = v;
					vpmap.p3d = v->m_Position;
					vpMappingList.push_back(vpmap);
				}

				Eigen::Matrix4f mat= TranslationMatrixTo2d(vpMappingList[0].p3d, vpMappingList[1].p3d, vpMappingList[2].p3d);
				
				for (VPMapping vpm: vpMappingList)
				{
					glm::vec2 p2d = Translate2d(vpm.p3d,mat);
					vpm.p2d = p2d;
				}

				int index = 0;
				std::vector<std::vector<Point>> polygon;
				std::vector<Point> outerPoints;
				std::unordered_map<int, Vertex*> map;

				for (auto it = vpMappingList.begin(); it != vpMappingList.end(); it++)
				{
					Point p = { it->p2d.x ,it->p2d.y };
					outerPoints.push_back(p);
					map[index / 2] = it->v;
					index += 2;
				}

				polygon.push_back(outerPoints);
				std::vector<int> result = mapbox::earcut<int>(polygon);

				for (int i = 0; i+2 < result.size(); i += 3)
				{
					Vertex* v1, *v2,* v3;
					v1 = map[result[i]];
					v2 = map[result[i+1]];
					v3 = map[result[i+2]];
					TriangleIntices tri = TriangleIntices(v1, v2, v3);
					triList.push_back(tri);
					
				}

				return triList;
			}
			//has innerloop
			else
			{
				
				lp = lp->m_NextL;
				int innerLoopNumber = 0;
				int outerVerticesNumber = verticesList.size();
				std::vector<int> innerLoopStartIndex;
				innerLoopStartIndex.push_back(outerVerticesNumber);

				while (lp!= nullptr)
				{
					HalfEdge* he = lp->m_HalfEdges;
					Vertex* startV = he->m_StartV;
					verticesList.push_back(startV);
					he = he->m_NextH;

					while (he->m_StartV != startV)
					{
						verticesList.push_back(he->m_StartV);
						he = he->m_NextH;
					}

					lp = lp->m_NextL;
					innerLoopNumber++;
					innerLoopStartIndex.push_back(verticesList.size());
				}
				
				for (Vertex* v : verticesList)
				{
					VPMapping vpmap;
					
					vpmap.v = v;
					vpmap.p3d = v->m_Position;
					vpMappingList.push_back(vpmap);
				}
				//get the 3d plane information
				Eigen::Matrix4f mat = TranslationMatrixTo2d(vpMappingList[0].p3d, vpMappingList[1].p3d, vpMappingList[2].p3d);

				for (VPMapping &vpm : vpMappingList)
				{
					glm::vec2 p2d = Translate2d(vpm.p3d, mat);
					vpm.p2d.x = p2d.x;
					vpm.p2d.y = p2d.y;
					
				}

			

				//make triangulation using earcutting algorithm
				int index = 0;
				std::vector<std::vector<Point>> polygon;
				std::vector<Point> points;
				std::unordered_map<int, Vertex*> map;
				int count = 0;
				int loopCount = 0;

				for (auto it = vpMappingList.begin(); it != vpMappingList.end(); it++)
				{
					Point p = { (*it).p2d.x ,(*it).p2d.y };
					points.push_back(p);
					map[index] = it->v;
					index++;
					count++;

					if (count == innerLoopStartIndex[loopCount])
					{
						polygon.push_back(points);
						points.clear();
						loopCount++;
					}

				}

				
				
				std::vector<int> result = mapbox::earcut<int>(polygon);
				
				for (int i = 0; i  < result.size(); i +=3)
				{
					Vertex* v1, * v2, * v3;
					v1 = map[result[i]];
					v2 = map[result[i + 1]];
					v3 = map[result[i + 2]];
					TriangleIntices tri = TriangleIntices(v1, v2, v3);
					triList.push_back(tri);
						
				}

				return triList;

			}


		}

		
	}
	
	glm::vec2 Translate2d(glm::vec3 point3d,Eigen::Matrix4f mat)
	{   
		Eigen::Vector4f temp3d(point3d.x, point3d.y, point3d.z,1.0f);
		Eigen::Vector4f temp2d = mat * temp3d;
		glm::vec2 result(temp2d[0], temp2d[1]);
		
		return result;
	}

	Eigen::Matrix4f TranslationMatrixTo2d(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
	{   //get the matrix to translate 3d points to 2d
		Eigen::Vector3f v1 (p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
		Eigen::Vector3f v2 (p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
		Eigen::Vector3f normal = v1.cross(v2);
		Eigen::Vector3f b;
		Eigen::Matrix4f T;
		T<< 1.0f, 0.0f, 0.0f, -p1.x,
			0.0f, 1.0f, 0.0f, -p1.y,
		    0.0f, 0.0f, 1.0f, -p1.z,
			0.0f, 0.0f, 0.0f, 1.0f;
		Eigen::Matrix4f R;
		normal.normalize();
		if (abs(normal[0] - 1.0f) <= epsilon && normal[0] > 0)
		{
			R << 0.0f, 0.0f,-1.0f, 0.0f,
				 0.0f, 1.0f, 0.0f, 0.0f,
				 1.0f, 0.0f, 0.0f, 0.0f,
				 0.0f, 0.0f, 0.0f, 1.0f;
			return R * T;
		}

		else if (abs(normal[0] - 1.0f) <= epsilon && normal[0] < 0)
		{
			R << 0.0f, 0.0f,1.0f, 0.0f,
				 0.0f, 1.0f, 0.0f, 0.0f,
			    -1.0f, 0.0f, 0.0f, 0.0f,
				 0.0f, 0.0f, 0.0f, 1.0f;
			return R * T;
		}

		else if (abs(normal[1] - 1.0f) <= epsilon && normal[1] > 0)
		{
			R << 1.0f, 0.0f, 0.0f, 0.0f,
				 0.0f, 0.0f,-1.0f, 0.0f,
				 0.0f, 1.0f, 0.0f, 0.0f,
				 0.0f, 0.0f, 0.0f, 1.0f;
			return R * T;
		}

		else if (abs(normal[1] - 1.0f) <= epsilon && normal[1] < 0)
		{
			R << 1.0f, 0.0f, 0.0f, 0.0f,
				 0.0f, 0.0f, 1.0f, 0.0f,
				 0.0f,-1.0f, 0.0f, 0.0f,
				 0.0f, 0.0f, 0.0f, 1.0f;
			return R * T;
		}

		if (normal[2] >= 0)
		{
			b = v1.cross(normal);
			b.normalize();
			v1.normalize();
			R<< v1[0],     v1[1],     v1[2], 0.0f,
				 b[0],      b[1],      b[2], 0.0f,
		    normal[0], normal[1], normal[2], 0.0f,
				 0.0f,      0.0f,      0.0f, 1.0f;
		
			
			return R * T;

		}

		else {
			normal = -normal;
			b = v1.cross(normal);
			b.normalize();
			v1.normalize();
			R << v1[0], v1[1], v1[2], 0.0f,
				b[0], b[1], b[2], 0.0f,
				normal[0], normal[1], normal[2], 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f;
			return R * T;
		}

	}



	

	TriangleIntices::TriangleIntices()
	{



	}

	TriangleIntices::TriangleIntices(Vertex *v1, Vertex *v2, Vertex *v3)
	{
		vertex3[0] = v1;
		vertex3[1] = v2;
		vertex3[2] = v3;

	}

	TriangleIntices::~TriangleIntices()
	{

	}

}