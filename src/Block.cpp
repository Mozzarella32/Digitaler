#include "Block.hpp"
#include "pch.hpp"

#include "OpenGlDefines.hpp"

#include "DataResourceManager.hpp"

#ifdef UseCollisionGrid
int VisualBlockInterior::BoxSize = 5;
#endif

const std::vector<TwoPointIVertex>& VisualBlockInterior::GetEdges(bool Preview) const {
	if (Preview) {
		return PreviewEdges;
	}
	return Edges;
}

const std::vector<TwoPointIVertex>& VisualBlockInterior::GetEdgesMarked(bool Preview) const {
	if (Preview) {
		return PreviewEdges;
	}
	return EdgesMarked;
}

static const  std::vector<TwoPointIVertex> EmptyPathVec = {};

const std::vector<TwoPointIVertex>& VisualBlockInterior::GetEdgesUnmarked(bool Preview) const {
	if (Preview) {
		return EmptyPathVec;
	}
	return EdgesUnmarked;
}

const std::vector<PointIVertex>& VisualBlockInterior::GetSpecialPoints(bool Preview) const {
	if (Preview) {
		return PreviewSpecialPoints;
	}
	return SpecialPoints;
}

const std::vector<TwoPointIVertex>& VisualBlockInterior::GetVerts(bool Preview) const {
	if (Preview) {
		return PreviewVerts;
	}
	return Verts;
}
//
//const std::vector<TwoPointIRGBAVertex>& VisualBlockInterior::GetBlockVerts() const {
//	return std::vector<;
//}

