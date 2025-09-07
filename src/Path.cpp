#include "pch.hpp"
#include "Path.hpp"

#include "OpenGlDefines.hpp"

VisualPath::VisualPath(VisualPathData&& Data)
	:Data(std::move(Data)) {
}

void VisualPath::SetHover(bool newHover) {
	if (Hover == newHover) return;
	Hover = newHover;
	Dirty = true;
}

bool VisualPath::ToggleMarkedIfHover() {
	if (!Hover) return false;
	Dirty = true;
	if (!MarkedBoundingBox) {
		MarkAll();
	}
	else {
		ClearMarkedArea();
	}
	return true;
}

void VisualPath::MarkAll() {
	SetMarkedArea(FullRectI);
}

void VisualPath::SetPreview(bool newPreview) {
	if (Preview == newPreview) return;
	Preview = newPreview;
	Dirty = true;
}

bool VisualPath::SetMarkedArea(const MyRectI& MarkedBB) {
	if (MarkedBoundingBox && MarkedBoundingBox.value() == MarkedBB) return false;
	MarkedBoundingBox = MarkedBB;
	Dirty = true;
	LineMarked.clear();
	bool FoundOne = false;
	FullyMarked = true;
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

			const bool IsMarked = MarkedBB == FullRectI || MyRectI::FromCorners(A, B).Intersectes(MarkedBoundingBox.value());
			if (IsMarked) {
				FoundOne = true;
			}
			else {
				FullyMarked = false;
			}

			LineMarked[{i, OtherIndex}] = IsMarked;

		}
	}
	if (!FoundOne) {
		LineMarked.clear();
		MarkedBoundingBox.reset();
	}
	return true;
}

bool VisualPath::ClearMarkedArea() {
	if (!MarkedBoundingBox) return false;
	MarkedBoundingBox.reset();
	FullyMarked = false;
	Dirty = true;
	return true;
}

bool VisualPath::HasMarked() const {
	return MarkedBoundingBox.has_value();
}

bool VisualPath::HasMarkedAt(const PointType& Mouse) const {
	if (!HasMarked()) return false;
	const auto Intercept = Data.Intercept(Mouse);
	if (Intercept == InvalidLineIndex) return false;
	return LineMarked.at(Intercept);
}

bool VisualPath::IsFullyMarked() const {
	return FullyMarked;
}

void VisualPath::SetDontShowHover(bool value) {
	if (DontShowHover == value) return;
	DontShowHover = value;
	Dirty = true;
}

std::pair<std::vector<CompressedPathData>, std::vector<CompressedPathData>> VisualPath::Split() {
	if (!HasMarked()) return {};
	std::unordered_set<LineIndex> Marked;
	for (const auto& [Lineindex, Value] : LineMarked) {
		if (!Value) continue;
		Marked.insert(Lineindex);
	}
	MarkAll();
	return Data.Split(Marked);
}

bool VisualPath::NeedsMerging() const {
	return NeedsMergingAfterMove;
}

void VisualPath::ClearNeedsMerging() {
	NeedsMergingAfterMove = false;
}

