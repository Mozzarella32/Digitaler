#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "BlockAndPathData.hpp"

#include "OpenGlDefines.hpp"


//Is Works as FreeListElements in VisualBlockInterior 
class VisualPath {
public:
	using PathIndex = unsigned int;

	static constexpr const PathIndex FreeListEnd = PathIndex(-1);

private:
	bool Marked : 1 = false;
	bool Hover : 1 = false;
	bool Preview : 1 = false;
	bool Dirty : 1 = true;
	bool ThisIsFree : 1 = false;
	bool FullyMarked : 1 = false;
	bool NeedsMergingAfterMove : 1 = false;
	bool DontShowHover : 1 = false;
public:
	VisualPathData Data;
public:

	struct DrawData {
		std::vector<AssetVertex> Edges;
		std::vector<AssetVertex> Verts;
		//If a vert is touched by at leas two lines
		std::vector<AssetVertex> IntersectionPoints;

		void clear() {
			Edges.clear();
			Verts.clear();
			IntersectionPoints.clear();
		}
	};

	std::vector<AssetVertex> allEdges;

	DrawData normal;
	DrawData marked;

private:
	MyRectI CachedBoundingBox;

	std::optional<MyRectF> MarkedBoundingBox;

	std::unordered_map<LineIndex, bool> LineMarked;

public:
	VisualPath(VisualPathData&& Data);

	void SetHover(bool Hover);
	bool GetHover() const {
		return Hover;
	}

	bool ToggleMarkedIfHover();
	void MarkAll();

	void SetPreview(bool Preview);
	bool GetPreview() const {
		return Preview;
	}

	//Returns if sth has changed
	bool SetMarkedArea(const MyRectF& MarkedBB);

	//Returnes if previously marked
	bool ClearMarkedArea();
	bool HasMarked() const;
	bool HasMarkedAt(const PointType& Mouse) const;
	bool IsFullyMarked() const;

	void SetDontShowHover(bool value);

	//Returnes marked connected components(other than the one this is assigned to) and other coneccted components(unmarked)
	std::pair<std::vector<CompressedPathData>, std::vector<CompressedPathData>> Split();

	bool NeedsMerging() const;
	void ClearNeedsMerging();

	//Cached therfore not const
	void ComputeAll(const MyRectI& BB);

	bool Intercept(const PointType& Pos) const;

	bool TryAbsorb(VisualPathData& Other);

	void Rotate(const PointType& Pos, bool CW);

	void Flip(const int& pos, bool X);

	void Move(const PointType& Diff);

	//Returnes the Next Free
	PathIndex Init(VisualPathData&& pd);

	//Set this head afterwareds
	void Free(const PathIndex& head);

	bool IsFree() const;

	//modify....
};
