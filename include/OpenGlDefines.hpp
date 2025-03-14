#pragma once

#include "pch.hpp"

//#include "math.h"

#include "BlockAndPathData.hpp"

struct TwoPointIVertex {

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

	TwoPointIVertex(int x1, int y1, int x2, int y2/*,int flags= 0*/)
		:
		x1(std::min(x1, x2)),
		y1(std::min(y1, y2)),
		x2(std::max(x1, x2)),
		y2(std::max(y1, y2))
		//flags(flags) 
	{
	}

	TwoPointIVertex(const PointType& p1, const PointType& p2/*, const int& flags = 0*/)
		:
		x1(std::min(p1.x(), p2.x())),
		y1(std::min(p1.y(), p2.y())),
		x2(std::max(p1.x(), p2.x())),
		y2(std::max(p1.y(), p2.y()))
		//flags(flags)
	{
	}

	bool operator ==(const TwoPointIVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(TwoPointIVertex), (void*)offsetof(TwoPointIVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(TwoPointIVertex), (void*)offsetof(TwoPointIVertex, x2)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));

		/*GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(TwoPointIVertex), (void*)offsetof(TwoPointIVertex, flags)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));*/
		//GLCALL(glEnableVertexAttribArray(Position));
		//GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(TwoPointIVertex), (void*)offsetof(TwoPointIVertex, colour)));
		//GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		//GLCALL(glEnableVertexAttribArray(Position));
		//GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(TwoPointIVertex), (void*)offsetof(TwoPointIVertex, marked)));
		//GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};


struct TwoPointIRGBAVertex{

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

	TwoPointIRGBAVertex(int x1, int y1, int x2, int y2, float r, float g, float b, float a/*,int flags= 0*/)
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

	TwoPointIRGBAVertex(const PointType& p1, const PointType& p2, const ColourType& c/*, const int& flags = 0*/)
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

	bool operator ==(const TwoPointIRGBAVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(TwoPointIRGBAVertex), (void*)offsetof(TwoPointIRGBAVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(TwoPointIRGBAVertex), (void*)offsetof(TwoPointIRGBAVertex, x2)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 4, GL_FLOAT, GL_FALSE, sizeof(TwoPointIRGBAVertex), (void*)offsetof(TwoPointIRGBAVertex, r)));
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
	struct hash<TwoPointIVertex> {
		std::size_t operator()(const TwoPointIVertex& v) const {
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



struct PointIRGBVertex{

	int x;
	int y;

	float r;
	float g;
	float b;

	PointIRGBVertex(int x, int y, float r, float g, float b)
		:
		x(x),
		y(y),
		r(r),
		g(g),
		b(b)
	{
	}

	PointIRGBVertex(const PointType& p, const ColourType& c)
		:
		x(p.x()),
		y(p.y()),
		r(c.x()),
		g(c.y()),
		b(c.z())
	{
	}

	bool operator ==(const PointIRGBVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointIRGBVertex), (void*)offsetof(PointIRGBVertex, x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointIRGBVertex), (void*)offsetof(PointIRGBVertex, r)));
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

struct PointIVertex {

	int x1;
	int y1;

	PointIVertex(int x1, int y1)
		:
		x1(x1),
		y1(y1)
	{
	}

	PointIVertex(const PointType& p1)
		:
		x1(p1.x()),
		y1(p1.y())
	{
	}

	bool operator ==(const PointIVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointIVertex), (void*)offsetof(PointIVertex, x1)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

namespace std {
	template <>
	struct hash<PointIVertex> {
		std::size_t operator()(const PointIVertex& v) const {
			// Kombiniere die Hashwerte der einzelnen Felder
			std::size_t h1 = std::hash<int>{}(v.x1);
			std::size_t h2 = std::hash<int>{}(v.y1);

			// Kombiniere die Hashes
			return h1 ^ (h2 << 1);
		}
	};
}

struct PointIOrientationRGBVertex{

	int x;
	int y;

	int Orientation;

	float r;
	float g;
	float b;

	PointIOrientationRGBVertex(int x, int y, int Orientation, float r, float g, float b)
		:
		x(x),
		y(y),
		Orientation(Orientation),
		r(r),
		g(g),
		b(b)
		//flags(flags) 
	{
	}

	PointIOrientationRGBVertex(const PointType& p, const MyDirektion::Direktion& d, const ColourType& c/*, const int& flags = 0*/)
		:
		x(p.x()),
		y(p.y()),
		Orientation([d]() {
		switch (MyDirektion::RotateCW(d)) {
		case MyDirektion::Right: return 0;
		case MyDirektion::Up: return 1;
		case MyDirektion::Left: return 2;
		case MyDirektion::Down: return 3;
		case MyDirektion::UpLeft:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::UpRight:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::DownLeft:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::DownRight:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::Neutral:assert(false && "Invalid Direktion for Pin");
		default: assert(false && "Invalid Direktion for Pin");
		}
		assert(false && "Invalid Direktion for Pin");
		return 0;
			}()),
		r(c.x()),
		g(c.y()),
		b(c.z())
		//flags(flags)
	{
	}

