#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "Path.hpp"

#include "OpenGlDefines.hpp"

#include <type_traits>

#include <stdbool.h>

class Renderer;
struct PointNode;
class DataResourceManager;


//
//class VisualBlockExterior {
//
//};

//There schould only exist one
//exept for multiple editors
class VisualBlockInterior {
private:
	/*enum DragDirection {
		Horizontal,
		Vertical,
		Unset
	};*/

private:

	DataResourceManager* ResourceManager;
	Renderer* renderer;

	BufferedVertexVec<TwoPointIRGBVertex> Edges;
	BufferedVertexVec<TwoPointIRGBVertex> EdgesMarked;
	BufferedVertexVec<TwoPointIRGBVertex> EdgesUnmarked;
	BufferedVertexVec<PointIVertex> SpecialPoints;
	BufferedVertexVec<TwoPointIRGBVertex> Verts;

	BufferedVertexVec<TwoPointIRGBVertex> PreviewEdges;
	BufferedVertexVec<PointIVertex> PreviewSpecialPoints;
	BufferedVertexVec<TwoPointIRGBVertex> PreviewVerts;

	BufferedVertexVec<TextVertex> StaticTextVerts;
	BufferedVertexVec<TextVertex> DynamicTextVerts;

	//BufferedVertexVec <std::pair<PointType, DragDirection>> PreviewData;
	std::vector<PointType> PreviewData;//Data of the Point

	//BufferedVertexVec <TwoPointIRGBAVertex> Boxes;

public:
	BufferedVertexVec<TwoPointIRGBVertex>& GetEdges(bool Preview);

	BufferedVertexVec<TwoPointIRGBVertex>& GetEdgesMarked(bool Preview);

	BufferedVertexVec<TwoPointIRGBVertex>& GetEdgesUnmarked(bool Preview);

	BufferedVertexVec<PointIVertex>& GetSpecialPoints(bool Preview);

	BufferedVertexVec<TwoPointIRGBVertex>& GetVerts(bool Preview);

	//const std::vector<TwoPointIRGBAVertex>& GetBoxes() const;

private:
	struct CharInfo {
		float Advance;
		//l t r b
		std::array<float, 4> CursorOffsets;
		std::array<float, 4> Clip;
	};

	const std::map<int, CharInfo> CharMap;

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

	int GetCharMapIndex(const char& c, const bool& bold, const bool& italic);
public:

	enum TextPlacmentFlags : int {
		x_Right = 0x1,
		x_Center = 0x2,
		x_Left = 0x4,
		y_Top = 0x8,
		y_Center = 0x10,
		y_Bottom = 0x20,
	};

	TextInfo GetTextExtend(const std::string& Text, const bool& Bold = false, const bool& Italic = false, const float& Scale = 1);

	void AddText(const std::string& Text, const Point<float>& Pos, const bool& Static, int TextPlacmentFlag = x_Right | y_Top, const bool& Bold = false, const bool& Italic = false, const float& Scale = 1, MyDirection::Direction d = MyDirection::Up, const ColourType& ForgroundColor = { 1.0,1.0,1.0,1.0 }, const ColourType& BackgroundColor = { 0.0,0.0,0.0,0.0 });


private:
	std::optional<VisualPath> PreviewCached;
	bool PreviewIsDirty : 1 = true;
	bool PreviewBoundingBoxIsDirty : 1 = true;
	bool Dirty : 1 = true;
	bool Destructing : 1 = false;
	bool DirtyBlocks : 1 = true;

	MyRectF PreviewCachedBoundingBox;

	PointType MouseCached;
	MyRectF CachedBoundingBox;

	PointType MouseCachedPreview;
	MyRectF CachedBoundingBoxPreview;

	unsigned int Highlited;

	//Carefull index 0 is unused
	std::vector<bool> MarkedBlocks;

public:

	//Returns if need is to redraw
	bool SetHighlited(unsigned int Highlited);

	unsigned int GetHighlited() const;
	//Renderer* renderer;

	bool HasHighlited() const;

	void ClearMarked();
	void MarkAll();