VisualBlockInterior::VisualBlockInterior(const CompressedBlockData& data, DataResourceManager* ResourceManager)
	:ResourceManager(ResourceManager)/*, Blocks(data.blockExteriorData.ContainedBlocks)* /
	/*:renderer(renderer)*/ {
	PROFILE_FUNKTION;

	Paths.reserve(data.Paths.size());
	for (const auto& pd : data.Paths) {
		Paths.emplace_back(VisualPathData(pd, this));
	}

	for (const auto& [Name, Blocks] : data.blockExteriorData.ContainedBlocks) {
		for (const auto& Block : Blocks) {
			AddBlock(ResourceManager->GetBlockIndex(Name), Block);
		}
	}

	//return;
	VisualPathData pd;


#ifdef Testing
	//3426
	//TODO has to be called somewhere else
	//srand(FrameCount);

	auto r = []() {
		return (rand() % 1000) - 500;
		};


	//std::string s;
	for (int i = 0; i < 10; i++) {

		std::vector<PointType> Points;

		auto shared = r();
		auto first = r();

		Points.push_back({ first, shared });

		int other;
		do {
			other = r();
		} while (other == first);

		Points.push_back({ other, shared });

		pd = VisualPathData(Points[0], Points[1], this);

		for (int j = 0; j < 100; j++) {
			//if (i * 10 + j > FrameCount / 5)break;
			Points.push_back({ r(), r() });
			//std::stringstream ss;
			//for (const auto& p : Points) {
			//	ss << "Vec.push_back({ " << p.x() << "," << p.y() << " });\n";
			//}
			///*std::ofstream of("Test.txt");
			//of << ss.str();
			//of.flush();
			//of.close();*/
			//s = ss.str();
			//std::cout << FrameCount;
			//auto hr = pd.toHumanReadable();
			pd.addTo(Points.back());
		}
		Points.push_back({ r(), r() });
		pd.addTo(Points.back());

		//Paths.emplace_back(std::move(pd));


		/*if ((i+1) * 10 > FrameCount / 5) {
		}*/
		//std::cout << FrameCount;

		VisualPath vp(std::move(pd));
		if (!TryAbsorb(vp)) {
			Paths.emplace_back(std::move(vp));
		}
	}

	//auto hr = Paths[0].Data.toHumanReadable();
	auto i = 0;
#else
	//std::vector<PointType> Vec;
	////Todo Resolve this
	//Vec.push_back({ -3,-4 });
	//Vec.push_back({ -5,-4 });
	//Vec.push_back({ -1,-7 });
	//Vec.push_back({ 3,-8 });
	//Vec.push_back({ -1,0 });
	//Vec.push_back({ -2,5 });
	//Vec.push_back({ -6,8 });
	//Vec.push_back({ -8,-4 });
	//Vec.push_back({ 6,7 });
	//Vec.push_back({ -4,3 });
	//Vec.push_back({ -10,-6 });
	//Vec.push_back({ 2,2 });
	//Vec.push_back({ -6,1 });
	//Vec.push_back({ 8,-8 });
	//Vec.push_back({ 9,5 });
	//Vec.push_back({ 6,8 });
	//Vec.push_back({ -7,2 });
	//Vec.push_back({ -6,-2 });
	//Vec.push_back({ -1,-5 });
	//Vec.push_back({ 7,1 });
	//Vec.push_back({ 4,7 });
	//Vec.push_back({ -9,5 });
	//Vec.push_back({ -5,3 });
	//Vec.push_back({ -9,-4 });
	//Vec.push_back({ -8,6 });
	//Vec.push_back({ -6,8 });
	//Vec.push_back({ 8,0 });
	//Vec.push_back({ -8,9 });
	//Vec.push_back({ -10,6 });
	//Vec.push_back({ -8,-7 });
	//Vec.push_back({ -7,-4 });
	//Vec.push_back({ 8,7 });
	//Vec.push_back({ -1,3 });
	//Vec.push_back({ 8,-8 });
	//Vec.push_back({ -10,2 });
	//Vec.push_back({ -2,-9 });
	//Vec.push_back({ 9,8 });
	//Vec.push_back({ -8,-4 });
	//Vec.push_back({ 1,0 });
	//Vec.push_back({ -10,7 });
	//Vec.push_back({ -3,4 });
	//Vec.push_back({ -6,6 });
	//Vec.push_back({ 6,-6 });
	//Vec.push_back({ -7,0 });
	//Vec.push_back({ 7,8 });
	//Vec.push_back({ -1,-1 });
	//Vec.push_back({ 0,-3 });
	//Vec.push_back({ -2,-7 });
	//Vec.push_back({ -3,-4 });
	//Vec.push_back({ -5,7 });
	//Vec.push_back({ 5,-7 });
	//Vec.push_back({ -2,-7 });
	//Vec.push_back({ -9,-6 });
	//Vec.push_back({ -8,9 });
	//Vec.push_back({ -10,0 });
	//Vec.push_back({ 5,5 });
	//Vec.push_back({ 8,2 });
	//Vec.push_back({ -9,7 });
	//Vec.push_back({ 0,0 });
	//Vec.push_back({ -1,0 });
	//Vec.push_back({ 0,8 });
	//Vec.push_back({ -6,8 });
	//Vec.push_back({ 0,1 });
	//Vec.push_back({ 1,-10 });
	//Vec.push_back({ 5,9 });
	//Vec.push_back({ -8,-3 });
	//Vec.push_back({ 4,-7 });
	//Vec.push_back({ -7,-3 });
	//Vec.push_back({ -2,2 });
	//Vec.push_back({ 7,3 });
	//Vec.push_back({ -8,-2 });
	//Vec.push_back({ 1,3 });
	//Vec.push_back({ -5,-5 });
	//Vec.push_back({ -7,-3 });
	//Vec.push_back({ -6,0 });
	//Vec.push_back({ 0,-6 });
	//Vec.push_back({ -5,5 });
	//Vec.push_back({ 0,-9 });
	//Vec.push_back({ -10,-4 });
	//Vec.push_back({ 3,6 });
	//Vec.push_back({ -5,5 });
	//Vec.push_back({ -6,-8 });
	//Vec.push_back({ 7,-7 });
	//Vec.push_back({ -6,-6 });
	//Vec.push_back({ -10,-1 });


	//bool last = false;
	////last = true;
	//pd = VisualPathData(Vec[0], Vec[1], this);
	//for (int i = 2; i < Vec.size() - 1; i++) {
	//	//if (i > FrameCount)break;
	//	if (i == Vec.size() - 1)continue;
	//	if (Vec[i] == PointType{ -7,2 }) {
	//		int j = 0;
	//	}
	//	auto hr = CollisionMapToString();
	//	pd.addTo(Vec[i]);
	//	hr = CollisionMapToString();
	//}
	//if (last)pd.addTo(Vec.back());

	////auto hr = pd.toHumanReadable();

	//AddPath(std::move(pd));

	//auto i = 0;
	/*{
		pd = VisualPathData({ 0,0 }, { 0,1 },this);
		pd.addTo({ 0,2 });
	}
	{
		pd = VisualPathData({ 0,0 }, { 0,1 }, this);
		pd.addTo({ 0,2 });
	}
	{
		pd = VisualPathData({ 0,0 }, { 0,1 }, this);
		pd.addTo({ 0,2 });
	}
	{
		pd = VisualPathData({ 0,0 }, { 0,1 }, this);
		pd.addTo({ 0,2 });
	}*/
#endif

	/*pd = VisualPathData({ {0,0},{0,-2} });
	pd.addTo({ 1,-2 });
	pd.addTo({ 1,2 });
	pd.addTo({ 0,2 });
	pd.addTo({ 0,1 });
	pd.addTo({ -1, -3 });

	Paths.emplace_back(pd);*/

	/*pd = VisualPathData({ {0,0},{0,-2} });
	pd.addTo({ 1,-1 });
	pd.addTo({ 1,1 });
	pd.addTo({ 1,1 });
	pd.addTo({ 0,1 });
	pd.addTo({ 0,2 });
	pd.addTo({ 1,2 });
	pd.addTo({ 1,3 });
	pd.addTo({ 0,3 });
	pd.addTo({ 0,-3 });*/

	/*pd = VisualPathData({ {0,0},{0,-2} });
	pd.addTo({ 2,-2 });
	pd.addTo({ 2,-1});
	pd.addTo({ 1,-1});
	pd.addTo({ 0,-1});
	Paths.emplace_back(pd);*/


	/*pd = VisualPathData({ {0,1},{0,0} });
	pd.addTo({ 2,0 });
	pd.addTo({ 2,2 });
	pd.addTo({ 2,-2 });

	auto hr = pd.toHumanReadable();

	Paths.emplace_back(pd);

	pd = VisualPathData({ {0,2},{2,2} });
	pd.addTo({ 2,-2 });
	pd.addTo({ 0,-2 });
	pd.addTo({ 0,2 });

	 hr = pd.toHumanReadable();

	 Paths.emplace_back(pd);

	 Paths[0].TryAbsorb(Paths[1]);

	 hr = Paths[0].Data.toHumanReadable();

	 Paths.pop_back();*/

	 //Paths.emplace_back(pd);


	/*
	pds.push_back({ { 10,0 },{ 0,0 } });

	pds.back().addTo(0, { 0,10 });
	*/
	/*pds.push_back({ { 10,0 },{ 0,0 } });

	//pds.back().addTo(0, { 0,5 });*/

	////pd.addTo(0, { 0,0 }, { 0,-5 });
	//pds.back().addTo(0, { 5,5 });
	//pds.back().addTo(3, { 10,5 });
	//pds.back().addTo(4, { 10,10 });
	//pds.back().addTo(5, { 5,10 });
	//pds.back().addTo(5, { 5,7 });
	//pds.back().addTo(8, { 5,8 });
	//pds.back().addTo(3, { 10,3 });
	//pds.back().addTo(7, { 7,8 });
	//auto hr = pds.back().toHumanReadable();

	///*auto mouse = PointType{ MouseIndex.x(),MouseIndex.y() };
	//if (mouse == PointType{9, 0}) {
	//int i = 0;
	//}

	//pds.back().addTo(0, mouse);*/

	//int MyFrameCount = (FrameCount) % 144;

	//PointType Point = { MyFrameCount / 12, MyFrameCount % 12 };

	//pds.back().addTo(0, Point);

	///*pd.addTo( { 5,0 }, { 5,5 });

	//pd.addTo({ 5,2 }, { MouseIndex.x(),2});

	//pd.addTo({MouseIndex.})*/

	/*for (int i = 32; i < 42; i++) {
		pd = VisualPathData({ { 5 + i,-530 + i },{ 5 + i,3 + i } });

		pd.addTo(0, { 20 + i,3 + i });

		pd.addTo(1, { 5 + i,500 + i });

		pd.addTo(1, { -500 + i,500 + i });
		pd.addTo(0, { -500 + i,-20 + i });
		pd.addTo(0, { 20 + i,-20 + i });
		pd.addTo(6, { -500 + i,-20 + i });

		pd.addTo(3, { 5 + i,500 + i });

		Paths.emplace_back(pd);
	}*/

	/*for (const auto& pd : pds) {
		Paths.emplace_back(pd);
	}*/

	////Paths.front().Marked = true;
	//Paths.front().SetMarked(FrameCount % 20 <= 10);

	//for (int i = 1; i < Paths.size(); i++) {
	//	if ((FrameCount % 200) / 200.0 < 0.1 * i) {
	//		Paths[i].SetMarked(true);
	//	}
	//}

}

