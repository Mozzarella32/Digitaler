#include "pch.hpp"
#include "Path.hpp"

#include "OpenGlDefines.hpp"

VisualPath::VisualPath(VisualPathData&& Data)
	:Data(std::move(Data)) {
}

// static const std::vector<TwoPointIRGBRHGHBHVertex> EmptyVerts;

void VisualPath::SetHover(bool newHover) {
	if (Hover == newHover) return;
	Hover = newHover;
	Dirty = true;
}
//
//void VisualPath::SetMarked(bool Marked) {
//	if (this->Marked == Marked) return;
//	this->Marked = Marked;
//	Dirty = true;
//}
//
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
//
//void VisualPath::UnmarkAll() {
//	if (!MarkedBoundingBox) return;
//	MarkedBoundingBox.reset();
//	FullyMarked = false;
//	Dirty = true;
//}

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
		EdgesUnmarked.clear();
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
	EdgesUnmarked.clear();
	EdgesMarked.clear();
	IntersectionPoints.clear();
	Verts.clear();
	// ConflictPoints.clear();

	//Calculate Edges and SpecialPoints
	std::unordered_set<PointType> intersectionPoints;
	/*Edges.reserve(Data.Lines.size());

	auto hr = Data.toHumanReadable();

	for (const auto& pair : Data.Lines) {
		const auto& A = Data.Points[pair.first];
		const auto& B = Data.Points[pair.second];
		assert(A != B && "You Messed Up, Points Of line are identical");

		if (MyRectI::FromCorners(A, B).Intersectes(BB)) {
			if (BB.Contains(A) && Data.GetConnectionCount(pair.first) >= 2) {
				specialPoints.emplace(A);
			}
			if (BB.Contains(B) && Data.GetConnectionCount(pair.second) >= 2) {
				specialPoints.emplace(B);
			}
			Edges.emplace_back(A, B);
		}
	}
	Edges.shrink_to_fit();*/

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

	for (PointIndex i = 0; i < Data.Points.size(); i++) {
		const PointNode& p = Data.Points[i];
		if (p.IsFree())continue;

		Verts.push_back(AssetVertex::PathVertex(p.Pos, MyColor));

		for (LineIndexInPoint j = 0; j < 4; j++) {
			const PointIndex& OtherIndex = p.Connections[j];
			if (OtherIndex < i)continue;
			if (OtherIndex == InvalidPointIndex || OtherIndex == ReservedPointIndex)break;
			const PointNode& other = Data.Points[OtherIndex];

			const PointType& A = p.Pos;
			const PointType& B = other.Pos;

			assert(A != B && "You Messed Up, Points Of line are identical");

			if (!MyRectI::FromCorners(A, B).Intersectes(BB)) continue;

			if (BB.Contains(A) && p.ConnectionCount() >= 2) {
				intersectionPoints.emplace(A);
			}
			if (BB.Contains(B) && other.ConnectionCount() >= 2) {
				intersectionPoints.emplace(B);
			}

			/*if (Marked) {
				EdgesMarked.emplace_back(A, B, ColourType{ 1.0f,0.0f,1.0f,1.0f });
				continue;
			}*/

			if (Preview) {
				EdgesMarked.push_back(AssetVertex::PathEdge(A, B, MyColor));
				// EdgesMarked.emplace_back(A, B, MyColor, ColourType{ 0.0f,1.0f,0.2f,0.0f });
				continue;
			}
			if (MarkedBoundingBox && LineMarked.at({ i,OtherIndex })) {
				EdgesMarked.push_back(AssetVertex::PathEdge(A, B, MyColor));
				// EdgesMarked.emplace_back(A, B, MyColor, ColourType{ 1.0f, 0.0f, 1.0f, 1.0f });
				continue;
			}
			if (Hover && !HasMarked() && !DontShowHover) {
				EdgesMarked.push_back(AssetVertex::PathEdge(A, B, MyColor));
				// EdgesMarked.emplace_back(A, B, MyColor, ColourType{ 1.0f,1.0f,0.0f,0.0f });
				continue;
			}
				EdgesUnmarked.push_back(AssetVertex::PathEdge(A, B, MyColor));
			// EdgesUnmarked.emplace_back(A, B, MyColor, ColourType::Zero());
		}
	}

	IntersectionPoints.reserve(intersectionPoints.size());
	for(const auto& p : intersectionPoints) {
		IntersectionPoints.push_back(AssetVertex::PathIntersection(p, MyColor));
		// SpecialPoints.emplace_back(p, MyColor);
	}

	// auto IsNormal = [](const float& r, const float& g, const float& b) {
	// 	return r != 1.0f || g != 0.0f || b != 1.0f;
	// 	};

	//Calculate Verts
	// std::unordered_map<PointType, TwoPointIRGBRHGHBHVertex> verts;
	// verts.reserve(2 * EdgesMarked.size());

	// auto PlaceVert = [this, IsNormal, &BB, &verts](const int& x, const int& y, const float& r, const float& g, const float& b, const float& rh, const float& gh, const float& bh, bool marked) {
	// 	if (!BB.Contains({ x, y })) return;
	// 	auto it = verts.find({ x, y });

	// 	if (it == verts.end()) {
	// 		if (!marked)return;
	// 		verts[{x, y}] = { x, y, x, y, r, g, b, rh, gh, bh };
	// 		return;
	// 	}

	// 	bool ThisNormal = IsNormal(rh, gh, bh);
	// 	bool OtherNormal = IsNormal(it->second.rh, it->second.gh, it->second.bh);
	// 	if (OtherNormal != ThisNormal) {
	// 		if (!OtherNormal) {
	// 			ConflictPoints.emplace_back(x, y, x, y, it->second.r, it->second.g, it->second.b, it->second.rh, it->second.gh, it->second.bh);
	// 		}
	// 		else {
	// 			ConflictPoints.emplace_back(x, y, x, y, r, g, b, rh, gh, bh);
	// 		}
	// 		verts.erase(it);
	// 		return;
	// 	}
	// 	if (!marked)return;
	// 	if (!ThisNormal) {
	// 		if (OtherNormal) {
	// 			verts.erase(it);
	// 			verts[{x, y}] = { x, y, x, y, r,g,b,rh, gh, bh };
	// 		}
	// 	}
	// 	};

	// for (const auto& edge : EdgesMarked) {
	// 	PlaceVert(edge.x1, edge.y1, edge.r, edge.g, edge.b, edge.rh, edge.gh, edge.bh, true);
	// 	PlaceVert(edge.x2, edge.y2, edge.r, edge.g, edge.b, edge.rh, edge.gh, edge.bh, true);
	// }
	// for (const auto& edge : EdgesUnmarked) {
	// 	PlaceVert(edge.x1, edge.y1, edge.r, edge.g, edge.b, edge.rh, edge.gh, edge.bh, false);
	// 	PlaceVert(edge.x2, edge.y2, edge.r, edge.g, edge.b, edge.rh, edge.gh, edge.bh, false);
	// }

	// Verts.reserve(verts.size());
	// for (const auto& pair : verts) {
	// 	Verts.emplace_back(pair.second);
	// }
}

