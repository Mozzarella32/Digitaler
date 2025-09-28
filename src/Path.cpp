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
	SetMarkedArea(FullRectF);
}

void VisualPath::SetPreview(bool newPreview) {
	if (Preview == newPreview) return;
	Preview = newPreview;
	Dirty = true;
}

bool VisualPath::SetMarkedArea(const MyRectF& MarkedBB) {
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

			const bool IsMarked = MarkedBB == FullRectF || MyRectF::FromCorners(A.cast<float>(), B.cast<float>()).Intersectes(MarkedBoundingBox.value());
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

void VisualPath::ComputeAll(const MyRectI& BB, [[maybe_unused]] bool allowSelf) {
	if (!Data.BoundingBox.Intersectes(BB)) {
		normal.clear();
		marked.clear();
		return;
	}

	if (CachedBoundingBox != BB) {
		CachedBoundingBox = BB;
		Dirty = true;
	}

	if (!Dirty) return;

	Dirty = false;
	normal.clear();
	marked.clear();

	//intersectionPoints and if they are marked
	std::unordered_map<PointType, bool> intersectionPoints;
	//verts and if they are marked
	std::unordered_map<PointType, bool> verts;

	ColourType MyColor = ColourType{0.5f,0.0f,0.5f,1.0f };

#ifdef PathRandomColor
	static std::unordered_map<int, ColourType> ColourMap;
	auto GetColour = [&](const int& i) {
		auto it = ColourMap.find(i);
		if (it == ColourMap.end()) {
			ColourMap[i] = ColourType(float(rand()) / (float)RAND_MAX, float(rand()) / (float)RAND_MAX, float(rand()) / (float)RAND_MAX, 1.0);
			return ColourMap[i];
		}
		return it->second;
		};


	MyColor = GetColour(Data.GetId());
#endif

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

			assert(allowSelf || (A != B && "You Messed Up, Points Of line are identical"));

			if (!MyRectI::FromCorners(A, B).Intersectes(BB)) continue;

			bool isMarked = MarkedBoundingBox && LineMarked.at({ i,OtherIndex });

			if (isMarked) {
				if (VisualPathData::LineIsHorizontal(A,B)) {
				  marked.EdgesH.push_back(AssetVertex::PathEdge(A, B, MyColor));
				}
				else {
				  marked.EdgesV.push_back(AssetVertex::PathEdge(A, B, MyColor));
				}
			}
			if (VisualPathData::LineIsHorizontal(A,B)) {
				normal.EdgesH.push_back(AssetVertex::PathEdge(A, B, MyColor));
			}
			else {
				normal.EdgesV.push_back(AssetVertex::PathEdge(A, B, MyColor));
			}

			if (BB.Contains(A) && p.ConnectionCount() >= 2) {
				auto it = intersectionPoints.find(A);
				if(it == intersectionPoints.end() || !it->second)
					intersectionPoints[A] = isMarked;
			}
			if (BB.Contains(B) && other.ConnectionCount() >= 2) {
				auto it = intersectionPoints.find(B);
				if(it == intersectionPoints.end() || !it->second)
					intersectionPoints[B] = isMarked;
			}

			if (BB.Contains(A)) {
				auto it = verts.find(A);
				if(it == verts.end() || !it->second)
					verts[A] = isMarked;
			}
			if (BB.Contains(B)) {
				auto it = verts.find(B);
				if(it == verts.end() || !it->second)
					verts[B] = isMarked;
			}
		}
	}

	normal.IntersectionPoints.reserve(intersectionPoints.size());
	for (const auto& p : intersectionPoints) {
		normal.IntersectionPoints.push_back(AssetVertex::PathIntersection(p.first, MyColor));
		if(p.second)
			marked.IntersectionPoints.push_back(AssetVertex::PathIntersection(p.first, MyColor));
	}

	normal.Verts.reserve(verts.size());
	for (const auto& p : verts) {
		normal.Verts.push_back(AssetVertex::PathVertex(p.first, MyColor));
		if (p.second)
			marked.Verts.push_back(AssetVertex::PathVertex(p.first, MyColor));
	}
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
	normal.clear();
	marked.clear();
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
