#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "BlockAndPathData.hpp"

//Is Works as FreeListElements in VisualBlockInterior 
class VisualPath {
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

	std::vector<TwoPointIRGBVertex> Edges;
	std::vector<PointIVertex> SpecialPoints;
	std::vector<TwoPointIRGBVertex> Verts;

	MyRectI CachedBoundingBox;

public:
	VisualPath(VisualPathData&& Data);
	//VisualPath(const VisualPath& Other) 
	//	:Data(Other.Data),CachedBoundingBox(MyRectI()){}

	bool IsMarked() { return Marked; }
	const bool IsMarked() const { return Marked; }
	void SetMarked(bool Marked) { this->Marked = Marked; }

	//Cached therfore not const
	const std::vector<TwoPointIRGBVertex>& ComputeAllAndGetEdges(const MyRectI& BoundingBox);

	const std::vector<PointIVertex>& getSpecialPoints() const;
	const std::vector<TwoPointIRGBVertex>& getVerts() const;

	bool Intercept(const PointType& Pos) const;

	bool TryAbsorb(VisualPath& Other);

	void RotateCW(const PointType& Pos) {
		IsDirty = true;
		Data.RotateAroundCW(Pos);
	}

	void RotateCCW(const PointType& Pos) {
		IsDirty = true;
		Data.RotateAroundCCW(Pos);
	}

	void RotateHW(const PointType& Pos) {
		IsDirty = true;
		Data.RotateAroundHW(Pos);
	}

	void FlipX(const int& pos) {
		IsDirty = true;
		Data.FlipX(pos);
	}

	void FlipY(const int& pos) {
		IsDirty = true;
		Data.FlipY(pos);
	}

	//Returnes the Next Free
	PathIndex Init(VisualPathData&& pd);

	//Set this head afterwareds
	void Free(const PathIndex& head);

	bool IsFree() const;

	//modify....
};