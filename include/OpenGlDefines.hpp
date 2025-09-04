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
	}
};

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

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointIRGBVertex), (void*)offsetof(PointIRGBVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointIRGBVertex), (void*)offsetof(PointIRGBVertex, r)));
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

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(IndexVertex), (void*)offsetof(IndexVertex, index)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};


struct AssetVertex {
	int index;
	int id;
	int transform;

	int xi1;
	int yi1;

	int xi2;
	int yi2;

	float colorAr;
	float colorAg;
	float colorAb;
	float colorAa;

	private:

	AssetVertex(
	int index,
	int id,
	int transform,
	int xi1,
	int yi1,
	int xi2,
	int yi2,
	float colorAr,
	float colorAg,
	float colorAb,
	float colorAa
) :
	index(index),
	id(id),
	transform(transform),
	xi1(xi1),
	yi1(yi1),
	xi2(xi2),
	yi2(yi2),
	colorAr(colorAr),
	colorAg(colorAg),
	colorAb(colorAb),
	colorAa(colorAa)
	{}

	public:

	AssetVertex() {}

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(AssetVertex), (void*)offsetof(AssetVertex, index)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(AssetVertex), (void*)offsetof(AssetVertex, id)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(AssetVertex), (void*)offsetof(AssetVertex, transform)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(AssetVertex), (void*)offsetof(AssetVertex, xi1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(AssetVertex), (void*)offsetof(AssetVertex, xi2)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 4, GL_FLOAT, GL_FALSE, sizeof(AssetVertex), (void*)offsetof(AssetVertex, colorAr)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}

	enum class ID : unsigned int {
		Box,
		And,
		Or,
		Xor,
		SevenSeg,
		SixteenSeg,
		Mux,
		InputPin,
		OutputPin,
		InputRoundPin,
		OutputRoundPin,
		PathEdge,
		PathIntersection,
		PathVertex
	};

	static AssetVertex Box(int transform, const Eigen::Vector2i& p1, const Eigen::Vector2i& p2, const ColourType& colorA, int id = 0) {
		return AssetVertex((int)ID::Box, id, transform, p1.x(), p1.y(), p2.x(), p2.y(), colorA.x(), colorA.y(), colorA.z(), colorA.w());
	}

	static AssetVertex Gate(ID type, int transform, const Eigen::Vector2i& p, int id = 0) {
		assert(type == ID::And || type == ID::Or || type == ID::Xor);
		return AssetVertex((int)type, id, transform, p.x(), p.y(), 0, 0, 0, 0, 0, 0);
	}

	static AssetVertex Display(ID type, int transform, int segs, const Eigen::Vector2i& p, const ColourType& colorH, int id = 0) {
		assert(type == ID::SevenSeg || type == ID::SixteenSeg);
		return AssetVertex((int)type, id, transform, p.x(), p.y(), segs, 0, colorH.x(), colorH.y(), colorH.z(), colorH.w());
	}

	static AssetVertex Mux(int transform, int selected, const Eigen::Vector2i& p, const ColourType& colorSelected, int id = 0) {
		return AssetVertex((int)ID::Mux, id, transform, p.x(), p.y(), selected, 0, colorSelected.x(), colorSelected.y(), colorSelected.z(), colorSelected.w());
	}

	static AssetVertex Pin(bool input, int transform, const Eigen::Vector2i& p, const ColourType& color, int id = 0) {
		return AssetVertex(int(input ? ID::InputPin : ID::OutputPin), id, transform, p.x(), p.y(), 0, 0, color.x(), color.y(), color.z(), color.w());
	}

	static AssetVertex RoundPin(bool input, int transform, const Eigen::Vector2i& p, int id = 0) {
		return AssetVertex(int(input ? ID::InputRoundPin : ID::OutputRoundPin), id, transform, p.x(), p.y(), 0, 0, 0.0, 0.0, 0.0, 0.0);
	}

	static AssetVertex PathEdge(const Eigen::Vector2i& p1, const Eigen::Vector2i& p2, const ColourType& color, int id = 0) {
		return AssetVertex((int)ID::PathEdge, id, 0, std::max(p1.x(), p2.x()), std::max(p1.y(),p2.y()), std::min(p1.x(),p2.x()), std::min(p1.y(),p2.y()), color.x(), color.y(), color.z(), 0.0);
	}

	static AssetVertex PathIntersection(const Eigen::Vector2i& p, const ColourType& color, int id = 0) {
		return AssetVertex((int)ID::PathIntersection, id, 0, p.x(), p.y(), 0, 0, color.x(), color.y(), color.z(), 0.0);
	}

	static AssetVertex PathVertex(const Eigen::Vector2i& p, const ColourType& color, int id = 0) {
		return AssetVertex((int)ID::PathVertex, id, 0, p.x(), p.y(), 0, 0, color.x(), color.y(), color.z(), 0.0);
	}

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
};