VisualBlockInterior::~VisualBlockInterior() noexcept {
	Destructing = true;
}

void VisualBlockInterior::UpdateVectsForVAOs(const MyRectI& ViewRect, const PointType& Mouse) {
	//std::vector<std::string> hrs;
	//for (auto& Path : Paths) {
	//	hrs.push_back(Path.Data.toHumanReadable());
	//	hrs.back().append("\nCompressed:\n");
	//	hrs.back().append(CompressedPathData(Path.Data).toHumanReadable());
	//	//Path = VisualPath(VisualPathData(CompressedPathData(Path.Data)));
	//	hrs.push_back(Path.Data.toHumanReadable());
	//}
	if (CachedBoundingBox != ViewRect) {
		CachedBoundingBox = ViewRect;
		Dirty = true;
		DirtyBlocks = true;
	}

	if (MouseCached != Mouse) {
		MouseCached = Mouse;
		Dirty = true;
		DirtyBlocks = true;
		for (auto& p : Paths) {
			if (p.Intercept(Mouse)) {
				if (!HasPreview()) {
					p.SetMarked(true);
				}
				else {
					p.SetMarked(false);
				}
			}
			else {
				p.SetMarked(false);
			}
		}
	}

	DirtyBlocks = true;
	if (DirtyBlocks) {
		UpdateBlocks();
	}

	if (!Dirty) return;

	Dirty = false;

	Edges.clear();
	EdgesMarked.clear();
	EdgesUnmarked.clear();
	SpecialPoints.clear();
	Verts.clear();

	for (auto& p : Paths) {
		const auto& e = p.ComputeAllAndGetEdges(ViewRect);
		if (p.IsMarked()) {
			EdgesMarked.insert(EdgesMarked.end(), e.begin(), e.end());
			Verts.insert(Verts.end(), p.getVerts().begin(), p.getVerts().end());
		}
		else {
			EdgesUnmarked.insert(EdgesUnmarked.end(), e.begin(), e.end());
		}
		SpecialPoints.insert(SpecialPoints.end(), p.getSpecialPoints().begin(), p.getSpecialPoints().end());
	}

	Edges.reserve(EdgesMarked.size() + EdgesUnmarked.size());
	Edges.insert(Edges.end(), EdgesMarked.begin(), EdgesMarked.end());
	Edges.insert(Edges.end(), EdgesUnmarked.begin(), EdgesUnmarked.end());
}

