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
	//bool Marked : 1 = false;
	bool Hover : 1 = false;
	bool Preview : 1 = false;
	bool Dirty : 1 = true;
	bool ThisIsFree : 1 = false;
	bool FullyMarked : 1 = false;
	bool NeedsMergingAfterMove : 1 = false;
	bool DontShowHover : 1 = false;
public:
	VisualPathData Data;
private:

	std::vector<TwoPointIRGBRHGHBHVertex> EdgesUnmarked;
	std::vector<TwoPointIRGBRHGHBHVertex> EdgesMarked;

	//If a vert is touched by at leas two lines
	std::vector<PointIRGBVertex> SpecialPoints;
	std::vector<TwoPointIRGBRHGHBHVertex> Verts;

	//If a vert toutches a marked and a unmarked line
	std::vector<TwoPointIRGBRHGHBHVertex> ConflictPoints;

	MyRectI CachedBoundingBox;

	std::optional<MyRectI> MarkedBoundingBox;

	std::unordered_map<LineIndex, bool> LineMarked;

public:
	VisualPath(VisualPathData&& Data);
	//VisualPath(const VisualPath& Other) 
	//	:Data(Other.Data),CachedBoundingBox(MyRectI()){}

	//bool IsMarked() { return Marked; }
	//const bool IsMarked() const { return Marked; }
	void SetHover(bool Hover);
	//void SetMarked(bool Marked);

	bool ToggleMarkedIfHover();
	void MarkAll();
	//void UnmarkAll();

	void SetPreview(bool Preview);

	//Returns if sth has changed
	bool SetMarkedArea(const MyRectI& MarkedBB);
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

	const std::vector<TwoPointIRGBRHGHBHVertex>& getEdgesUnmarked() const;
	const std::vector<TwoPointIRGBRHGHBHVertex>& getEdgesMarked() const;
	const std::vector<PointIRGBVertex>& getSpecialPoints() const;
	const std::vector<TwoPointIRGBRHGHBHVertex>& getVerts() const;
	const std::vector<TwoPointIRGBRHGHBHVertex>& getConflictPoints() const;

	bool Intercept(const PointType& Pos) const;

	bool TryAbsorb(VisualPathData& Other);

	void Rotate(const PointType& Pos, bool CW);
	//void RotateCCW(const PointType& Pos);
	//void RotateHW(const PointType& Pos);

	void Flip(const int& pos, bool X);
	//void FlipY(const int& pos);

	void Move(const PointType& Diff);

	//Returnes the Next Free
	PathIndex Init(VisualPathData&& pd);

	//Set this head afterwareds
	void Free(const PathIndex& head);

	bool IsFree() const;

	//modify....
};