void VisualPath::ComputeAll(const MyRectI& BB) {
	if (!Data.BoundingBox.Intersectes(BB)) {
		Edges.clear();
		EdgesMarked.clear();
		IntersectionPoints.clear();
		Verts.clear();
		return;
	}

	if (CachedBoundingBox != BB) {
		CachedBoundingBox = BB;
		Dirty = true;
	}

	if (!Dirty) return;

	Dirty = false;
	Edges.clear();
	EdgesMarked.clear();
	IntersectionPoints.clear();
	Verts.clear();

	std::unordered_map<PointType, unsigned int> intersectionPoints;

	static std::unordered_map<int, ColourType> ColourMap;
	auto GetColour = [&](const int& i) {
		auto it = ColourMap.find(i);
		if (it == ColourMap.end()) {
			ColourMap[i] = ColourType(float(rand()) / (float)RAND_MAX, float(rand()) / (float)RAND_MAX, float(rand()) / (float)RAND_MAX, 1.0);
			return ColourMap[i];
		}
		return it->second;
		};


	ColourType MyColor = GetColour(Data.GetId());

	Verts.reserve(Data.Points.size());

	bool vertHasMarked = false;

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

			if (!MyRectI::FromCorners(A, B).Intersectes(BB)) continue;

			unsigned int flags = 0;
			if (Preview) {
				flags |= AssetVertex::Preview;
			}
			if (MarkedBoundingBox && LineMarked.at({ i,OtherIndex })) {
				flags |= AssetVertex::Marked;
				EdgesMarked.push_back(AssetVertex::PathEdge(A, B, MyColor, AssetVertex::Flags::Marked));
				vertHasMarked = true;
			}
			if (Hover && !HasMarked() && !DontShowHover) {
				flags |= AssetVertex::Highlight;
			}

			if (BB.Contains(A) && p.ConnectionCount() >= 2) {
				auto it = intersectionPoints.find(A);
				if (it == intersectionPoints.end() || ((it->second & AssetVertex::Flags::Marked) == 0))
				   intersectionPoints[A] = flags;
			}
			if (BB.Contains(B) && other.ConnectionCount() >= 2) {
				auto it = intersectionPoints.find(B);
				if (it == intersectionPoints.end() || ((it->second & AssetVertex::Flags::Marked) == 0))
					intersectionPoints[B] = flags;
			}

			/*if (Marked) {
				EdgesMarked.emplace_back(A, B, ColourType{ 1.0f,0.0f,1.0f,1.0f });
				continue;
			}*/

			/*unsigned int flags = 0;
			if (Preview) {
				flags |= AssetVertex::Preview;*/
				// EdgesMarked.push_back(AssetVertex::PathEdge(A, B, MyColor));
				// // EdgesMarked.emplace_back(A, B, MyColor, ColourType{ 0.0f,1.0f,0.2f,0.0f });
				// continue;
			//}
			//if (MarkedBoundingBox && LineMarked.at({ i,OtherIndex })) {
				//flags |= AssetVertex::Marked;
				// EdgesMarked.push_back(AssetVertex::PathEdge(A, B, MyColor));
				// // EdgesMarked.emplace_back(A, B, MyColor, ColourType{ 1.0f, 0.0f, 1.0f, 1.0f });
				// continue;
			//}
			//if (Hover && !HasMarked() && !DontShowHover) {
				//flags |= AssetVertex::Highlight;
				// EdgesMarked.push_back(AssetVertex::PathEdge(A, B, MyColor));
				// // EdgesMarked.emplace_back(A, B, MyColor, ColourType{ 1.0f,1.0f,0.0f,0.0f });
				// continue;
			//}
				// EdgesUnmarked.push_back(AssetVertex::PathEdge(A, B, MyColor));
			// EdgesUnmarked.emplace_back(A, B, MyColor, ColourType::Zero());
			Edges.push_back(AssetVertex::PathEdge(A, B, MyColor, flags));
		}

		unsigned int flags = 0;
		if (Preview) {
			flags |= AssetVertex::Preview;
		}
		if (vertHasMarked) {
			flags |= AssetVertex::Marked;
		}
		if (Hover && !HasMarked() && !DontShowHover) {
			flags |= AssetVertex::Highlight;
		}

		Verts.push_back(AssetVertex::PathVertex(p.Pos, MyColor, flags));
	}

	IntersectionPoints.reserve(intersectionPoints.size());
	for(const auto& p : intersectionPoints) {
		IntersectionPoints.push_back(AssetVertex::PathIntersection(p.first, MyColor, p.second));
	}
}

const std::vector<AssetVertex>& VisualPath::getEdges() const {
	return Edges;
}

const std::vector<AssetVertex>& VisualPath::getEdgesMarked() const {
	return EdgesMarked;
}

const std::vector<AssetVertex>& VisualPath::getIntersectionPoints() const {
	return IntersectionPoints;
}

const std::vector<AssetVertex>& VisualPath::getVerts() const {
	return Verts;
}

bool VisualPath::Intercept(const PointType& Pos) const {
	return Data.Intercept(Pos) != InvalidLineIndex;
}

bool VisualPath::TryAbsorb(VisualPathData& Other) {
	if (Data.AbsorbIfIntercept(Other)) {
		ClearMarkedArea();
		Dirty = true;
		return true;
	}
	return false;
}

//Returnes the Next Free
VisualPath::PathIndex VisualPath::Init(VisualPathData&& pd) {
	assert(IsFree());
	PathIndex PathIndexNext = CachedBoundingBox.Position.x();
	Data = std::move(pd);
	ThisIsFree = false;
	Dirty = true;
	FullyMarked = false;
	Hover = false;
	Preview = false;
	return PathIndexNext;
}

//Set this head afterwareds
void VisualPath::Free(const PathIndex& head) {
	Data = {};
	CachedBoundingBox = {};
	Edges.clear();
	EdgesMarked.clear();
	IntersectionPoints.clear();
	Verts.clear();
	ClearMarkedArea();
	Dirty = true;
	FullyMarked = false;
	Hover = false;
	Preview = false;
	CachedBoundingBox.Position.x() = head;
	ThisIsFree = true;
}

bool VisualPath::IsFree() const {
	return ThisIsFree;
}

void VisualPath::Rotate(const PointType& Pos, bool CW) {
	assert(IsFullyMarked());
	Data.RotateAround(Pos, CW);
	ClearMarkedArea();
	MarkAll();
	NeedsMergingAfterMove = true;
}

void VisualPath::Flip(const int& pos, bool X) {
	assert(IsFullyMarked());
	Data.Flip(pos, X);
	ClearMarkedArea();
	MarkAll();
	NeedsMergingAfterMove = true;
}

void VisualPath::Move(const PointType& Diff) {
	assert(IsFullyMarked());
	Data.Move(Diff);
	ClearMarkedArea();
	MarkAll();
	NeedsMergingAfterMove = true;
}