void VisualBlockInterior::UpdateVectsForVAOsPreview(const MyRectI& ViewRect, const PointType& Mouse) {
	if (CachedBoundingBox != ViewRect) {
		CachedBoundingBox = ViewRect;
		PreviewBoundingBoxIsDirty = true;
	}

	if (PreviewMouseCached != Mouse) {
		PreviewMouseCached = Mouse;
		PreviewIsDirty = true;
	}

	if (!PreviewIsDirty && !PreviewBoundingBoxIsDirty) return;

	bool RecalcVecs = false;

	if (PreviewIsDirty) {
		PreviewIsDirty = false;
		RecalcVecs = true;

		PreviewCached = GeneratePreviewPath(Mouse);
	}

	if (PreviewBoundingBoxIsDirty) {
		PreviewBoundingBoxIsDirty = false;
		RecalcVecs = true;
	}

	if (RecalcVecs) {
		PreviewEdges.clear();
		PreviewVerts.clear();
		PreviewSpecialPoints.clear();

		if (!PreviewCached.has_value()) return;

		auto& Preview = PreviewCached.value();

		const auto& ToAppend = Preview.ComputeAllAndGetEdges(ViewRect);
		PreviewEdges.insert(PreviewEdges.end(), ToAppend.begin(), ToAppend.end());
		PreviewVerts.insert(PreviewVerts.end(), Preview.getVerts().begin(), Preview.getVerts().end());
		PreviewSpecialPoints.insert(PreviewSpecialPoints.end(), Preview.getSpecialPoints().begin(), Preview.getSpecialPoints().end());
	}
}

#ifdef UseCollisionGrid
std::vector<PointType> VisualBlockInterior::GetBoxesFromLine(const PointType& A, const PointType& B) {
	std::vector<PointType> ret;

	auto GetBoxCorner = [](const PointType& p) -> PointType {
		return {
			(p.x() < 0 ? (static_cast<int>(p.x()) / BoxSize - 1) : static_cast<int>(p.x()) / BoxSize) * BoxSize,
			(p.y() < 0 ? (static_cast<int>(p.y()) / BoxSize - 1) : static_cast<int>(p.y()) / BoxSize) * BoxSize
		};
		};

	PointType BoxA = GetBoxCorner(A);
	PointType BoxB = GetBoxCorner(B);

	if (BoxA.x() == BoxB.x()) {
		int minY = std::min(BoxA.y(), BoxB.y());
		int maxY = std::max(BoxA.y(), BoxB.y());
		for (int y = minY; y <= maxY; y += BoxSize) {
			ret.push_back({ BoxA.x(), y });
		}
	}
	else if (BoxA.y() == BoxB.y()) {
		int minX = std::min(BoxA.x(), BoxB.x());
		int maxX = std::max(BoxA.x(), BoxB.x());
		for (int x = minX; x <= maxX; x += BoxSize) {
			ret.push_back({ x, BoxA.y() });
		}
	}

	return ret;
}

