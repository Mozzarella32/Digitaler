#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "Path.hpp"

#include "OpenGlDefines.hpp"

class Renderer;
struct PointNode;
class DataResourceManager;


//There schould only exist one
//exept for multiple editors
class VisualBlockInterior {
private:

	DataResourceManager* ResourceManager;
	Renderer* renderer;

public:

	struct PathDrawData {
		std::vector<AssetVertex>                      EdgesV;
		std::vector<AssetVertex>                      EdgesH;
		std::optional<BufferedVertexVec<AssetVertex>> Edges;
		BufferedVertexVec<AssetVertex>                Verts;
		BufferedVertexVec<AssetVertex>                IntersectionPoints;

		void clear() {
			Edges.reset();
			EdgesV.clear();
			EdgesH.clear();
			Verts.clear();
			IntersectionPoints.clear();
		}

		void append(const VisualPath::DrawData& data) {
			EdgesV.insert(std::end(EdgesV), std::begin(data.EdgesV), std::end(data.EdgesV));
			EdgesH.insert(std::end(EdgesH), std::begin(data.EdgesH), std::end(data.EdgesH));
			Verts.append(data.Verts);
			IntersectionPoints.append(data.IntersectionPoints);
		}

		BufferedVertexVec<AssetVertex>& GetEdges() {
			if(Edges.has_value()){
				return Edges.value();
			}
			Edges = BufferedVertexVec<AssetVertex>{};
			Edges.value().append(EdgesV);
			Edges.value().append(EdgesH);
			EdgesV.clear();
			EdgesH.clear();
			return Edges.value();
		}
	};

	PathDrawData normal;
	PathDrawData marked;
	PathDrawData preview;
	PathDrawData highlighted;

	std::vector<PointType> PreviewData;//Data of the Point

private:
	std::optional<VisualPath> PreviewCached;
	bool PreviewIsDirty : 1 = true;
	bool PreviewBoundingBoxIsDirty : 1 = true;
	bool Dirty : 1 = true;
	bool Destructing : 1 = false;
	bool DirtyBlocks : 1 = true;
	bool PathNeedsMerging : 1= false;
	bool Moving : 1 = false;//Used to know when to scann for split
	bool hasHighlitedPath : 1 = false;
	bool hasMarkedPath : 1 = false;

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

	bool HasAnythingMarked() const;

	bool HasMarkedPaths() const;
	bool HasMarkedBlocks() const;

	//Returns if need is to redraw
	bool SetMarked(unsigned int Mark, const bool& Value);

	bool GetMarked(unsigned int Mark) const;

	bool HasMarkedPathAt(const PointType& Mouse) const;

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

	//Carefull BB calculation messes with opengl
	void UpdateVectsForVAOs(const MyRectF& ViewRect, const PointType& Mouse, bool AllowHover);
	
	//Carefull BB calculation messes with opengl
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
private:

	std::unordered_map<CompressedBlockDataIndex, std::vector<BlockMetadata>> Blocks;

public:
	BufferedVertexVec<AssetVertex> AssetVBO;

	BufferedVertexVec<AssetVertex> HighlightAssetVBO;
	BufferedVertexVec<AssetVertex> MarkedAssetVBO;

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
	void ShowMultiplicity(const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin);

	void ShowLable(const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin);

	void ShowBlockLabl(const PointType& BlockSize, const BlockMetadata& Meta, const std::string& Name);

private:
	void UpdateBlocks();
};
