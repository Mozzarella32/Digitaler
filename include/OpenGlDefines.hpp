#pragma once

#include "pch.hpp"

//#include "math.h"

#include "BlockAndPathData.hpp"

// Path: Edges EdgesMarked EdgesUnmarked Verts
struct TwoPointIRGBRHGHBHVertex {

	int x1;
	int y1;

	int x2;
	int y2;

	float r;
	float g;
	float b;

	float rh;
	float gh;
	float bh;
	//int flags = 0;//flags

	//enum Flags {
	//	Marked = 1 << 0,
	//	//Colour = 1 << 1,
	//};

	//int colour = 1;//colour

	//int marked = false;

	TwoPointIRGBRHGHBHVertex(int x1, int y1, int x2, int y2, float r, float g, float b, float rh, float gh, float bh)
		:
		x1(std::min(x1, x2)),
		y1(std::min(y1, y2)),
		x2(std::max(x1, x2)),
		y2(std::max(y1, y2)),
		r(r),
		g(g),
		b(b),
		rh(rh),
		gh(gh),
		bh(bh)
		//flags(flags) 
	{
	}

	TwoPointIRGBRHGHBHVertex(const PointType& p1, const PointType& p2, const ColourType& c, const ColourType& h)
		:
		x1(std::min(p1.x(), p2.x())),
		y1(std::min(p1.y(), p2.y())),
		x2(std::max(p1.x(), p2.x())),
		y2(std::max(p1.y(), p2.y())),
		r(c.x()),
		g(c.y()),
		b(c.z()),
		rh(h.x()),
		gh(h.y()),
		bh(h.z())
		//flags(flags)
	{
	}

	TwoPointIRGBRHGHBHVertex() {};

