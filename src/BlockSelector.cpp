#include "pch.hpp"

#include "BlockSelector.hpp"

#include "Renderer.hpp"

#include "DataResourceManager.hpp"

#include "MyFrame.hpp"
#include <sys/types.h>

BlockSelector::BlockSelector(DataResourceManager* ResourceManager, Renderer* renderer)
	: ResourceManager(ResourceManager), renderer(renderer)
{
	Blocks.push_back({ BlockIdentifiyer::CreateNewBlockIdent });
}

void BlockSelector::PushInfo(const BlockInfo& Block) {
	Blocks.back().Zoom= Block.Zoom;
	Blocks.back().Offset = Block.Offset;
	Blocks.emplace_back(Block.Ident,0.01,Eigen::Vector2f{0,0});
	UpdateExtendsAndText();
}

bool BlockSelector::Hover(const Eigen::Vector2i& Mouse) {

	// const auto& CanvasSize = renderer->CanvasSize;

	int OldHoverIndex = HoverIndex;
	HoverIndex = -1;
	for (size_t i = 0; i < ClipRects.size(); i++) {
		const auto& Rect = ClipRects[i];
		if (!Rect.Contains(Mouse)) continue;
		HoverIndex = i;
		break;
	}

	if (OldHoverIndex == HoverIndex) return false;

	UpdateExtendsAndText();

	return true;
}
std::optional<BlockSelector::BlockInfo> BlockSelector::Click(const Eigen::Vector2i& Mouse) {
	ssize_t ClickIndex = -1;
	for (size_t i = 0; i < ClipRects.size(); i++) {
		const auto& Rect = ClipRects[i];
		if (!Rect.Contains(Mouse)) continue;
		ClickIndex = i;
		break;
	}
	if (ClickIndex == -1) return std::nullopt;
	assert(ClickIndex < (ssize_t)Blocks.size());
	auto it = Blocks.begin();
	std::advance(it, ClickIndex + 1);
	Blocks.erase(it, Blocks.end());
	return Blocks[ClickIndex];
}

Eigen::Affine2f BlockSelector::GetTransform(const Eigen::Vector2f& CanvasSize) const {
	Eigen::Affine2f transform = Eigen::Affine2f::Identity();
	transform.scale(Eigen::Vector2f{ 1.0f / 0.01f, -1.0f / 0.01f });
	transform.translate(Eigen::Vector2f{
		CanvasSize.x() * 0.01f / 2.0f,
		-CanvasSize.y() * 0.01f / 2.0f
		});
	return transform;
}

void BlockSelector::UpdateExtendsAndText() {
	TextVerts.clear();
	ClipRects.clear();
	//Blocks.clear();

	const auto& CanvasSize = renderer->CanvasSize;

	RenderTextUtility::CursorData Cursor = { .Offset = {0,0}, .Pos = { -0.01f * CanvasSize.x() / 2.0f + 0.1f,0.01f * CanvasSize.y() / 2.0f - 0.1f } };

	Eigen::Affine2f transform = GetTransform(CanvasSize);

	for (ssize_t i = 0; i < (ssize_t)Blocks.size(); i++) {
		const auto& Info = Blocks[i];
		const auto& Ident = Info.Ident;
		std::string Text;
		if (i != 0) {
			Text = ">";
		}
		Text += Ident.GetName();


		auto TextExtend = RenderTextUtility::GetTextExtend(Text, i <= HoverIndex, false, FontSize);
		Eigen::Vector2f TopLeft = { Cursor.Pos.x + Cursor.Offset.x, Cursor.Pos.y + Cursor.Offset.y };
		Eigen::Vector2f BottomRight = { TopLeft.x() + TextExtend.Width , TopLeft.y() - TextExtend.EvendentHeight };

		TopLeft = transform * TopLeft;
		BottomRight = transform * BottomRight;

		ClipRects.push_back(MyRectI::FromCorners(TopLeft.cast<int>(), BottomRight.cast<int>()));

		//Blocks.Emplace(TopLeft, BottomRight, ColourType{ i*0.5f,1.0,0.5,1.0 });

		ColourType TextColour = i <= HoverIndex ? ColourType{ 1.0,1.0,0.0,1.0 } : ColourType{ 1.0,1.0,1.0,1.0 };

		RenderTextUtility::AddText(Text, Cursor, TextVerts, RenderTextUtility::x_Right | RenderTextUtility::y_Bottom, i <= HoverIndex, false, FontSize, MyDirection::Up, TextColour);

	}

	for (size_t i = 0; i < Blocks.size(); i++) {
		const auto& Info = Blocks[i];
		const auto& Ident = Info.Ident;;
		std::string Text;
		if (i != 0) {
			Text = ">";
		}
		Text += Ident.GetName();
	}
}

void BlockSelector::Update() {

	const auto& CanvasSize = renderer->CanvasSize;

	if (BufferedCanvasSize != CanvasSize) {
		BufferedCanvasSize = CanvasSize;
		Dirty = true;
	}

	if (!Dirty) return;
	Dirty = false;

	/*if (.empty()) {
		PushIdentifiyer(BlockIdentifiyer::ParsePredefined("Test:Adder"));
		PushIdentifiyer(BlockIdentifiyer::ParsePredefined("Test:What"));
		PushIdentifiyer(BlockIdentifiyer::ParsePredefined("Test:Hi"));
	}*/

	UpdateExtendsAndText();

	//Text.Clear();

	//std::string ToDisplay;
	//auto& IndexOpt = ResourceManager->InteriorData;
	//if (!IndexOpt) {
	//	ToDisplay = "Missing";
	//}
	//else {
	//	auto IdentOpt = ResourceManager->GetIdentifyer(IndexOpt.value());
	//	assert(IdentOpt);
	//	ToDisplay = IdentOpt.value().GetCombined();
	//}

	////RenderTextUtility::CursorData Cursor = { .Offset = {0,0}, .Point = { -0.01f * CanvasSize.x() / 2.0f,0.01f * CanvasSize.y() / 2.0f} };
	//RenderTextUtility::CursorData Cursor = { .Offset = {0,0}, .Point = { -0.01f * CanvasSize.x() / 2.0f + 0.1f,0.01f * CanvasSize.y() / 2.0f - 0.1f } };
	//
	//Eigen::Affine2f transform = GetTransform(CanvasSize);

	//Eigen::Vector2f Pos = transform * Eigen::Vector2f{ Cursor.Pos.x, Cursor.Pos.y };

	//ToDisplay.clear();
	//ToDisplay.append(std::to_string(Pos.x()) + ", " + std::to_string(Pos.y()));

	//RenderTextUtility::AddText(ToDisplay, Cursor, Text, RenderTextUtility::x_Right | RenderTextUtility::y_Bottom, false, false, 0.5);
	//
	/*RenderTextUtility::AddText(ToDisplay, Cursor, Text, RenderTextUtility::x_Right | RenderTextUtility::y_Bottom, false, false, 0.5);
	RenderTextUtility::AddText(ToDisplay, Cursor, Text, RenderTextUtility::x_Right | RenderTextUtility::y_Bottom, false, false, 0.5);
	RenderTextUtility::AddText(ToDisplay, Cursor, Text, RenderTextUtility::x_Right | RenderTextUtility::y_Bottom, false, false, 0.5);*/

}

BufferedVertexVec<TextVertex>& BlockSelector::GetTextVerts() {
	return TextVerts;
}
