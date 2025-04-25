#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "OpenGlDefines.hpp"

#include "RenderTextUtility.hpp"

class DataResourceManager;
class Renderer;
class BlockIdentifiyer;

class BlockSelector {
	DataResourceManager* ResourceManager;
	Renderer* renderer;

public:
	struct BlockInfo {
		BlockIdentifiyer Ident;
		double Zoom = 0.01;
		Eigen::Vector2f Offset = { 0,0 };
	};

	std::vector<BlockInfo> Blocks;
	std::vector<MyRectI> ClipRects;

	int HoverIndex = -1;//-1 is no hover

	bool Dirty : 1 = true;

	Eigen::Vector2f BufferedCanvasSize;

	BufferedVertexVec<TextVertex> TextVerts;

	constexpr static const float FontSize = 0.35f;
public:
	BlockSelector(DataResourceManager* ResourceManager, Renderer* renderer);

public:
	void PushInfo(const BlockInfo& Block);

	/*void PopIdentifiyer() {
		Identifiyers.pop_back();
	}*/


	//Returnes if dirty
	bool Hover(const Eigen::Vector2i& Mouse);

	//Returnes the Ident that was clicked or nullopt
	std::optional<BlockInfo> Click(const Eigen::Vector2i& Mouse);

private:

	Eigen::Affine2f GetTransform(const Eigen::Vector2f& CanvasSize) const;

	void UpdateExtendsAndText();
public:

	void Update();

	BufferedVertexVec<TextVertex>& GetTextVerts();

	//BufferedVertexVec<PointFRGBVertex> Blocks;
};