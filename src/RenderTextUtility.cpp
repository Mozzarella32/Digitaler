#include "OpenGlDefines.hpp"
#include "pch.hpp"
#include "RenderTextUtility.hpp"

#include "BlockAndPathData.hpp"

#include "AtlasMap.hpp"


RenderTextUtility& RenderTextUtility::GetInstance() {
	static RenderTextUtility This;
	return This;
}

int RenderTextUtility::GetCharMapIndex(const char& c, const bool& bold, const bool& italic) {
	return static_cast<unsigned char>(c) + (10000 * ((int)bold + (int)italic * 2));
}

RenderTextUtility::RenderTextUtility()
	:
	CharMap([]() {
	PROFILE_FUNKTION;
	std::map<int, CharInfo> CharMap;
	//         0  1  2   3 4 5 6 7 8 9 10
	//bolditallic id adv l t r b l t r b
	//                   plain   atlas
	for (ssize_t i = 0; i < (ssize_t)(sizeof(atlasMap) / sizeof(atlasMap[0]) / 11); i++) {
		CharMap[(10000 * atlasMap[i * 11]) + atlasMap[i * 11 + 1]] = {
			(float)atlasMap[i * 11 + 2],{
				(float)atlasMap[i * 11 + 3], (float)atlasMap[i * 11 + 4],
				(float)atlasMap[i * 11 + 5],(float)atlasMap[i * 11 + 6]},
			{(float)atlasMap[i * 11 + 7] / TextAtlasSize.x,(TextAtlasSize.y - (float)atlasMap[i * 11 + 8]) / TextAtlasSize.y,
			(float)atlasMap[i * 11 + 9] / TextAtlasSize.x,(TextAtlasSize.y - (float)atlasMap[i * 11 + 10]) / TextAtlasSize.y} };
	}
	return CharMap;
		}()) {
}

RenderTextUtility::TextInfo RenderTextUtility::GetTextExtend(const std::string& Text, const bool& Bold, const bool& Italic, const float& Scale) {
	RenderTextUtility& This = GetInstance();

	TextInfo i;
	float Highest = -LineHeight * Scale;
	float Lowest = LineHeight * Scale;
	i.LineWidths.push_back(0);
	float y = 0;

	if (!This.CharMap.contains(GetCharMapIndex('?', Bold, Italic)))return{};

	static CharInfo ciQuestion = This.CharMap.at(GetCharMapIndex('?', Bold, Italic));


	for (const auto& c : Text) {
		if (c == '\n') {
			y -= LineHeight * Scale;
			i.LineWidths.push_back(0);
			continue;
		}

		const CharInfo& ci = This.CharMap.contains(GetCharMapIndex(c, Bold, Italic))
			? This.CharMap.at(GetCharMapIndex(c, Bold, Italic))
			: ciQuestion;

		i.LineWidths.back() += ci.Advance * Scale;
		if (i.LineWidths.size() == 1)
			Highest = std::max(Highest, ci.CursorOffsets[3] * Scale);
		Lowest = std::min(Highest, y + ci.CursorOffsets[1] * Scale);
	}
	i.Width = *std::min_element(i.LineWidths.begin(), i.LineWidths.end());
	i.TheoreticalHeight = std::abs(TextAscender * Scale) + std::abs(TextDescender * Scale)
		+ (i.LineWidths.size() - 1) * LineHeight * Scale;
	i.EvendentHeight = Highest - Lowest;
	return i;
}

void RenderTextUtility::AddText(const std::string& Text, CursorData& Cursor, BufferedVertexVec<AssetVertex>& Buff, int TextPlacmentFlag, const bool& Bold, const bool& Italic, const float& Scale, MyDirection::Direction d, const ColourType& ForgroundColor, const ColourType& BackgroundColor) {
	using enum TextPlacmentFlags;

	RenderTextUtility& This = GetInstance();

	Point<float> OFF = { 0,0 };

	Point<float> Up;
	Point<float> Right;

	Up += d;
	Right += MyDirection::RotateCW(d);

	if (d == MyDirection::Left || d == MyDirection::Right) {
		Right.y *= -1;
		Up.x *= -1;
	}

	if (!This.CharMap.contains(GetCharMapIndex('?', Bold, Italic)))return;

	static CharInfo ciQuestion = This.CharMap.at(GetCharMapIndex('?', Bold, Italic));

	Point<float> OFFAlignment = { 0,0 };

	if (TextPlacmentFlag != (x_Right | y_Top)) {
		auto extend = GetTextExtend(Text, Bold, Italic, Scale);

		if (TextPlacmentFlag & x_Center) {
			OFFAlignment -= Right * extend.Width / 2.0;
		}

		if (TextPlacmentFlag & y_Center) {
			OFFAlignment += Up * extend.EvendentHeight / 2.0;
		}

		if (TextPlacmentFlag & x_Left) {
			OFFAlignment -= Right * extend.Width;
		}

		if (TextPlacmentFlag & y_Bottom) {
			OFFAlignment += Up * extend.EvendentHeight;
		}
	}

	OFF += OFFAlignment;

	for (const char& c : Text) {
		if (c == '\n') {
			OFF -= (OFF* Right * Right);//x = 0
			Cursor.Offset -= Cursor.Offset * Right * Right;
			OFF += LineHeight * Up * Scale;
			continue;
		}


		CharInfo ci = This.CharMap.contains(GetCharMapIndex(c, Bold, Italic)) ?
			This.CharMap.at(GetCharMapIndex(c, Bold, Italic))
			: ciQuestion;


		if (c == ' ') {
			OFF += ci.Advance * Right * Scale;
			continue;
		}

		//Buff.Dirty = true;
		Buff.emplace(
			AssetVertex::Text(Cursor.Pos + Cursor.Offset + OFF, ci.CursorOffsets.data(), ci.Clip.data(), Scale, d, ForgroundColor, BackgroundColor)
		);

		OFF += ci.Advance * Right * Scale;
	}

	Cursor.Offset += OFF;
	//Cursor.Offset += Alig
	Cursor.Offset -= OFFAlignment;
}

void RenderTextUtility::AddText(const std::string& Text, const Point<float>& Pos, BufferedVertexVec<AssetVertex>& Buff, int TextPlacmentFlag, const bool& Bold, const bool& Italic, const float& Scale, MyDirection::Direction d, const ColourType& ForgroundColor, const ColourType& BackgroundColor) {
	CursorData Cursor{ .Offset = {0,0}, .Pos = Pos};
	AddText(Text, Cursor, Buff, TextPlacmentFlag, Bold, Italic, Scale, d, ForgroundColor, BackgroundColor);
}
