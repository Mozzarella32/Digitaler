#include "pch.hpp"
#include "Block.hpp"

#include "OpenGlDefines.hpp"

#ifdef UseCollisionGrid
int VisualBlock::BoxSize = 5;
#endif

const std::vector<PathVertex>& VisualBlock::GetEdges(bool Preview) const {
	if (Preview) {
		return PreviewEdges;
	}
	return Edges;
}

const std::vector<PathVertex>& VisualBlock::GetEdgesMarked(bool Preview) const {
	if (Preview) {
		return PreviewEdges;
	}
	return EdgesMarked;
}

static const  std::vector<PathVertex> EmptyPathVec = {};

const std::vector<PathVertex>& VisualBlock::GetEdgesUnmarked(bool Preview) const {
	if (Preview) {
		return EmptyPathVec;
	}
	return EdgesUnmarked;
}

const std::vector<PointVertex>& VisualBlock::GetSpecialPoints(bool Preview) const {
	if (Preview) {
		return PreviewSpecialPoints;
	}
	return SpecialPoints;
}

const std::vector<PathVertex>& VisualBlock::GetVerts(bool Preview) const {
	if (Preview) {
		return PreviewVerts;
	}
	return Verts;
}
//
//const std::vector<RectRGBAVertex>& VisualBlock::GetBoxes() const {
//	return std::vector<;
//}

