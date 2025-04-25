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

	BufferedVertexVec<TwoPointIRGBRHGHBHVertex> Edges;
	BufferedVertexVec<TwoPointIRGBRHGHBHVertex> EdgesMarked;
	BufferedVertexVec<TwoPointIRGBRHGHBHVertex> EdgesUnmarked;
	BufferedVertexVec<PointIRGBVertex> SpecialPoints;
	BufferedVertexVec<TwoPointIRGBRHGHBHVertex> Verts;
	BufferedVertexVec<TwoPointIRGBRHGHBHVertex> ConflictPoints;

	BufferedVertexVec<TwoPointIRGBRHGHBHVertex> FloatingEdges;
	BufferedVertexVec<PointIRGBVertex> FloatingSpecialPoints;
	BufferedVertexVec<TwoPointIRGBRHGHBHVertex> FloatingVerts;

	BufferedVertexVec<TextVertex> StaticTextVerts;
	BufferedVertexVec<TextVertex> DynamicTextVerts;

	//BufferedVertexVec <std::pair<PointType, DragDirection>> PreviewData;
	std::vector<PointType> PreviewData;//Data of the Point

	//BufferedVertexVec <TwoPointIRGBAVertex> Boxes;

public:
	BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& GetEdges(bool Floating);

	BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& GetEdgesMarked(bool Floating);

	BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& GetEdgesUnmarked(bool Floating);

	BufferedVertexVec<PointIRGBVertex>& GetSpecialPoints(bool Floating);

	BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& GetVerts(bool Floating);

	BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& GetConflictPoints(bool Floating);

	//const std::vector<TwoPointIRGBAVertex>& GetBoxes() const;

private:
	std::optional<VisualPath> PreviewCached;
	bool PreviewIsDirty : 1 = true;
	bool PreviewBoundingBoxIsDirty : 1 = true;
	bool Dirty : 1 = true;
	bool Destructing : 1 = false;
	bool DirtyBlocks : 1 = true;
	bool PathNeedsMerging : 1= false;
	bool Moving : 1 = false;//Used to know when to scann for split

	//bool HasSthMarked : 1 = false;

	MyRectF PreviewCachedBoundingBox;

	PointType MouseCached;
	MyRectF CachedBoundingBox;

	PointType MouseCachedPreview;
	MyRectF CachedBoundingBoxPreview;

	unsigned int Highlited;

	//Carefull index 0 is unused
	std::vector<bool> MarkedBlocks;
	unsigned int NumMarkedBlocks = 0;

	using BlockApplyer = std::function<void(BlockMetadata&, const PointType&)>;
	using PathApplyer = std::function<void(VisualPath&)>;

	//Returnes if Modifyed
	bool ApplyToMarked(const BlockApplyer& Blockapplyer, const PathApplyer& Pathapplyer);

	//Returnes if Modifyed
	bool ApplyToMarkedPath(const PathApplyer& PathApplyer);

public:



	//Returns if need is to redraw
	bool SetHighlited(unsigned int Highlited);

	unsigned int GetHighlited() const;
	//Renderer* renderer;

	bool HasHighlited() const;

	std::optional<CompressedBlockDataIndex> GetBlockIdByStencil(unsigned int Id) const;

	void ClearMarked();
	void MarkAll();
	bool ToggleMarkHoverPath();

	//Carefull O(n) with n = Paths.size()
	bool HasAnythingMarked() const;

	//Carefull O(n) with n = Paths.size()
	bool HasMarkedPaths() const;
	bool HasMarkedBlocks() const;

	//Returns if need is to redraw
	bool SetMarked(unsigned int Mark, const bool& Value);

	bool GetMarked(unsigned int Mark) const;

	bool HasMarkedPathAt(const PointType& Mouse) const;

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
	//PointType RotateMarked();

private:
	//Returns the axis
	//int FlipMarkedBlocks(bool X);

	//int FlipMarkedBlocksX();
	//int FlipMarkedBlocksY();
public:

	//Returnes if Modifyed
	bool RotateMarked(bool CW);

	//void RotateMarkedCCW(const PointType& Pos);

	//void RotateMarkedHW(const PointType& Pos);

	//Returnes if Modifyed
	bool FlipMarked(bool X);

	//void FlipMarkedY(const int& pos);

	void MoveMarked(const PointType& Diff);

	//Returnes if Modifyed
	bool DeleteMarked();

	VisualBlockInterior(DataResourceManager* ResourceManager, Renderer* renderer);

	//Looses its state, musst be updated before next action
	void WriteBack(CompressedBlockData& Data);

	void UpdateCurrentBlock();

	~VisualBlockInterior() noexcept;

	void UpdateVectsForVAOs(const MyRectF& ViewRect, const float& Zoom, const PointType& Mouse, bool AllowHover);
	void UpdateVectsForVAOsFloating(const MyRectF& ViewRect, const PointType& Mouse);

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

	bool TryAbsorb(VisualPathData& Path);

	void AbsorbOrAdd(VisualPathData&& Data);

	//Be carfull, may invalidate Paths ref/ptr/it
	VisualPath& SplitPath(size_t PathIndex);

	//Does not merge two how need merging
	void MergeAfterMove();
public:


	bool HasMark(bool Preview);
	bool HasUnmarked(bool Preview);

	std::optional<VisualPathData> GeneratePreviewPath(const PointType& Mouse);

	std::optional<VisualPathData> GeneratePreviewPath();

	size_t GetDragSize() const;
	void StartDrag(const PointType& p);

	//std::vector<MyRectI> GetBoundingBox();

	//Finised
	bool AddDrag(const PointType& mouse);

	void EndDrag();

	void CancleDrag();

	bool HasPreview() const;

	bool HasFloating() const;
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

#ifdef ShowBasePositionOfBlocks
	BufferedVertexVec<PointFRGBVertex> BasePositionVerts;
#endif
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

#ifdef ShowBasePositionOfBlocks
	BufferedVertexVec<PointFRGBVertex>& GetBasePotitionOfBlocksVerts();
#endif
};