	//Returns if need is to redraw
	bool SetMarked(unsigned int Mark, const bool& Value);

	bool GertMarked(unsigned int Mark) const;

	void MoveMarked(const Eigen::Vector2i& Diff);

	void DeleteMarked();

	//using BlockBoundingBoxCallback = std::function<const std::array<MyRectF, 4>& (const CompressedBlockDataIndex&)>;
	const std::array<MyRectF, 4>& GetBlockBoundingBoxes(const CompressedBlockDataIndex& cbi);

	//Bind Context befor this
	void MarkArea(const MyRectF& Area/*, BlockBoundingBoxCallback bbbc*/);

public:
	static PointType GetPositionDiff(const BlockMetadata& Meta, const PointType& BlockSize);

	static PointType GetBasePosition(const BlockMetadata& Meta, const PointType& BlockSize);
private:

	Eigen::Vector2f GetMarkedMean() const;

public:
	//Returnes Mean Position or InvalidPoint
	PointType RotateMarked();

private:
	//Returns the axis
	int FlipMarked(bool X);

public:
	int FlipMarkedX();
	int FlipMarkedY();

	void RotateCW(const PointType& Pos) {
		Dirty = true;
		for (auto& Path : Paths) {
			if (Path.IsFree()) continue;
			Path.RotateCW(Pos);
		}
	}

	void RotateCCW(const PointType& Pos) {
		Dirty = true;
		for (auto& Path : Paths) {
			if (Path.IsFree()) continue;
			Path.RotateCCW(Pos);
		}
	}

	void RotateHW(const PointType& Pos) {
		Dirty = true;
		for (auto& Path : Paths) {
			if (Path.IsFree()) continue;
			Path.RotateHW(Pos);
		}
	}

	void FlipX(const int& pos) {
		Dirty = true;
		for (auto& Path : Paths) {
			if (Path.IsFree()) continue;
			Path.FlipX(pos);
		}
	}

	void FlipY(const int& pos) {
		Dirty = true;
		for (auto& Path : Paths) {
			if (Path.IsFree()) continue;
			Path.FlipY(pos);
		}
	}

	VisualBlockInterior(const CompressedBlockData& data, DataResourceManager* ResourceManager, Renderer* renderer);

	~VisualBlockInterior() noexcept;

	void UpdateVectsForVAOs(const MyRectF& ViewRect, const float& Zoom, const PointType& Mouse);
	void UpdateVectsForVAOsPreview(const MyRectF& ViewRect, const PointType& Mouse);

private:


#ifdef UseCollisionGrid
	struct LineAndPath {
		LineIndex Line;
		VisualPathData::VisualPathDataId PathId;

		bool operator==(const LineAndPath& other) const {
			return Line == other.Line && PathId == other.PathId;
		}
		bool operator!=(const LineAndPath& other) const {
			return !(*this == other);
		}
	};

public:
	//Must be befor Paths so the Block datas can be correctely destructed
	std::unordered_map<PointType, std::vector<LineAndPath>> CollisionMap;

	static int BoxSize;
#endif
private:

	VisualPath::PathIndex PathsFreeListHead = VisualPath::FreeListEnd;

	std::vector<VisualPath> Paths;


#ifdef UseCollisionGrid
	std::vector<PointType> GetBoxesFromLine(const PointType& A, const PointType& B);

public:
	void RegisterLine(const LineIndex& l, const std::vector<PointNode>& Points, const VisualPathData::VisualPathDataId& Id);

	void UnRegisterLine(const LineIndex& l, const std::vector<PointNode>& Points, const VisualPathData::VisualPathDataId& Id);

	//std::vector<LineIndex> GetLinesWith(const PointType& p, const VisualPathData& pd);
	LineIndex GetLineWith(const PointType& p, const VisualPathData& pd);
#endif

public:

	VisualPath::PathIndex AddPath(VisualPathData&& p);

	bool TryAbsorb(VisualPath& Path);
public:

	bool hasMark(bool Preview);
	bool hasUnmarked(bool Preview);

	std::optional<VisualPath> GeneratePreviewPath(const PointType& Mouse);