VisualBlock::VisualBlock(int FrameCount) :renderer(renderer) {
	PROFILE_FUNKTION;
	
	//return;
	VisualPathData pd;


#ifdef Testing
	//3426
	srand(FrameCount);

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

		pd = VisualPathData( Points[0], Points[1] ,this);

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
	{
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
	}
#endif

	/*pd = VisualPathData({ {0,0},{0,-2} });
	pd.addTo({ 1,-2 });
	pd.addTo({ 1,2 });
	pd.addTo({ 0,2 });
	pd.addTo({ 0,1 });
	pd.addTo({ -1, -3 });

	Paths.emplace_back(pd);*/

	/*pd = PathData({ {0,0},{0,-2} });
	pd.addTo({ 1,-1 });
	pd.addTo({ 1,1 });
	pd.addTo({ 1,1 });
	pd.addTo({ 0,1 });
	pd.addTo({ 0,2 });
	pd.addTo({ 1,2 });
	pd.addTo({ 1,3 });
	pd.addTo({ 0,3 });
	pd.addTo({ 0,-3 });*/

	/*pd = PathData({ {0,0},{0,-2} });
	pd.addTo({ 2,-2 });
	pd.addTo({ 2,-1});
	pd.addTo({ 1,-1});
	pd.addTo({ 0,-1});
	Paths.emplace_back(pd);*/


	/*pd = PathData({ {0,1},{0,0} });
	pd.addTo({ 2,0 });
	pd.addTo({ 2,2 });
	pd.addTo({ 2,-2 });

	auto hr = pd.toHumanReadable();

	Paths.emplace_back(pd);

	pd = PathData({ {0,2},{2,2} });
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
		pd = PathData({ { 5 + i,-530 + i },{ 5 + i,3 + i } });

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
	//Paths.front().setMarked(FrameCount % 20 <= 10);

	//for (int i = 1; i < Paths.size(); i++) {
	//	if ((FrameCount % 200) / 200.0 < 0.1 * i) {
	//		Paths[i].setMarked(true);
	//	}
	//}

}

void VisualBlock::UpdateVectsForVAOs(const MyRectI& ViewRect, const PointType& Mouse) {
	std::vector<std::string> hrs;
	for (auto& Path : Paths) {
		hrs.push_back(Path.Data.toHumanReadable());
		hrs.back().append("\nCompressed:\n");
		hrs.back().append(CompressedPathData(Path.Data).toHumanReadable());
		//Path = VisualPath(VisualPathData(CompressedPathData(Path.Data)));
		hrs.push_back(Path.Data.toHumanReadable());
	}
	if (CachedBoundingBox != ViewRect) {
		CachedBoundingBox = ViewRect;
		Dirty = true;
	}

	if (MouseCached != Mouse) {
		MouseCached = Mouse;
		Dirty = true;
		for (auto& p : Paths) {
			if (p.Intercept(Mouse)) {
				if (!HasPreview()) {
					p.setMarked(true);
				}
				else {
					p.setMarked(false);
				}
			}
			else {
				p.setMarked(false);
			}
		}
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
		if (p.isMarked()) {
			EdgesMarked.append_range(e);
			Verts.append_range(p.getVerts());
		}
		else {
			EdgesUnmarked.append_range(e);
		}
		SpecialPoints.append_range(p.getSpecialPoints());
	}

	Edges.reserve(EdgesMarked.size() + EdgesUnmarked.size());
	Edges.append_range(EdgesMarked);
	Edges.append_range(EdgesUnmarked);

}

void VisualBlock::UpdateVectsForVAOsPreview(const MyRectI& ViewRect, const PointType& Mouse) {
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

		PreviewEdges.append_range(Preview.ComputeAllAndGetEdges(ViewRect));
		PreviewVerts.append_range(Preview.getVerts());
		PreviewSpecialPoints.append_range(Preview.getSpecialPoints());
	}
}

#ifdef UseCollisionGrid
std::vector<PointType> VisualBlock::GetBoxesFromLine(const PointType& A, const PointType& B) {
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

void VisualBlock::RegisterLine(const LineIndex& l, const std::vector<PointNode>& Points, const VisualPathData::VisualPathDataId& Id) {
	//PROFILE_FUNKTION;

	for (const auto& p : GetBoxesFromLine(Points[l.A].Pos, Points[l.B].Pos)) {
		CollisionMap[p].emplace_back( l, Id );
	}
}

void VisualBlock::UnRegisterLine(const LineIndex& l, const std::vector<PointNode>& Points, const VisualPathData::VisualPathDataId& Id) {
	//PROFILE_FUNKTION;
	
	if (Destructing)return;
	for (const auto& p : GetBoxesFromLine(Points[l.A].Pos, Points[l.B].Pos)) {
		auto& lap = CollisionMap[p];
		auto it = std::find(lap.begin(), lap.end(),LineAndPath{l,Id});
		std::swap(*it, lap.back());
		lap.pop_back();
	}
}
//
//std::vector<LineIndex> VisualBlock::GetLinesWith(const PointType& p, const VisualPathData& pd) {
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


LineIndex VisualBlock::GetLineWith(const PointType& p, const VisualPathData& pd) {
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

VisualPath::PathIndex VisualBlock::AddPath(VisualPathData&& p) {
	if (PathsFreeListHead == VisualPath::FreeListEnd) {
		Paths.emplace_back(std::move(p));
		return Paths.size() - 1;
	}

	VisualPath::PathIndex OldHead = PathsFreeListHead;
	PathsFreeListHead = Paths[OldHead].Init(std::move(p));
	return OldHead;
}

bool VisualBlock::TryAbsorb(VisualPath& Path) {
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

bool VisualBlock::hasMark(bool Preview) const {
	return !GetEdgesMarked(Preview).empty();
}

std::optional<VisualPath> VisualBlock::GeneratePreviewPath(const PointType& Mouse) {
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

std::optional<VisualPath> VisualBlock::GeneratePreviewPath() {
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

void VisualBlock::StartDrag(const PointType& p) {
	PreviewData.clear();
	PreviewData.emplace_back(p);
}

//std::vector<MyRectI> VisualBlock::GetBoundingBox() {
//	std::vector<MyRectI> ret;
//	ret.reserve(Paths.size());
//	for (const auto& Path : Paths) {
//		ret.push_back(Path.Data.BoundingBox);
//	}
//	return ret;
//}

//Finised

bool VisualBlock::AddDrag(const PointType& mouse) {
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

void VisualBlock::EndDrag() {
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

void VisualBlock::CancleDrag() {
	PreviewData.clear();
	PreviewCached = std::nullopt;
}

bool VisualBlock::HasPreview() const {
	return !PreviewData.empty();
}
//
//std::string VisualBlock::CollisionMapToString() const {
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