const std::vector<AssetVertex>& VisualPath::getEdgesUnmarked() const {
	return EdgesUnmarked;
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

// const std::vector<TwoPointIRGBRHGHBHVertex>& VisualPath::getConflictPoints() const {
// 	return ConflictPoints;
// }

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
	EdgesMarked.clear();
	EdgesUnmarked.clear();
	IntersectionPoints.clear();
	Verts.clear();
	ClearMarkedArea();
	Dirty = true;
	FullyMarked = false;
	Hover = false;
	Preview = false;
	//Marked = false;
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
//
//void VisualPath::RotateCCW(const PointType& Pos) {
//	assert(IsFullyMarked());
//	Data.RotateAroundCCW(Pos);
//	ClearMarkedArea();
//	MarkAll();
//	NeedsMergingAfterMove = true;
//}
//
//void VisualPath::RotateHW(const PointType& Pos) {
//	assert(IsFullyMarked());
//	Data.RotateAroundHW(Pos);
//	ClearMarkedArea();
//	MarkAll();
//	NeedsMergingAfterMove = true;
//}

void VisualPath::Flip(const int& pos, bool X) {
	assert(IsFullyMarked());
	Data.Flip(pos, X);
	ClearMarkedArea();
	MarkAll();
	NeedsMergingAfterMove = true;
}
//
//void VisualPath::FlipY(const int& pos) {
//	assert(IsFullyMarked());
//	Data.FlipY(pos);
//	ClearMarkedArea();
//	MarkAll();
//	NeedsMergingAfterMove = true;
//}

void VisualPath::Move(const PointType& Diff) {
	assert(IsFullyMarked());
	Data.Move(Diff);
	ClearMarkedArea();
	MarkAll();
	NeedsMergingAfterMove = true;
}