void VisualBlockInterior::RegisterLine(const LineIndex& l, const std::vector<PointNode>& Points, const VisualPathData::VisualPathDataId& Id) {
	//PROFILE_FUNKTION;

	for (const auto& p : GetBoxesFromLine(Points[l.A].Pos, Points[l.B].Pos)) {
		CollisionMap[p].emplace_back(l, Id);
	}
}

void VisualBlockInterior::UnRegisterLine(const LineIndex& l, const std::vector<PointNode>& Points, const VisualPathData::VisualPathDataId& Id) {
	//PROFILE_FUNKTION;

	if (Destructing)return;
	for (const auto& p : GetBoxesFromLine(Points[l.A].Pos, Points[l.B].Pos)) {
		auto lapItr = CollisionMap.find(p);
		assert(lapItr != end(CollisionMap));

		auto& lap = lapItr->second;
		auto it = std::find(lap.begin(), lap.end(), LineAndPath{ l,Id });
		std::swap(*it, lap.back());
		lap.pop_back();

		if (lap.empty()) {
			CollisionMap.erase(lapItr);
		}

	}
}
//
//std::vector<LineIndex> VisualBlockInterior::GetLinesWith(const PointType& p, const VisualPathData& pd) {
//	if (Destructing)return {};
//	std::vector<LineIndex> ret;
//	for (const auto& Box : GetBoxesFromLine(p, p)) {
//		auto it = CollisionMap.find(Box);
//		if (it == CollisionMap.end())continue;
//		for (const auto& l : it->second.GetLines()) {
//			if (l.IsFree())continue;
//			if (l.PathId != pd.Id)continue;
//			//const auto& Points = pd->Points;
//			ret.emplace_back(l.Line);
//		}
//	}
//	return ret;
//}


LineIndex VisualBlockInterior::GetLineWith(const PointType& p, const VisualPathData& pd) {
	//PROFILE_FUNKTION;

	if (Destructing)return {};
	for (const auto& Box : GetBoxesFromLine(p, p)) {
		auto it = CollisionMap.find(Box);
		if (it == CollisionMap.end())continue;
		for (const auto& l : it->second) {
			if (l.PathId != pd.Id)continue;
			if (!pd.PointStrictelyOnLine(l.Line, p))continue;
			//const auto& Points = pd->Points;
			return l.Line;
		}
	}
	return InvalidLineIndex;
}
#endif

VisualPath::PathIndex VisualBlockInterior::AddPath(VisualPathData&& p) {
	if (PathsFreeListHead == VisualPath::FreeListEnd) {
		Paths.emplace_back(std::move(p));
		return Paths.size() - 1;
	}

	VisualPath::PathIndex OldHead = PathsFreeListHead;
	PathsFreeListHead = Paths[OldHead].Init(std::move(p));
	return OldHead;
}

bool VisualBlockInterior::TryAbsorb(VisualPath& Path) {
	PROFILE_FUNKTION;

	Dirty = true;
	PreviewIsDirty = true;

	bool MergedAtLeastOnes = false;
	std::optional<size_t> AbsorbIndex = std::nullopt;
	for (size_t i = 0; i < Paths.size(); i++) {
		if (Paths[i].TryAbsorb(Path)) {
			AbsorbIndex = i;
			MergedAtLeastOnes = true;
			break;
		}
	}
	while (AbsorbIndex) {
		bool found = false;
		for (size_t i = 0; i < Paths.size(); i++) {
			if (i != AbsorbIndex.value() && Paths[i].TryAbsorb(Paths[AbsorbIndex.value()])) {
				Paths.erase(Paths.begin() + AbsorbIndex.value());
				if (i > AbsorbIndex.value())i--;
				AbsorbIndex = i;
				found = true;
				break;
			}
		}
		if (!found)break;
	}

	return MergedAtLeastOnes;
}

bool VisualBlockInterior::hasMark(bool Preview) const {
	return !GetEdgesMarked(Preview).empty();
}

