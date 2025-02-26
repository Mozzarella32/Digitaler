#pragma once

#include "pch.h"

#include "math.h"

#include "BlockAndPathData.h"

struct PathVertex {

	int x1;
	int y1;

	int x2;
	int y2;


	//int flags = 0;//flags

	//enum Flags {
	//	Marked = 1 << 0,
	//	//Colour = 1 << 1,
	//};

	//int colour = 1;//colour

	//int marked = false;

	PathVertex(int x1, int y1, int x2, int y2/*,int flags= 0*/)
		:
		x1(std::min(x1, x2)),
		y1(std::min(y1, y2)),
		x2(std::max(x1, x2)),
		y2(std::max(y1, y2))
		//flags(flags) 
		{
	}

	PathVertex(const PointType& p1, const PointType&p2/*, const int& flags = 0*/)
		:
		x1(std::min(p1.x(), p2.x())),
		y1(std::min(p1.y(), p2.y())),
		x2(std::max(p1.x(), p2.x())),
		y2(std::max(p1.y(), p2.y()))
		//flags(flags)
		{
	}

	bool operator ==(const PathVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PathVertex), (void*)offsetof(PathVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PathVertex), (void*)offsetof(PathVertex, x2)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));

		/*GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(PathVertex), (void*)offsetof(PathVertex, flags)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));*/
		//GLCALL(glEnableVertexAttribArray(Position));
		//GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(PathVertex), (void*)offsetof(PathVertex, colour)));
		//GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		//GLCALL(glEnableVertexAttribArray(Position));
		//GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(PathVertex), (void*)offsetof(PathVertex, marked)));
		//GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};


struct RectRGBAVertex {

	int x1;
	int y1;

	int x2;
	int y2;

	float r;
	float g;
	float b;
	float a;


	//int flags = 0;//flags

	//enum Flags {
	//	Marked = 1 << 0,
	//	//Colour = 1 << 1,
	//};

	//int colour = 1;//colour

	//int marked = false;

	RectRGBAVertex(int x1, int y1, int x2, int y2, float r, float g, float b, float a/*,int flags= 0*/)
		:
		x1(std::min(x1, x2)),
		y1(std::min(y1, y2)),
		x2(std::max(x1, x2)),
		y2(std::max(y1, y2)),
		r(r),
		g(g),
		b(b),
		a(a)
		//flags(flags) 
	{
	}

	RectRGBAVertex(const PointType& p1, const PointType& p2, const ColourType& c/*, const int& flags = 0*/)
		:
		x1(std::min(p1.x(), p2.x())),
		y1(std::min(p1.y(), p2.y())),
		x2(std::max(p1.x(), p2.x())),
		y2(std::max(p1.y(), p2.y())),
		r(c.x()),
		g(c.y()),
		b(c.z()),
		a(c.w())
		//flags(flags)
	{
	}

	bool operator ==(const RectRGBAVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(RectRGBAVertex), (void*)offsetof(RectRGBAVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(RectRGBAVertex), (void*)offsetof(RectRGBAVertex, x2)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 4, GL_FLOAT, GL_FALSE, sizeof(RectRGBAVertex), (void*)offsetof(RectRGBAVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		/*GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(RectColourVertex), (void*)offsetof(RectColourVertex, flags)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));*/
		//GLCALL(glEnableVertexAttribArray(Position));
		//GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(RectColourVertex), (void*)offsetof(RectColourVertex, colour)));
		//GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		//GLCALL(glEnableVertexAttribArray(Position));
		//GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(RectColourVertex), (void*)offsetof(RectColourVertex, marked)));
		//GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

namespace std {
	template <>
	struct hash<PathVertex> {
		std::size_t operator()(const PathVertex& v) const {
			// Kombiniere die Hashwerte der einzelnen Felder
			std::size_t h1 = std::hash<int>{}(v.x1);
			std::size_t h2 = std::hash<int>{}(v.y1);
			std::size_t h3 = std::hash<int>{}(v.x2);
			std::size_t h4 = std::hash<int>{}(v.y2);

			// Kombiniere die Hashes
			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
		}
	};
}


struct PointVertex {

	int x1;
	int y1;
	
	PointVertex(int x1, int y1)
		:
		x1(x1),
		y1(y1)
	{
	}

	PointVertex(const PointType& p1)
		:
		x1(p1.x()),
		y1(p1.y())
	{
	}

	bool operator ==(const PointVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointVertex), (void*)offsetof(PointVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

namespace std {
	template <>
	struct hash<PointVertex> {
		std::size_t operator()(const PointVertex& v) const {
			// Kombiniere die Hashwerte der einzelnen Felder
			std::size_t h1 = std::hash<int>{}(v.x1);
			std::size_t h2 = std::hash<int>{}(v.y1);

			// Kombiniere die Hashes
			return h1 ^ (h2 << 1);
		}
	};
}

struct IndexVertex {

	int index;

	bool operator ==(const IndexVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(IndexVertex), (void*)offsetof(IndexVertex, index)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};
