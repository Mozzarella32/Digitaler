#pragma once

#include "pch.h"

#include "Include.h"

#include "BlockAndPathData.h"

class VisualPath {
	/*class State {
		bool Marked : 1 = false;
		bool Green : 1 = false;
	};*/
public:
	using PathIndex = unsigned int;

	static constexpr const PathIndex FreeListEnd = PathIndex(-1);

private:
	bool Marked : 1 = false;
	bool IsDirty : 1 = true;
	bool ThisIsFree : 1 = false;
public:
	VisualPathData Data;
private:

	std::vector<PathVertex> Edges;
	std::vector<PointVertex> SpecialPoints;
	std::vector<PathVertex> Verts;

	MyRectI CachedBoundingBox;

public:
	VisualPath(VisualPathData&& Data);
	//VisualPath(const VisualPath& Other) 
	//	:Data(Other.Data),CachedBoundingBox(MyRectI()){}

	bool isMarked() { return Marked; }
	const bool isMarked() const { return Marked; }
	void setMarked(bool Marked) { this->Marked = Marked; }

	//Cached therfore not const
	const std::vector<PathVertex>& ComputeAllAndGetEdges(const MyRectI& BoundingBox);

	const std::vector<PointVertex>& getSpecialPoints() const;
	const std::vector<PathVertex>& getVerts() const;

	bool Intercept(const PointType& Pos) const;

	bool TryAbsorb(VisualPath& Other);

	//Returnes the Next Free
	PathIndex Init(VisualPathData&& pd) {
		assert(!IsFree());
		PathIndex PathIndexNext = CachedBoundingBox.Position.x();
		Data = std::move(pd);
		ThisIsFree = false;
		return PathIndexNext;
	}

	//Set this head afterwareds
	void Free(const PathIndex& head) {
		Data = {};
		CachedBoundingBox = {};
		Edges.clear();
		SpecialPoints.clear();
		Verts.clear();
		IsDirty = true;
		Marked = false;
		CachedBoundingBox.Position.x() = head;
		ThisIsFree = true;
	}

	bool IsFree() const {
		return ThisIsFree;
	}

	//modify....
};