std::optional<VisualPath> VisualBlockInterior::GeneratePreviewPath(const PointType& Mouse) {
	if (PreviewData.size() < 1) return std::nullopt;

	VisualPathData pd;

	if (PreviewData.size() == 1) {
		if (PreviewData[0] == Mouse)return std::nullopt;
		if (VisualPathData::PointsMakeStreightLine(PreviewData[0], Mouse)) {
			pd = VisualPathData(PreviewData[0], Mouse, this);
			return VisualPath(std::move(pd));
		}
		pd = VisualPathData(PreviewData[0], { PreviewData[0].x(),Mouse.y() }, this);
		pd.addTo(Mouse);
		return VisualPath(std::move(pd));
	}

	if (PreviewData[0] == PreviewData[1])return std::nullopt;

	if (VisualPathData::PointsMakeStreightLine(PreviewData[0], PreviewData[1])) {
		pd = VisualPathData(PreviewData[0], PreviewData[1], this);
	}
	else {
		pd = VisualPathData(PreviewData[0], { PreviewData[0].x(),PreviewData[1].y() }, this);
		pd.addTo(PreviewData[1]);
	}

	for (size_t i = 2; i < PreviewData.size(); i++) {
		pd.addTo(PreviewData[i]);
	}
	pd.addTo(Mouse);
	return VisualPath(std::move(pd));
}

std::optional<VisualPath> VisualBlockInterior::GeneratePreviewPath() {
	if (PreviewData.size() < 2) return std::nullopt;
	if (PreviewData[0] == PreviewData[1])return std::nullopt;

	VisualPathData pd;

	if (VisualPathData::PointsMakeStreightLine(PreviewData[0], PreviewData[1])) {
		pd = VisualPathData(PreviewData[0], PreviewData[1], this);
	}
	else {
		pd = VisualPathData(PreviewData[0], { PreviewData[0].x(),PreviewData[1].y() }, this);
		pd.addTo(PreviewData[1]);
	}

	for (size_t i = 2; i < PreviewData.size(); i++) {
		pd.addTo(PreviewData[i]);
	}
	return VisualPath(std::move(pd));
}

void VisualBlockInterior::StartDrag(const PointType& p) {
	PreviewData.clear();
	PreviewData.emplace_back(p);
}

//std::vector<MyRectI> VisualBlockInterior::GetBoundingBox() {
//	std::vector<MyRectI> ret;
//	ret.reserve(Paths.size());
//	for (const auto& Path : Paths) {
//		ret.push_back(Path.Data.BoundingBox);
//	}
//	return ret;
//}

//Finised

bool VisualBlockInterior::AddDrag(const PointType& mouse) {
	auto PathOpt = GeneratePreviewPath();
	if (PathOpt && PathOpt.value().Intercept(mouse)) {
		PreviewData.emplace_back(mouse);
		return true;
	}
	PreviewData.emplace_back(mouse);
	for (const auto& p : Paths) {
		if (p.Intercept(mouse))return true;
	}
	return false;
}

void VisualBlockInterior::EndDrag() {
	auto PathOpt = GeneratePreviewPath();
	PreviewData.clear();
	PreviewCached.reset();
	if (!PathOpt) {
		return;
	}
	PreviewIsDirty = true;
	Dirty = true;
	if (TryAbsorb(PathOpt.value())) {
		return;
	}
	Paths.emplace_back(std::move(PathOpt.value()));
}

void VisualBlockInterior::CancleDrag() {
	PreviewData.clear();
	PreviewCached = std::nullopt;
}

bool VisualBlockInterior::HasPreview() const {
	return !PreviewData.empty();
}
//
//std::string VisualBlockInterior::CollisionMapToString() const {
//	std::stringstream s;
//	for (const auto& pair : CollisionMap) {
//		s << pair.first << "\n";
//		for (const auto& l : pair.second) {
//			s << l.PathId << " " << "(" << l.LineAndPath.A << ", " << l.LineAndPath.B << "\n";
//		}
//		s << "\n";
//	}
//	return s.str();
//}

