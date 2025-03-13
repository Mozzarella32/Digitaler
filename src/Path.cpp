#include "pch.hpp"
#include "Path.hpp"

#include "OpenGlDefines.hpp"

VisualPath::VisualPath(VisualPathData&& Data)
	:Data(std::move(Data)) {
}

static const std::vector<TwoPointIVertex> EmptyVerts;

const std::vector<TwoPointIVertex>& VisualPath::ComputeAllAndGetEdges(const MyRectI& BoundingBox) {
	if (!Data.BoundingBox.Intersectes(BoundingBox)) return EmptyVerts;

	if (CachedBoundingBox != BoundingBox) {
		CachedBoundingBox = BoundingBox;
		IsDirty = true;
	}

	if (!IsDirty) {
		return Edges;
	}
	IsDirty = false;
	Edges.clear();
	SpecialPoints.clear();
	Verts.clear();

	//Calculate Edges and SpecialPoints
	std::unordered_set<PointIVertex> specialPoints;
	/*Edges.reserve(Data.Lines.size());

	auto hr = Data.toHumanReadable();

	for (const auto& pair : Data.Lines) {
		const auto& A = Data.Points[pair.first];
		const auto& B = Data.Points[pair.second];
		assert(A != B && "You Messed Up, Points Of line are identical");

		if (MyRectI::FromCorners(A, B).Intersectes(BoundingBox)) {
			if (BoundingBox.Contains(A) && Data.GetConnectionCount(pair.first) >= 2) {
				specialPoints.emplace(A);
			}
			if (BoundingBox.Contains(B) && Data.GetConnectionCount(pair.second) >= 2) {
				specialPoints.emplace(B);
			}
			Edges.emplace_back(A, B);
		}
	}
	Edges.shrink_to_fit();*/

	for (PointIndex i = 0; i < Data.Points.size(); i++) {
		const PointNode& p = Data.Points[i];
		if (p.IsFree())continue;
		for (LineIndexInPoint j = 0; j < 4; j++) {
			const PointIndex& OtherIndex = p.Connections[j];
			if (OtherIndex < i)continue;
			if (OtherIndex == InvalidPointIndex || OtherIndex == ReservedPointIndex)break;
			const PointNode& other = Data.Points[OtherIndex];

			const PointType& A = p.Pos;
			const PointType& B = other.Pos;

			assert(A != B && "You Messed Up, Points Of line are identical");

			if (MyRectI::FromCorners(A, B).Intersectes(BoundingBox)) {
				if (BoundingBox.Contains(A) && p.ConnectionCount()  >= 2) {
					specialPoints.emplace(A);
				}
				if (BoundingBox.Contains(B) && other.ConnectionCount() >= 2) {
					specialPoints.emplace(B);
				}
				Edges.emplace_back(A, B);
			}
		}
	}

	SpecialPoints = std::vector(specialPoints.begin(), specialPoints.end());

	//Calculate Verts
	std::unordered_set<TwoPointIVertex> verts;
	verts.reserve(2 * Edges.size());
	for (const auto& edge : Edges) {
		if (BoundingBox.Contains({ edge.x1, edge.y1 })) {
			verts.emplace(edge.x1, edge.y1, edge.x1, edge.y1);
		}
		if (BoundingBox.Contains({ edge.x2, edge.y2 })) {
			verts.emplace(edge.x2, edge.y2, edge.x2, edge.y2);
		}
	}

	Verts = std::vector(verts.begin(), verts.end());

	return Edges;
}

const std::vector<PointIVertex>& VisualPath::getSpecialPoints() const {
	return SpecialPoints;
}

const std::vector<TwoPointIVertex>& VisualPath::getVerts() const {
	return Verts;
}

bool VisualPath::Intercept(const PointType& Pos) const {
	return Data.Intercept(Pos) != InvalidLineIndex;
}

bool VisualPath::TryAbsorb(VisualPath& Other) {
	if (Data.AbsorbIfIntercept(Other.Data)) {
		IsDirty = true;
		return true;
	}
	return false;
}

//Returnes the Next Free
VisualPath::PathIndex VisualPath::Init(VisualPathData&& pd) {
	assert(!IsFree());
	PathIndex PathIndexNext = CachedBoundingBox.Position.x();
	Data = std::move(pd);
	ThisIsFree = false;
	return PathIndexNext;
}

//Set this head afterwareds
void VisualPath::Free(const PathIndex& head) {
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

bool VisualPath::IsFree() const {
	return ThisIsFree;
}