	bool operator ==(const PointIOrientationRGBVertex& Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(PointIOrientationRGBVertex), (void*)offsetof(PointIOrientationRGBVertex, x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(PointIOrientationRGBVertex), (void*)offsetof(PointIOrientationRGBVertex, Orientation)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(PointIOrientationRGBVertex), (void*)offsetof(PointIOrientationRGBVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};


struct SevenSegVertex {

	int x;
	int y;

	int Orientation;

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

	SevenSegVertex(int x, int y, int Orientation, int flags, float r, float g, float b)
		:
		x(x),
		y(y),
		Orientation(Orientation),
		flags(flags),
		r(r),
		g(g),
		b(b)
		//flags(flags) 
	{
	}


	//Number must be between 0 and F(15)
	SevenSegVertex(const PointType& p, const MyDirektion::Direktion& d, int number, const ColourType& c/*, const int& flags = 0*/)
		:
		x(p.x()),
		y(p.y()),
		Orientation([d]() {
		switch (MyDirektion::RotateCW(d)) {
		case MyDirektion::Right: return 0;
		case MyDirektion::Up: return 1;
		case MyDirektion::Left: return 2;
		case MyDirektion::Down: return 3;
		case MyDirektion::UpLeft:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::UpRight:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::DownLeft:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::DownRight:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::Neutral:assert(false && "Invalid Direktion for Pin");
		default: assert(false && "Invalid Direktion for Pin");
		}
		assert(false && "Invalid Direktion for Pin");
		return 0;
			}()),
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
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(SevenSegVertex), (void*)offsetof(SevenSegVertex, Orientation)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(SevenSegVertex), (void*)offsetof(SevenSegVertex, flags)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(SevenSegVertex), (void*)offsetof(SevenSegVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

struct SixteenSegVertex {

	int x;
	int y;

	int Orientation;

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
		0b1000010000000000,//\ 
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


	// a b 
	//hklmc
	// i j 
	//gnopd
	// f e
	SixteenSegVertex(int x, int y, int Orientation, int flags, float r, float g, float b)
		:
		x(x),
		y(y),
		Orientation(Orientation),
		flags(flags),
		r(r),
		g(g),
		b(b)
		//flags(flags) 
	{
	}

	//Out of data comment:
	//Number must be between 0 and F(15)
	SixteenSegVertex(const PointType& p, const MyDirektion::Direktion& d, int number, const ColourType& c/*, const int& flags = 0*/)
		:
		x(p.x()),
		y(p.y()),
		Orientation([d]() {
		switch (MyDirektion::RotateCW(d)) {
		case MyDirektion::Right: return 0;
		case MyDirektion::Up: return 1;
		case MyDirektion::Left: return 2;
		case MyDirektion::Down: return 3;
		case MyDirektion::UpLeft:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::UpRight:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::DownLeft:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::DownRight:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::Neutral:assert(false && "Invalid Direktion for Pin");
		default: assert(false && "Invalid Direktion for Pin");
		}
		assert(false && "Invalid Direktion for Pin");
		return 0;
			}()),
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
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(SixteenSegVertex), (void*)offsetof(SixteenSegVertex, Orientation)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(SixteenSegVertex), (void*)offsetof(SixteenSegVertex, flags)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(SixteenSegVertex), (void*)offsetof(SixteenSegVertex, r)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
	}
};

struct MuxVertex {

	int x;
	int y;

	int Orientation;

	int size;
	int selected;

	float r;
	float g;
	float b;

	MuxVertex(int x, int y, int Orientation, int size, int selected, float r, float g, float b)
		:
		x(x),
		y(y),
		Orientation(Orientation),
		size(size),
		selected(selected),
		r(r),
		g(g),
		b(b)
		//flags(flags) 
	{
	}


	//Number must be between 0 and F(15)
	MuxVertex (const PointType& p, const MyDirektion::Direktion& d,int size, int selceted, const ColourType& c/*, const int& flags = 0*/)
		:
		x(p.x()),
		y(p.y()),
		Orientation([d]() {
		switch (MyDirektion::RotateCW(d)) {
		case MyDirektion::Right: return 0;
		case MyDirektion::Up: return 1;
		case MyDirektion::Left: return 2;
		case MyDirektion::Down: return 3;
		case MyDirektion::UpLeft:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::UpRight:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::DownLeft:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::DownRight:assert(false && "Invalid Direktion for Pin");
		case MyDirektion::Neutral:assert(false && "Invalid Direktion for Pin");
		default: assert(false && "Invalid Direktion for Pin");
		}
		assert(false && "Invalid Direktion for Pin");
		return 0;
			}()),
		size(size),
		selected(selected),
		r(c.x()),
		g(c.y()),
		b(c.z())
		//flags(flags)
	{
	}

	bool operator ==(const MuxVertex & Other) const = default;

	static void PrepareVBO(GLuint& Position, GLuint Instancingdivisor) {
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 2, GL_INT, sizeof(MuxVertex ), (void*)offsetof(MuxVertex , x)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(MuxVertex ), (void*)offsetof(MuxVertex , Orientation)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(MuxVertex ), (void*)offsetof(MuxVertex , size)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribIPointer(Position, 1, GL_INT, sizeof(MuxVertex), (void*)offsetof(MuxVertex, selected)));
		GLCALL(glVertexAttribDivisor(Position++, Instancingdivisor));
		GLCALL(glEnableVertexAttribArray(Position));
		GLCALL(glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(MuxVertex ), (void*)offsetof(MuxVertex , r)));
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