void VisualBlockInterior::UpdateBlocks() {
	if (!DirtyBlocks)return;
	DirtyBlocks = false;
	PinVerts.clear();
	BlockVerts.clear();
	SevenSegVerts.clear();
	SixteenSegVerts.clear();
	for (const auto& [IndexContained, MetaVec] : Blocks) {
		const auto& ContainedExteriorOpt = ResourceManager->GetCompressedBlockData(IndexContained);
		if (!ContainedExteriorOpt) {
			assert(false && "you messed up");
			continue;
		}
		const auto& ContainedExterior = ContainedExteriorOpt.value();
		const auto& InputPins = ContainedExterior.blockExteriorData.InputPin;
		const auto& OutputPins = ContainedExterior.blockExteriorData.OutputPin;
		const auto& Name = ContainedExterior.blockExteriorData.Name;
		int i = 0;
		for (const auto& Meta : MetaVec) {
			const PointType& ContainedSize = ContainedExterior.blockExteriorData.Size;
			const PointType& Pos = Meta.Pos;
			if (!CachedBoundingBox.Intersectes(
				MyRectI::FromCorners(Pos + PointType{ -1, -1 },
					Pos + PointType{ContainedSize.x(), -ContainedSize.y()} + PointType{ 1, 1 }))) {
				continue;
			}
			const MyDirektion::Direktion& Rotation = Meta.Rotation;
			PointType TopLeft;
			PointType BottomLeft;
			PointType TopRight;
			PointType BottomRight;
			if (Rotation == MyDirektion::Left || Rotation == MyDirektion::Right) {
				TopLeft = Pos + PointType{ -1,-1 }*int((ContainedSize.y() - ContainedSize.x())/2.0);
				BottomLeft = TopLeft - PointType{ 0,ContainedSize.x() };
				TopRight = TopLeft + PointType{ ContainedSize.y(),0 };
				BottomRight = TopLeft + PointType{ ContainedSize.y(), -ContainedSize.x() };
			}
			else {
				TopLeft = Pos;
				BottomLeft = TopLeft - PointType{ 0,ContainedSize.y() };
				TopRight = TopLeft + PointType{ ContainedSize.x(),0 };
				BottomRight = TopLeft + PointType{ ContainedSize.x(), -ContainedSize.y() };
			}

			//TODO Rotation

			/*
			PinVerts.emplace_back(-1, 1, 2, 0, 0.5, 1);
	PinVerts.emplace_back(-1, 2, 2, 0, 0.5, 1);
	PinVerts.emplace_back(1, -1, 3, 0, 0.5, 1);
	PinVerts.emplace_back(3, 0, 0, 0, 0.5, 1);
	PinVerts.emplace_back(3, 2, 0, 0, 0.5, 1);
	PinVerts.emplace_back(1, 4, 1, 0, 0.5, 1);
			*/

			//Blocks.emplace_back(0, 0, 2, -3, 1.0, 0.5, 0.5, 1.0);



			if (IndexContained == ResourceManager->GetSpecialBlockIndex().SevengSeg) {
				SevenSegVerts.emplace_back(Pos, Meta.Rotation,time(0) % 0x10, ColourType{1.0,1.0,1.0,1.0});
				BlockVerts.emplace_back(TopLeft, BottomRight, ColourType{ 0.1,0.1,0.1,1.0 });
			}
			else if (IndexContained == ResourceManager->GetSpecialBlockIndex().SixteenSeg) {
				static std::array<int, 218> Translation = { 73,99,104,32,104,97,98,101,32,106,101,116,122,116,32,101,105,110,102,117,110,107,116,105,111,110,105,101,114,101,110,100,101,115,49,54,32,83,101,103,109,101,110,116,32,68,105,115,112,108,97,121,100,97,115,32,97,108,108,101,32,97,115,99,105,105,32,90,101,105,99,104,101,110,100,97,114,115,116,101,108,108,101,110,32,107,97,110,110,58,48,49,50,51,52,53,54,55,56,57,116,104,101,32,113,117,105,99,107,32,98,114,111,119,110,32,102,111,120,32,106,117,109,112,115,32,111,118,101,114,32,116,104,101,32,108,97,122,121,32,100,111,103,84,72,69,32,81,85,73,67,75,32,66,82,79,87,78,32,70,79,88,32,74,85,77,80,83,32,79,86,69,82,32,84,72,69,32,76,65,90,89,32,68,79,71,33,64,35,36,37,94,38,42,40,41,95,45,43,123,125,124,58,34,60,62,63,96,126,91,93,92,59,39,44,46,47,126, }; SixteenSegVerts.emplace_back(Pos, Meta.Rotation, Translation[i++], ColourType{ 1.0,1.0,1.0,1.0 });
				BlockVerts.emplace_back(TopLeft, BottomRight, ColourType{ 0.1,0.1,0.1,1.0 });
			}
			else {
				BlockVerts.emplace_back(TopLeft, BottomRight, ColourType{ 0.5,0.5,1.0,1.0 });
			}
			auto GetPosition = [TopLeft, BottomLeft, TopRight, BottomRight, ContainedSize, Rotation]
				(const CompressedBlockData::BlockExteriorData::Pin& Pin)
					->PointType {
				using enum MyDirektion::Direktion;
				switch (Pin.rotation) {
				case Up:
					switch (Rotation) {
					case Up: return TopLeft + PointType{ Pin.offset,1 };
					case Right: return TopRight + PointType{ 1,-Pin.offset };
					case Down: return BottomRight + PointType{ -Pin.offset,-1 };
					case Left: return BottomLeft + PointType{ -1,Pin.offset };
					}
				case Right:
					switch (Rotation) {
					case Up: return TopRight + PointType{ 1,-Pin.offset };
					case Right: return BottomRight + PointType{ -Pin.offset,-1 };
					case Down: return BottomLeft + PointType{ -1,Pin.offset };
					case Left: return TopLeft + PointType{ Pin.offset,1 };
					}
				case Down:
					switch (Rotation) {
					case Up: return BottomLeft + PointType{ Pin.offset,-1 };
					case Right: return TopLeft + PointType{ -1,-Pin.offset };
					case Down: return TopRight + PointType{ -Pin.offset,1 };
					case Left: return BottomRight + PointType{ 1,Pin.offset };
					}
				case Left:
					switch (Rotation) {
					case Up: return TopLeft + PointType{ -1,-Pin.offset };
					case Right: return TopRight + PointType{ -Pin.offset,1 };
					case Down: return BottomRight + PointType{ 1,Pin.offset };
					case Left: return BottomLeft + PointType{ Pin.offset,-1 };
					}

				}
				assert(false);
				return {};
				};

				auto GetRotation = [Rotation](const CompressedBlockData::BlockExteriorData::Pin& Pin) {
					using enum MyDirektion::Direktion;
					if (Rotation == Up)return Pin.rotation;
					if (Rotation == Left)return MyDirektion::RotateCCW(Pin.rotation);
					if (Rotation == Down)return MyDirektion::RotateCW(MyDirektion::RotateCW(Pin.rotation));
					if (Rotation == Right)return MyDirektion::RotateCW(Pin.rotation);
					return Neutral;
					};

				for (const auto& Pin : InputPins) {
					PinVerts.emplace_back(GetPosition(Pin), GetRotation(Pin), ColourType{ 1.0,0.0,0.0,0.0 });
				}
				for (const auto& Pin : OutputPins) {
					PinVerts.emplace_back(GetPosition(Pin), GetRotation(Pin), ColourType{ 0.0,1.0,0.0,0.0 });
				}
		}
	}
}

