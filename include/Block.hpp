#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "Path.hpp"

class Renderer;
struct PointNode;

class VisualBlock {
private:
	/*enum DragDirection {
		Horizontal,
		Vertical,
		Unset
	};*/

private:

	std::vector<PathVertex> Edges;
	std::vector<PathVertex> EdgesMarked;
	std::vector<PathVertex> EdgesUnmarked;
	std::vector<PointVertex> SpecialPoints;
	std::vector<PathVertex> Verts;

	std::vector<PathVertex> PreviewEdges;
	std::vector<PointVertex> PreviewSpecialPoints;
	std::vector<PathVertex> PreviewVerts;

	//std::vector<std::pair<PointType, DragDirection>> PreviewData;
	std::vector<PointType> PreviewData;

	//std::vector<RectRGBAVertex> Boxes;

public:
	const std::vector<PathVertex>& GetEdges(bool Preview) const;

	const std::vector<PathVertex>& GetEdgesMarked(bool Preview) const;

	const std::vector<PathVertex>& GetEdgesUnmarked(bool Preview) const;

	const std::vector<PointVertex>& GetSpecialPoints(bool Preview) const;

	const std::vector<PathVertex>& GetVerts(bool Preview) const;

	//const std::vector<RectRGBAVertex>& GetBoxes() const;

	PointType PreviewMouseCached;
	std::optional<VisualPath> PreviewCached;
	bool PreviewIsDirty : 1 = true;
	bool PreviewBoundingBoxIsDirty : 1 = true;
	bool Dirty : 1 = true;
	bool Destructing : 1 = false;
	MyRectI PreviewCachedBoundingBox;

	PointType MouseCached;
	MyRectI CachedBoundingBox;
public:

	Renderer* renderer;

	VisualBlock(int FrameCount);

	~VisualBlock() noexcept {
		Destructing = true;
	}

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

	//std::string CollisionMapToString() const;
};