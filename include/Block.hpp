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


//There schould only exist one
//exept for multiple editors
class VisualBlockInterior {
private:

	DataResourceManager* ResourceManager;
	Renderer* renderer;

	BufferedVertexVec<AssetVertex> Edges;
	BufferedVertexVec<AssetVertex> EdgesMarked;
	BufferedVertexVec<AssetVertex> IntersectionPoints;
	BufferedVertexVec<AssetVertex> Verts;

	BufferedVertexVec<AssetVertex> FloatingEdges;
	BufferedVertexVec<AssetVertex> FloatingIntersectionPoints;
	BufferedVertexVec<AssetVertex> FloatingVerts;

	BufferedVertexVec<TextVertex> StaticTextVBO;
	BufferedVertexVec<TextVertex> DynamicTextVBO;

	std::vector<PointType> PreviewData;//Data of the Point

public:
	BufferedVertexVec<AssetVertex>& GetEdges(bool Floating);

	BufferedVertexVec<AssetVertex>& GetEdgesMarked(bool Floating);

	BufferedVertexVec<AssetVertex>& GetIntersectionPoints(bool Floating);

	BufferedVertexVec<AssetVertex>& GetVerts(bool Floating);

private:
	std::optional<VisualPath> PreviewCached;
	bool PreviewIsDirty : 1 = true;
	bool PreviewBoundingBoxIsDirty : 1 = true;
	bool Dirty : 1 = true;
	bool Destructing : 1 = false;
	bool DirtyBlocks : 1 = true;
	bool PathNeedsMerging : 1= false;
	bool Moving : 1 = false;//Used to know when to scann for split
	bool hasHighlitedPath = false;

	MyRectF PreviewCachedBoundingBox;

	PointType MouseCached;
	MyRectF CachedBoundingBox;

	PointType MouseCachedPreview;
	MyRectF CachedBoundingBoxPreview;

	ssize_t Highlited;

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
	bool SetHighlited(int Highlited);

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
	// const std::array<MyRectF, 4>& GetBlockBoundingBoxes(const CompressedBlockDataIndex& cbi);

	//Bind Context befor this
	void MarkArea(const MyRectF& Area/*, BlockBoundingBoxCallback bbbc*/);

public:
	static PointType GetPositionDiff(const BlockMetadata& Meta, const PointType& BlockSize);

	static PointType GetBasePosition(const BlockMetadata& Meta, const PointType& BlockSize);
private:

	Eigen::Vector2f GetMarkedMean() const;

public:

	//Returnes if Modifyed
	bool RotateMarked(bool CW);

	//Returnes if Modifyed
	bool FlipMarked(bool X);

	void MoveMarked(const PointType& Diff);

	//Returnes if Modifyed
	bool DeleteMarked();

	void BBUpdate();

	VisualBlockInterior(DataResourceManager* ResourceManager, Renderer* renderer);

	//Looses its state, musst be updated before next action
	void WriteBack(CompressedBlockData& Data);

	void UpdateCurrentBlock();

	~VisualBlockInterior() noexcept;

	bool HasHighlitedPath() const;

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

	std::optional<VisualPathData> GeneratePreviewPath(const PointType& Mouse);

	std::optional<VisualPathData> GeneratePreviewPath();

	size_t GetDragSize() const;
	void StartDrag(const PointType& p);

	//Finished
	bool AddDrag(const PointType& mouse);

	void EndDrag();

	void CancleDrag();

	bool HasPreview() const;

	bool HasFloating() const;
private:

	std::unordered_map<CompressedBlockDataIndex, std::vector<BlockMetadata>> Blocks;

	BufferedVertexVec<AssetVertex> PinVBO;
	BufferedVertexVec<AssetVertex> AssetVBO;
	BufferedVertexVec<AssetVertex> RoundPinVBO;

#ifdef ShowBasePositionOfBlocks
	BufferedVertexVec<AssetFVertex> BasePositionVBO;
#endif

#ifdef ShowBoundingBoxes
	BufferedVertexVec<AssetFVertex> BBVBO;
#endif

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

	BufferedVertexVec<AssetVertex>& GetPinVBO();
	BufferedVertexVec<AssetVertex>& GetAssetVBO();
	BufferedVertexVec<AssetVertex>& GetRoundPinVBO();
	BufferedVertexVec<TextVertex>& GetStaticTextVBO();
	BufferedVertexVec<TextVertex>& GetDynamicTextVBO();

#ifdef ShowBasePositionOfBlocks
	BufferedVertexVec<AssetFVertex>& GetBasePotitionOfBlocksVBO();
#endif

#ifdef ShowBoundingBoxes
	BufferedVertexVec<AssetFVertex>& GetBBVBO();
#endif
};