const std::vector<PointIOrientationRGBVertex>& VisualBlockInterior::GetPinVerts() const {
	return PinVerts;
}

const std::vector<TwoPointIRGBAVertex>& VisualBlockInterior::GetBlockVerts() const {
	return BlockVerts;
}

const std::vector<SevenSegVertex>& VisualBlockInterior::GetSevenSegVerts() const {
	return SevenSegVerts;
}

const std::vector<SixteenSegVertex>& VisualBlockInterior::GetSixteenSegVerts() const {
	return SixteenSegVerts;
}

void VisualBlockInterior::AddBlock(const CompressedBlockDataIndex& bedi, const BlockMetadata& Transform) {
	Blocks[bedi].push_back(Transform);
	DirtyBlocks = true;
}

//Invalidates iterators/reverences
bool VisualBlockInterior::RemoveBlock(const BlockIndex& index) {
	if (!Blocks.contains(index.first))return false;
	auto& vec = Blocks[index.first];
	if (vec.size() <= index.second) return false;
	std::swap(vec.back(), vec[index.second]);
	DirtyBlocks = true;
	vec.pop_back();
	return true;
}

std::optional<BlockMetadata> VisualBlockInterior::GetBlockMetadata(const BlockIndex& index) {
	if (!Blocks.contains(index.first))return std::nullopt;
	auto& vec = Blocks[index.first];
	if (vec.size() <= index.second) return std::nullopt;
	return vec[index.second];
}

void VisualBlockInterior::SetBlockMetadata(const BlockIndex& index, const BlockMetadata& Transform) {
	if (!Blocks.contains(index.first))return;
	auto& vec = Blocks[index.first];
	if (vec.size() <= index.second) return;
	DirtyBlocks = true;
	vec[index.second] = Transform;
}

void VisualBlockInterior::StartPlacingBlock(const CompressedBlockDataIndex& bedi, const BlockMetadata& Transform) {
	PlacingBlock.emplace(bedi, Transform);
}

std::optional<std::reference_wrapper<BlockMetadata>> VisualBlockInterior::GetPlacingBlockTransform() {
	if (!PlacingBlock.has_value())return std::nullopt;
	return PlacingBlock.value().second;
}

void VisualBlockInterior::EndPlacingBlock() {
	if (!PlacingBlock.has_value())return;
	AddBlock(PlacingBlock.value().first, PlacingBlock.value().second);
	PlacingBlock = std::nullopt;
}

void VisualBlockInterior::CancalePlacingBlock() {
	PlacingBlock = std::nullopt;
}