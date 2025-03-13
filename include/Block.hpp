#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "Path.hpp"

#include "OpenGlDefines.hpp"

#include <type_traits>

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

	std::vector<TwoPointIVertex> Edges;
	std::vector<TwoPointIVertex> EdgesMarked;
	std::vector<TwoPointIVertex> EdgesUnmarked;
	std::vector<PointIVertex> SpecialPoints;
	std::vector<TwoPointIVertex> Verts;

	std::vector<TwoPointIVertex> PreviewEdges;
	std::vector<PointIVertex> PreviewSpecialPoints;
	std::vector<TwoPointIVertex> PreviewVerts;

	//std::vector<std::pair<PointType, DragDirection>> PreviewData;
	std::vector<PointType> PreviewData;//Data of the Point

	//std::vector<TwoPointIRGBAVertex> Boxes;

public:
	const std::vector<TwoPointIVertex>& GetEdges(bool Preview) const;

	const std::vector<TwoPointIVertex>& GetEdgesMarked(bool Preview) const;

	const std::vector<TwoPointIVertex>& GetEdgesUnmarked(bool Preview) const;

	const std::vector<PointIVertex>& GetSpecialPoints(bool Preview) const;

	const std::vector<TwoPointIVertex>& GetVerts(bool Preview) const;

	//const std::vector<TwoPointIRGBAVertex>& GetBoxes() const;

private:
	PointType PreviewMouseCached;
	std::optional<VisualPath> PreviewCached;
	bool PreviewIsDirty : 1 = true;
	bool PreviewBoundingBoxIsDirty : 1 = true;
	bool Dirty : 1 = true;
	bool Destructing : 1 = false;
	bool DirtyBlocks : 1 = true;

	MyRectI PreviewCachedBoundingBox;

	PointType MouseCached;
	MyRectI CachedBoundingBox;
public:

	//Renderer* renderer;

	VisualBlockInterior(const CompressedBlockData& data, DataResourceManager* ResourceManager);

	~VisualBlockInterior() noexcept;

	void UpdateVectsForVAOs(const MyRectI& ViewRect, const PointType& Mouse);
	void UpdateVectsForVAOsPreview(const MyRectI& ViewRect, const PointType& Mouse);

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

	bool hasMark(bool Preview) const;
	bool hasUnmarked(bool Preview) const { return !GetEdgesUnmarked(Preview).empty(); }

	std::optional<VisualPath> GeneratePreviewPath(const PointType& Mouse);

	std::optional<VisualPath> GeneratePreviewPath();

	void StartDrag(const PointType& p);

	//std::vector<MyRectI> GetBoundingBox();

	//Finised
	bool AddDrag(const PointType& mouse);

	void EndDrag();

	void CancleDrag();

	bool HasPreview() const;
private:

	std::unordered_map<CompressedBlockDataIndex, std::vector<BlockMetadata>> Blocks;

	std::vector<PointIOrientationRGBVertex> PinVerts;
	std::vector<TwoPointIRGBAVertex> BlockVerts;
	std::vector<SevenSegVertex> SevenSegVerts;
	std::vector<SixteenSegVertex> SixteenSegVerts;

public:
	using BlockIndex = std::pair<CompressedBlockDataIndex, unsigned int>;

	void AddBlock(const CompressedBlockDataIndex& bedi, const BlockMetadata& Transform);

	//Invalidates iterators/reverences
	bool RemoveBlock(const BlockIndex& index);

	std::optional<BlockMetadata> GetBlockMetadata(const BlockIndex& index);

	void SetBlockMetadata(const BlockIndex& index, const BlockMetadata& Transform);

private:
	void UpdateBlocks();
public:

	const std::vector<PointIOrientationRGBVertex>& GetPinVerts() const;
	const std::vector<TwoPointIRGBAVertex>& GetBlockVerts() const;
	const std::vector<SevenSegVertex>& GetSevenSegVerts() const;
	const std::vector<SixteenSegVertex>& GetSixteenSegVerts() const;
private:

	std::optional<std::pair<CompressedBlockDataIndex, BlockMetadata>> PlacingBlock;
public:

	void StartPlacingBlock(const CompressedBlockDataIndex& bedi, const BlockMetadata& Transform);
	std::optional<std::reference_wrapper<BlockMetadata>> GetPlacingBlockTransform();
	void EndPlacingBlock();
	void CancalePlacingBlock();

	//std::string CollisionMapToString() const;
};