	std::optional<VisualPath> GeneratePreviewPath();

	size_t GetDragSize() const;
	void StartDrag(const PointType& p);

	//std::vector<MyRectI> GetBoundingBox();

	//Finised
	bool AddDrag(const PointType& mouse);

	void EndDrag();

	void CancleDrag();

	bool HasPreview() const;
private:

	std::unordered_map<CompressedBlockDataIndex, std::vector<BlockMetadata>> Blocks;

	BufferedVertexVec<PointIOrientationRGBRHGHBHIDVertex> PinVerts;
	BufferedVertexVec<TwoPointIRGBAIDVertex> BlockVerts;
	BufferedVertexVec<SevenSegVertex> SevenSegVerts;
	BufferedVertexVec<SixteenSegVertex> SixteenSegVerts;
	BufferedVertexVec<PointIRGBIDVertex> RoundedPinVerts;
	BufferedVertexVec<PointIOrientationRGBIDVertex> AndVerts;
	BufferedVertexVec<PointIOrientationRGBIDVertex> OrVerts;
	BufferedVertexVec<PointIOrientationRGBIDVertex> XOrVerts;
	BufferedVertexVec<MuxIDVertex> MuxVerts;

	/*BufferedVertexVec<PointIOrientationRGBIDVertex> NotTriangleVerts;
	BufferedVertexVec<PointIOrientationRGBIDVertex> NDotVerts;*/


public:
	using BlockIndex = std::pair<CompressedBlockDataIndex, unsigned int>;

	void AddBlock(const CompressedBlockDataIndex& bedi, const BlockMetadata& Transform);
	void AddBlockBatched(const std::map<CompressedBlockDataIndex, std::vector<BlockMetadata>>& ToInsert);

	//Invalidates iterators/reverences
	bool RemoveBlock(const BlockIndex& index);

	std::optional<BlockMetadata> GetBlockMetadata(const BlockIndex& index);

	void SetBlockMetadata(const BlockIndex& index, const BlockMetadata& Transform);

	static PointType GetPinPosition(const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin, const int& Expoltion);
	static MyDirection::Direction GetPinRotation(const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin);

private:
	void ShowMultiplicity(const float& Zoom, const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin);

	void ShowLable(const float& Zoom, const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin);

	void ShowBlockLabl(const PointType& BlockSize, const BlockMetadata& Meta, const std::string& Name);

private:
	void UpdateBlocks(const float& Zoom);
public:

	BufferedVertexVec<PointIOrientationRGBRHGHBHIDVertex>& GetPinVerts();
	BufferedVertexVec<TwoPointIRGBAIDVertex>& GetBlockVerts();
	BufferedVertexVec<SevenSegVertex>& GetSevenSegVerts();
	BufferedVertexVec<SixteenSegVertex>& GetSixteenSegVerts();
	BufferedVertexVec<PointIRGBIDVertex>& GetRoundedPinVerts();
	BufferedVertexVec<PointIOrientationRGBIDVertex>& GetAndVerts();
	BufferedVertexVec<PointIOrientationRGBIDVertex>& GetOrVerts();
	BufferedVertexVec<PointIOrientationRGBIDVertex>& GetXOrVerts();
	/*BufferedVertexVec<PointIOrientationRGBIDVertex>& GetNotTriangleVerts();
	BufferedVertexVec<PointIOrientationRGBIDVertex>& GetNDotVerts();*/
	BufferedVertexVec<MuxIDVertex>& GetMuxVerts();
	BufferedVertexVec<TextVertex>& GetStaticTextVerts();
	BufferedVertexVec<TextVertex>& GetDynamicTextVerts();
private:

	std::optional<std::pair<CompressedBlockDataIndex, BlockMetadata>> PlacingBlock;
public:

	void StartPlacingBlock(const CompressedBlockDataIndex& bedi, const BlockMetadata& Transform);
	std::optional<std::reference_wrapper<BlockMetadata>> GetPlacingBlockTransform();
	void EndPlacingBlock();
	void CancalePlacingBlock();

	//std::string CollisionMapToString() const;
};