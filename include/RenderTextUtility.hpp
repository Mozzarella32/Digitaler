#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "OpenGlDefines.hpp"

struct RenderTextUtility {
private:
	struct CharInfo {
		float Advance;
		//l t r b
		std::array<float, 4> CursorOffsets;
		std::array<float, 4> Clip;
	};

	const std::map<int, CharInfo> CharMap;


public:
	static const constexpr float LineHeight = 1.32f;
	static const constexpr float TextAscender = 1.02f;
	static const constexpr float TextDescender = -0.3f;
	static const constexpr Point<int> TextAtlasSize = { 4280, 4280 };

public:
	struct TextInfo {
		std::vector<float> LineWidths;
		unsigned int LineCount;
		float TheoreticalHeight;//Based on acender/decender
		float EvendentHeight;//Based on the height of characters
		float Width;
	};

private:


	//BufferedVertexVec<TextVertex> StaticTextVerts;
	//BufferedVertexVec<TextVertex> DynamicTextVerts;

	 static int GetCharMapIndex(const char& c, const bool& bold, const bool& italic);

	 RenderTextUtility();

	static RenderTextUtility& GetInstance();
public:

	enum TextPlacmentFlags : int {
		x_Right = 0x1,
		x_Center = 0x2,
		x_Left = 0x4,
		y_Top = 0x8,
		y_Center = 0x10,
		y_Bottom = 0x20,
	};

	static TextInfo GetTextExtend(const std::string& Text, const bool& Bold = false, const bool& Italic = false, const float& Scale = 1);

	struct CursorData {
		Point<float> Offset;
		Point<float> Point;
	};

	//Pos acts as a cursor
	static void AddText(const std::string& Text, CursorData& Cursor , BufferedVertexVec<TextVertex>& Buff, int TextPlacmentFlag = x_Right | y_Top, const bool& Bold = false, const bool& Italic = false, const float& Scale = 1, MyDirection::Direction d = MyDirection::Up, const ColourType& ForgroundColor = { 1.0,1.0,1.0,1.0 }, const ColourType& BackgroundColor = { 0.0,0.0,0.0,0.0 });
	static void AddText(const std::string& Text, const Point<float>& Pos, BufferedVertexVec<TextVertex>& Buff, int TextPlacmentFlag = x_Right | y_Top, const bool& Bold = false, const bool& Italic = false, const float& Scale = 1, MyDirection::Direction d = MyDirection::Up, const ColourType& ForgroundColor = { 1.0,1.0,1.0,1.0 }, const ColourType& BackgroundColor = { 0.0,0.0,0.0,0.0 });

	/*static BufferedVertexVec<TextVertex>& GetStaticTextVerts();
	static BufferedVertexVec<TextVertex>& GetDynamicTextVerts();*/
};