	bool operator ==(const TwoPointIRGBRHGHBHVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(TwoPointIRGBRHGHBHVertex), (void*)offsetof(TwoPointIRGBRHGHBHVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(TwoPointIRGBRHGHBHVertex), (void*)offsetof(TwoPointIRGBRHGHBHVertex, x2)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(TwoPointIRGBRHGHBHVertex), (void*)offsetof(TwoPointIRGBRHGHBHVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(TwoPointIRGBRHGHBHVertex), (void*)offsetof(TwoPointIRGBRHGHBHVertex, rh)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		/*GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(TwoPointIRGBRHGHBHVertex), (void*)offsetof(TwoPointIRGBRHGHBHVertex, flags)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));*/
		//GLCALL(glEnableVertexAttribArray(Position));
		//GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(TwoPointIRGBRHGHBHVertex), (void*)offsetof(TwoPointIRGBRHGHBHVertex, colour)));
		//GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		//GLCALL(glEnableVertexAttribArray(Position));
		//GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(TwoPointIRGBRHGHBHVertex), (void*)offsetof(TwoPointIRGBRHGHBHVertex, marked)));
		//GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

// CollisionGrid and Blocks
struct TwoPointIRGBAIDVertex {

	unsigned int id;

	int x1;
	int y1;

	int x2;
	int y2;

	float r;
	float g;
	float b;
	float a;

	float rHighlight;
	float gHighlight;
	float bHighlight;

	//int flags = 0;//flags

	//enum Flags {
	//	Marked = 1 << 0,
	//	//Colour = 1 << 1,
	//};

	//int colour = 1;//colour

	//int marked = false;

	TwoPointIRGBAIDVertex() {};

	//TwoPointIRGBAVertex(int x1, int y1, int x2, int y2, float r, float g, float b, float a/*,int flags= 0*/)
	//	:
	//	x1(std::min(x1, x2)),
	//	y1(std::min(y1, y2)),
	//	x2(std::max(x1, x2)),
	//	y2(std::max(y1, y2)),
	//	r(r),
	//	g(g),
	//	b(b),
	//	a(a)
	//	//flags(flags) 
	//{
	//}

	TwoPointIRGBAIDVertex(unsigned int id, const PointType& p1, const PointType& p2, const ColourType& c, const ColourType& Highlight/*, const int& flags = 0*/)
		:
		id(id),
		x1(std::min(p1.x(), p2.x())),
		y1(std::min(p1.y(), p2.y())),
		x2(std::max(p1.x(), p2.x())),
		y2(std::max(p1.y(), p2.y())),
		r(c.x()),
		g(c.y()),
		b(c.z()),
		a(c.w()),
		rHighlight(Highlight.x()),
		gHighlight(Highlight.y()),
		bHighlight(Highlight.z())
		//flags(flags)
	{
	}

	bool operator ==(const TwoPointIRGBAIDVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_UNSIGNED_INT, sizeof(TwoPointIRGBAIDVertex), (void*)offsetof(TwoPointIRGBAIDVertex, id)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(TwoPointIRGBAIDVertex), (void*)offsetof(TwoPointIRGBAIDVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(TwoPointIRGBAIDVertex), (void*)offsetof(TwoPointIRGBAIDVertex, x2)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 4, GL_FLOAT, GL_FALSE, sizeof(TwoPointIRGBAIDVertex), (void*)offsetof(TwoPointIRGBAIDVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(TwoPointIRGBAIDVertex), (void*)offsetof(TwoPointIRGBAIDVertex, rHighlight)));
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
	struct hash<TwoPointIRGBRHGHBHVertex> {
		std::size_t operator()(const TwoPointIRGBRHGHBHVertex& v) const {
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


//RoundPin
struct PointIRGBIDVertex {

	unsigned int id;

	int x;
	int y;

	float r;
	float g;
	float b;

	PointIRGBIDVertex() {};

	/*PointIRGBIDVertex(int x, int y, float r, float g, float b)
		:
		x(x),
		y(y),
		r(r),
		g(g),
		b(b)
	{
	}*/

	PointIRGBIDVertex(unsigned int id, const PointType& p, const ColourType& c)
		:
		id(id),
		x(p.x()),
		y(p.y()),
		r(c.x()),
		g(c.y()),
		b(c.z())
	{
	}

	bool operator ==(const PointIRGBIDVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_UNSIGNED_INT, sizeof(PointIRGBIDVertex), (void*)offsetof(PointIRGBIDVertex, id)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointIRGBIDVertex), (void*)offsetof(PointIRGBIDVertex, x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointIRGBIDVertex), (void*)offsetof(PointIRGBIDVertex, r)));
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

//Base Position Area Select
struct PointFRGBVertex {

	float x1;
	float y1;

	float x2;
	float y2;

	float r;
	float g;
	float b;

	PointFRGBVertex() {};

	/*PointFRGBVertex(int x, int y, float r, float g, float b)
		:
		x(x),
		y(y),
		r(r),
		g(g),
		b(b)
	{
	}*/

	PointFRGBVertex(const Eigen::Vector2f& p1, const Eigen::Vector2f& p2, const ColourType& c)
		:
		x1(std::min(p1.x(), p2.x())),
		y1(std::min(p1.y(), p2.y())),
		x2(std::max(p1.x(), p2.x())),
		y2(std::max(p1.y(), p2.y())),
		r(c.x()),
		g(c.y()),
		b(c.z())
	{
	}

	bool operator ==(const PointFRGBVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 2, GL_FLOAT, GL_FALSE, sizeof(PointFRGBVertex), (void*)offsetof(PointFRGBVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 2, GL_FLOAT, GL_FALSE, sizeof(PointFRGBVertex), (void*)offsetof(PointFRGBVertex, x2)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointFRGBVertex), (void*)offsetof(PointFRGBVertex, r)));
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

// struct PointIVertex {

// 	int x1;
// 	int y1;

// 	PointIVertex() {}

// 	PointIVertex(int x1, int y1)
// 		:
// 		x1(x1),
// 		y1(y1)
// 	{
// 	}

// 	PointIVertex(const PointType& p1)
// 		:
// 		x1(p1.x()),
// 		y1(p1.y())
// 	{
// 	}

// 	bool operator ==(const PointIVertex& Other) const = default;

// 	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
// 		GLCALL(glEnableVertexAttribArray(Position));
// 		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointIVertex), (void*)offsetof(PointIVertex, x1)));
// 		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
// 		/*GLCALL(glEnableVertexAttribArray(Position));
// 		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointFRGBVertex), (void*)offsetof(PointFRGBVertex, r)));
// 		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));*/
// 	}
// };

// Path: special points
struct PointIRGBVertex {

	int x1;
	int y1;

	float r;
	float g;
	float b;

	PointIRGBVertex() {}

	PointIRGBVertex(int x1, int y1, float r, float g, float b)
		:
		x1(x1),
		y1(y1),
		r(r),
		g(g),
		b(b)
	{
	}

	PointIRGBVertex(const PointType& p1, const ColourType& c)
		:
		x1(p1.x()),
		y1(p1.y()),
		r(c.x()),
		g(c.y()),
		b(c.z())
	{
	}

	bool operator ==(const PointIRGBVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointIRGBVertex), (void*)offsetof(PointIRGBVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointIRGBVertex), (void*)offsetof(PointIRGBVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

namespace std {
	template <>
	struct hash<PointIRGBVertex> {
		std::size_t operator()(const PointIRGBVertex& v) const {
			// Kombiniere die Hashwerte der einzelnen Felder
			std::size_t h1 = std::hash<int>{}(v.x1);
			std::size_t h2 = std::hash<int>{}(v.y1);

			// Kombiniere die Hashes
			return h1 ^ (h2 << 1);
		}
	};
}

// And Or XOr
struct PointIOrientationRGBIDVertex {

	unsigned int id;

	int x;
	int y;

	int Transform;

	float r;
	float g;
	float b;

	PointIOrientationRGBIDVertex() {}

	//PointIOrientationRGBIDVertex(int x, int y, int Orientation)
	//	:
	//	x(x),
	//	y(y),
	//	Orientation(Orientation)
	//	//flags(flags) 
	//{
	//}

	PointIOrientationRGBIDVertex(unsigned int id, const BlockMetadata& meta, const ColourType& c)
		:
		id(id),
		x(meta.Pos.x()),
		y(meta.Pos.y()),
		Transform(meta.Transform()),
		r(c.x()),
		g(c.y()),
		b(c.z())
	{
	}

	bool operator ==(const PointIOrientationRGBIDVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_UNSIGNED_INT, sizeof(PointIOrientationRGBIDVertex), (void*)offsetof(PointIOrientationRGBIDVertex, id)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointIOrientationRGBIDVertex), (void*)offsetof(PointIOrientationRGBIDVertex, x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(PointIOrientationRGBIDVertex), (void*)offsetof(PointIOrientationRGBIDVertex, Transform)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointIOrientationRGBIDVertex), (void*)offsetof(PointIOrientationRGBIDVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

// Pin
struct PointIOrientationRGBRHGHBHIDVertex {

	int id;

	int x;
	int y;

	int Orientation;

	float r;
	float g;
	float b;

	float rHighlight;
	float gHighlight;
	float bHighlight;

	PointIOrientationRGBRHGHBHIDVertex() {}

	//PointIOrientationRGBRHGHBHIDVertex(int x, int y, int Orientation, float r, float g, float b)
	//	:
	//	x(x),
	//	y(y),
	//	Orientation(Orientation),
	//	r(r),
	//	g(g),
	//	b(b)
	//	//flags(flags) 
	//{
	//}

	PointIOrientationRGBRHGHBHIDVertex(unsigned int id, const PointType& p, const MyDirection::Direction& d, const ColourType& c, const ColourType& Highlight)
		:
		id(id),
		x(p.x()),
		y(p.y()),
		Orientation([d]() {
		switch (d) {
		case MyDirection::Up: return 0;
		case MyDirection::Right: return 1;
		case MyDirection::Down: return 2;
		case MyDirection::Left: return 3;
		case MyDirection::UpLeft:assert(false && "Invalid Direction for Pin");
		case MyDirection::UpRight:assert(false && "Invalid Direction for Pin");
		case MyDirection::DownLeft:assert(false && "Invalid Direction for Pin");
		case MyDirection::DownRight:assert(false && "Invalid Direction for Pin");
		case MyDirection::Neutral:assert(false && "Invalid Direction for Pin");
		default: assert(false && "Invalid Direction for Pin");
		}
		assert(false && "Invalid Direction for Pin");
		return 0;
			}()),
		r(c.x()),
		g(c.y()),
		b(c.z()),
		rHighlight(Highlight.x()),
		gHighlight(Highlight.y()),
		bHighlight(Highlight.z())
		//flags(flags)
	{
	}

	bool operator ==(const PointIOrientationRGBRHGHBHIDVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_UNSIGNED_INT, sizeof(PointIOrientationRGBRHGHBHIDVertex), (void*)offsetof(PointIOrientationRGBRHGHBHIDVertex, id)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointIOrientationRGBRHGHBHIDVertex), (void*)offsetof(PointIOrientationRGBRHGHBHIDVertex, x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(PointIOrientationRGBRHGHBHIDVertex), (void*)offsetof(PointIOrientationRGBRHGHBHIDVertex, Orientation)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointIOrientationRGBRHGHBHIDVertex), (void*)offsetof(PointIOrientationRGBRHGHBHIDVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointIOrientationRGBRHGHBHIDVertex), (void*)offsetof(PointIOrientationRGBRHGHBHIDVertex, rHighlight)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

// Seven
struct SevenSegVertex {

	int x;
	int y;

	int Transform;

	int flags;

	float r;
	float g;
	float b;

	static constexpr const std::array<int, 16> NumberTo7Flags = {
		0b0111111,//0
		0b0000110,//1
		0b1011011,//2
		0b1001111,//3
		0b1100110,//4
		0b1101101,//5
		0b1111101,//6
		0b0000111,//7
		0b1111111,//8
		0b1101111,//9
		0b1110111,//A
		0b1111100,//b
		0b0111001,//C
		0b1011110,//d
		0b1111001,//E
		0b1110001,//F
	};

	SevenSegVertex() {}

	//SevenSegVertex(int x, int y, int Orientation, int flags, float r, float g, float b)
	//	:
	//	x(x),
	//	y(y),
	//	Orientation(Orientation),
	//	flags(flags),
	//	r(r),
	//	g(g),
	//	b(b)
	//	//flags(flags) 
	//{
	//}


	//Number must be between 0 and F(15)
	SevenSegVertex(const BlockMetadata& meta, int number, const ColourType& c/*, const int& flags = 0*/)
		:
		x(meta.Pos.x()),
		y(meta.Pos.y()),
		Transform(meta.Transform()),
		flags([number]() {
		assert(number >= 0 && number <= 15);
		return NumberTo7Flags[number];
			}()),
		r(c.x()),
		g(c.y()),
		b(c.z())
		//flags(flags)
	{
	}

	bool operator ==(const SevenSegVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(SevenSegVertex), (void*)offsetof(SevenSegVertex, x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(SevenSegVertex), (void*)offsetof(SevenSegVertex, Transform)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(SevenSegVertex), (void*)offsetof(SevenSegVertex, flags)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(SevenSegVertex), (void*)offsetof(SevenSegVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

// Sixteen
struct SixteenSegVertex {

	int x;
	int y;

	int Transform;

	int flags;

	float r;
	float g;
	float b;

	static constexpr const std::array<int, 128> NumberTo16Flags = {
		0b0,//0
		0b0,//1
		0b0,//2
		0b0,//3
		0b0,//4
		0b0,//5
		0b0,//6
		0b0,//7
		0b0,//8
		0b0,//9
		0b0,//10
		0b0,//11
		0b0,//12
		0b0,//13
		0b0,//14
		0b0,//15
		0b0,//16
		0b0,//17
		0b0,//18
		0b0,//19
		0b0,//20
		0b0,//21
		0b0,//22
		0b0,//23
		0b0,//24
		0b0,//25
		0b0,//26
		0b0,//27
		0b0,//28
		0b0,//29
		0b0,//30
		0b0,//31
		0b0,//32
		0b10100100000000000,//!
		0b0000100010000000,//"
		0b0100101100111100,//#
		0b0100101110111011,//$
		0b1011011110001000,//%
		0b1000011101110011,//&
		0b0001000000000000,//'
		0b1001000000000000,//(
		0b0010010000000000,//)
		0b1111111100000000,//*
		0b0100101100000000,//+
		0b0010000000000000,//,
		0b0000001100000000,//-
		0b10000000000000000,//.
		0b0011000000000000,///
		0b0011000011111111,//0
		0b0001000000001100,//1
		0b1101110111,//2
		0b1100111111,//3
		0b1110001100,//4
		0b1110111011,//5
		0b1111111011,//6
		0b0000001111,//7
		0b1111111111,//8
		0b1110111111,//9
		0b0000100001,//:
		0b0010000000000001,//;
		0b1110000011,//<
		0b1100000011,//=
		0b1100000111,//>
		0b10100001000000111,//?
		0b0100000101111111,//@
		0b1111001111,//A
		0b0001001111111011,//B
		0b0011110011,//C
		0b0100100000111111,//D
		0b0111110011,//E
		0b0111000011,//F
		0b1011111011,//G
		0b1111001100,//H
		0b0100100000110011,//I
		0b0001111100,//J
		0b1001000111000000,//K
		0b0011110000,//L
		0b0001010011001100,//M
		0b1000010011001100,//N
		0b0011111111,//O
		0b1111000111,//P
		0b1000000011111111,//Q
		0b1000001111000111,//R
		0b1110111011,//S
		0b0100100000000011,//T
		0b0011111100,//U
		0b0011000011000000,//V
		0b1010000011001100,//W
		0b1011010000000000,//X
		0b0101010000000000,//Y
		0b0011001100110011,//Z
		0b0100100000010010,//[
		0b1000010000000000,//backslash 
		0b0100100000100001,//]
		0b0001000000000100,//^
		0b0000110000,//_
		0b0000010000000000,//`
		0b0100000101110000,//a
		0b1000000111110000,//b
		0b1101110000,//c
		0b0010001000111100,//d
		0b0010000101110000,//e
		0b0001001111000011,//f
		0b0000011000111111,//g
		0b1111001000,//h
		0b0001000000,//i
		0b0000111100,//j
		0b1000001111000000,//k
		0b0001110000,//l
		0b0100001101001000,//m
		0b1000000101000000,//n
		0b1101111000,//o
		0b0001000111000011,//p
		0b0000011000001111,//q
		0b1101000000,//r
		0b1000001000110000,//s
		0b0111110000,//t
		0b0001111000,//u
		0b0010000001000000,//v
		0b1010000001001000,//w
		0b1010001100000000,//x
		0b0000011000111100,//y
		0b0010000100110000,//z
		0b1001000100000000,//{
		0b0100100000000000,//|
		0b0010011000000000,//}
		0b11,//~
		0b0,//127
	};

	SixteenSegVertex() {}

	// a b 
	//hklmc
	// i j 
	//gnopd
	// f e
	//SixteenSegVertex(int x, int y, int Orientation, int flags, float r, float g, float b)
	//	:
	//	x(x),
	//	y(y),
	//	Orientation(Orientation),
	//	flags(flags),
	//	r(r),
	//	g(g),
	//	b(b)
	//	//flags(flags) 
	//{
	//}

	//Out of data comment:
	//Number must be between 0 and F(15)
	SixteenSegVertex(const BlockMetadata& meta, int number, const ColourType& c/*, const int& flags = 0*/)
		:
		x(meta.Pos.x()),
		y(meta.Pos.y()),
		Transform(meta.Transform()),
		flags([number]() {
		assert(number >= 0 && number < 128);
		return NumberTo16Flags[number];
			}()),
		//flags(flags),
		r(c.x()),
		g(c.y()),
		b(c.z())
		//flags(flags)
	{
	}

	bool operator ==(const SixteenSegVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(SixteenSegVertex), (void*)offsetof(SixteenSegVertex, x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(SixteenSegVertex), (void*)offsetof(SixteenSegVertex, Transform)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(SixteenSegVertex), (void*)offsetof(SixteenSegVertex, flags)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(SixteenSegVertex), (void*)offsetof(SixteenSegVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

// Mux
struct MuxIDVertex {

	unsigned int id;

	int x;
	int y;

	int Transform;

	int size;
	int selected;

	float r;
	float g;
	float b;

	MuxIDVertex() {}

	//MuxIDVertex(int x, int y, int Orientation, int size, int selected, float r, float g, float b)
	//	:
	//	x(x),
	//	y(y),
	//	Orientation(Orientation),
	//	size(size),
	//	selected(selected),
	//	r(r),
	//	g(g),
	//	b(b)
	//	//flags(flags) 
	//{
	//}


	//Number must be between 0 and F(15)
	MuxIDVertex(unsigned int id, const BlockMetadata& meta, int size, int selected, const ColourType& c/*, const int& flags = 0*/)
		:
		id(id),
		x(meta.Pos.x()),
		y(meta.Pos.y()),
		Transform(meta.Transform()),
		size(size),
		selected(selected),
		r(c.x()),
		g(c.y()),
		b(c.z())
		//flags(flags)
	{
	}

	bool operator ==(const MuxIDVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_UNSIGNED_INT, sizeof(MuxIDVertex), (void*)offsetof(MuxIDVertex, id)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(MuxIDVertex), (void*)offsetof(MuxIDVertex, x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(MuxIDVertex), (void*)offsetof(MuxIDVertex, Transform)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(MuxIDVertex), (void*)offsetof(MuxIDVertex, size)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(MuxIDVertex), (void*)offsetof(MuxIDVertex, selected)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(MuxIDVertex), (void*)offsetof(MuxIDVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

// Text
struct TextVertex {
	/*float x;
	float y;

	float u;
	float v;*/

	//l t r b

	float x;
	float y;

	float PosOffLeft;
	float PosOffTop;
	float PosOffRight;
	float PosOffBottom;

	float UVOffLeft;
	float UVOffTop;
	float UVOffRight;
	float UVOffBottom;

	int Orientation;
	float FontScale;

	float ColorR;
	float ColorG;
	float ColorB;
	float ColorA;

	float BackgroundR;
	float BackgroundG;
	float BackgroundB;
	float BackgroundA;

	bool operator ==(const TextVertex& Other) const = default;

	TextVertex() {}

	TextVertex(const Point<float>& Pos, const float* PosOff, const float* UVOff, const float& Scale, const MyDirection::Direction& d, const ColourType& Foregrorund, const ColourType& Background)
		:
		x(Pos.x),
		y(Pos.y),
		PosOffLeft(PosOff[0] * Scale),
		PosOffTop(PosOff[1] * Scale),
		PosOffRight(PosOff[2] * Scale),
		PosOffBottom(PosOff[3] * Scale),
		UVOffLeft(UVOff[0]),
		UVOffTop(UVOff[1]),
		UVOffRight(UVOff[2]),
		UVOffBottom(UVOff[3]),
		Orientation([d]() {
		switch (d) {
		case MyDirection::Up: return 0;
		case MyDirection::Right: return 1;
		case MyDirection::Down: return 2;
		case MyDirection::Left: return 3;
		case MyDirection::UpLeft:assert(false && "Invalid Direction for Pin");
		case MyDirection::UpRight:assert(false && "Invalid Direction for Pin");
		case MyDirection::DownLeft:assert(false && "Invalid Direction for Pin");
		case MyDirection::DownRight:assert(false && "Invalid Direction for Pin");
		case MyDirection::Neutral:assert(false && "Invalid Direction for Pin");
		default: assert(false && "Invalid Direction for Pin");
		}
		assert(false && "Invalid Direction for Pin");
		return 0;
			}()),
		FontScale(Scale),
		ColorR(Foregrorund.x()),
		ColorG(Foregrorund.y()),
		ColorB(Foregrorund.z()),
		ColorA(Foregrorund.w()),
		BackgroundR(Background.x()),
		BackgroundG(Background.y()),
		BackgroundB(Background.z()),
		BackgroundA(Background.w())
	{
	}

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(TextVertex), (void*)offsetof(TextVertex, x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, PosOffLeft)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, UVOffLeft)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(TextVertex), (void*)offsetof(TextVertex, Orientation)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 1, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, FontScale)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, ColorR)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, BackgroundR)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}

};

struct IndexVertex {
	int index;

	bool operator ==(const IndexVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(IndexVertex), (void*)offsetof(IndexVertex, index)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};
