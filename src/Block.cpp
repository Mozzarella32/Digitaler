#include "Block.hpp"
#include "pch.hpp"

#include "OpenGlDefines.hpp"

#include "DataResourceManager.hpp"

#include "AtlasMap.hpp"

#include "Renderer.hpp"

#ifdef UseCollisionGrid
int VisualBlockInterior::BoxSize = 5;
#endif

BufferedVertexVec<TwoPointIVertex>& VisualBlockInterior::GetEdges(bool Preview) {
	if (Preview) {
		return PreviewEdges;
	}
	return Edges;
}

BufferedVertexVec<TwoPointIVertex>& VisualBlockInterior::GetEdgesMarked(bool Preview) {
	if (Preview) {
		return PreviewEdges;
	}
	return EdgesMarked;
}

static BufferedVertexVec<TwoPointIVertex> EmptyPathVec = {};

BufferedVertexVec<TwoPointIVertex>& VisualBlockInterior::GetEdgesUnmarked(bool Preview) {
	if (Preview) {
		return EmptyPathVec;
	}
	return EdgesUnmarked;
}

BufferedVertexVec<PointIVertex>& VisualBlockInterior::GetSpecialPoints(bool Preview) {
	if (Preview) {
		return PreviewSpecialPoints;
	}
	return SpecialPoints;
}

BufferedVertexVec<TwoPointIVertex>& VisualBlockInterior::GetVerts(bool Preview) {
	if (Preview) {
		return PreviewVerts;
	}
	return Verts;
}
//
//const std::vector<TwoPointIRGBAIDVertex>& VisualBlockInterior::GetBlockVerts() const {
//	return std::vector<;
//}

//Returns if need is to redraw
bool VisualBlockInterior::SetHighlited(unsigned int Highlited) {
	if (this->Highlited == Highlited)return false;
	this->Highlited = Highlited;
	Dirty = true;
	DirtyBlocks = true;
	return true;
}

unsigned int VisualBlockInterior::GetHighlited() const {
	return Highlited;
}

bool VisualBlockInterior::HasHighlited() const {
	return Highlited != 0;
}

void VisualBlockInterior::ClearMarked() {
	std::fill(begin(MarkedBlocks), end(MarkedBlocks), false);
}

void VisualBlockInterior::MarkAll() {
	std::fill(begin(MarkedBlocks), end(MarkedBlocks), true);
}

//Returns if need is to redraw
bool VisualBlockInterior::SetMarked(unsigned int Mark, const bool& Value) {
	if (MarkedBlocks.size() <= Mark) return false;
	if (MarkedBlocks[Mark] == Value)return false;
	MarkedBlocks[Mark] = Value;
	Dirty = true;
	DirtyBlocks = true;
	return true;
}

bool VisualBlockInterior::GertMarked(unsigned int Mark) const {
	assert(MarkedBlocks.size() > Mark);
	return MarkedBlocks[Mark];
}

void VisualBlockInterior::MoveMarked(const Eigen::Vector2i& Diff) {
	unsigned int id = 0;
	for (auto& [type, vec] : Blocks) {
		for (auto& block : vec) {
			id++;
			assert(MarkedBlocks.size() > id);
			if (!MarkedBlocks[id]) continue;
			block.Pos += Diff;
		}
	}
}

void VisualBlockInterior::DeleteMarked() {
	std::vector<CompressedBlockDataIndex> Indecies;

	std::stringstream log;

	unsigned int id = 0;
	for (auto& [type, vec] : Blocks) {
		Indecies.push_back(type);
		id += vec.size();
		log << type << " " << vec.size() << "\n";
	}

	log << "ID: " << id << "\n";

	size_t Deleted = 0;

	//id contains highest id
	for (auto it = rbegin(Indecies); it != rend(Indecies); it++) {
		auto& vec = Blocks.at(*it);
		if (vec.size() == 0) continue;
		for (size_t i = vec.size() - 1;; i--) {
			if (!MarkedBlocks[id--]) {
				if (i == 0) break;
				continue;
			}

			if (Highlited > id) {
				Highlited--;
			}
			if (Highlited == id) {
				Highlited = 0;
			}

			Deleted++;
			auto itdelete = vec.begin();
			std::advance(itdelete, i);
			vec.erase(itdelete);
			if (i == 0) break;
		}
	}

	MarkedBlocks.resize(MarkedBlocks.size() - Deleted, false);
	ClearMarked();
}

Eigen::Vector2f VisualBlockInterior::GetMarkedMean() const {
	int id = 0;

	Eigen::Vector2f Mean = { 0,0 };
	int count = 0;

	for (const auto& [Index, MetaVec] : Blocks) {
		const auto& ContainedExteriorOpt = ResourceManager->GetCompressedBlockData(Index);
		if (!ContainedExteriorOpt) {
			assert(false && "you messed up");
			continue;
		}
		const auto& ContainedExterior = ContainedExteriorOpt.value();
		const auto& BlockSize = ContainedExterior.blockExteriorData.Size;
		for (const auto& Meta : MetaVec) {
			id++;
			if (!MarkedBlocks[id])continue;
			Eigen::Vector2f Pos = Meta.Pos.cast<float>();

			switch (Meta.Rotation) {
			case MyDirection::Up:
				Pos += Eigen::Vector2f{ 0,0 };
				break;
			case MyDirection::Right:
				Pos += Eigen::Vector2f{ BlockSize.y(),0 };
				break;
			case MyDirection::Down:
				Pos += Eigen::Vector2f{ BlockSize.x(),-BlockSize.y() };
				break;
			case MyDirection::Left:
				Pos += Eigen::Vector2f{ 0,-BlockSize.x() };
				break;
			}

			Eigen::Vector2f Diff = Pos - Mean;
			Diff /= count + 1;
			Mean += Diff;
			count++;
		}
	}
	return Mean;
}

void VisualBlockInterior::MarkArea(const MyRectF& Area/*, BlockBoundingBoxCallback bbbc*/) {
	ClearMarked();
	int id = 0;

	for (const auto& [Index, MetaVec] : Blocks) {
		std::array<MyRectF, 4> BBS = renderer->GetBlockBoundingBoxes(Index);
		for (const auto& Meta : MetaVec) {
			id++;
			MyRectF BB = BBS[Meta.Rotation];
			if (BB != MyRectF::FromCorners({ 1,1 }, { 1,1 })) {
				int a = 0;
			}
			BB.Position += Eigen::Vector2f{ Meta.Pos.x(), Meta.Pos.y() };
			if (!BB.IsContainedIn(Area))continue;
			SetMarked(id, true);
		}
	}
}

void VisualBlockInterior::RotateMarked() {

	auto Rotate = [](float angle) {
		return Eigen::Matrix2i{
			{(int)std::round(cos(angle)),-(int)std::round(sin(angle))},
			{(int)std::round(sin(angle)),(int)std::round(cos(angle))}
		};
		};

	auto Rotation = Rotate(-M_PI / 2.0);

	Eigen::Vector2f Mean = GetMarkedMean();

	Eigen::Vector2i pos = { int(std::round(Mean.x())),int(std::round(Mean.y())) };

	int id = 0;
	for (auto& [Index, MetaVec] : Blocks) {
		const auto& ContainedExteriorOpt = ResourceManager->GetCompressedBlockData(Index);
		if (!ContainedExteriorOpt) {
			assert(false && "you messed up");
			continue;
		}
		const auto& ContainedExterior = ContainedExteriorOpt.value();
		const auto& BlockSize = ContainedExterior.blockExteriorData.Size;
		for (auto& Meta : MetaVec) {

			id++;
			if (!MarkedBlocks[id])continue;
			Meta.Rotation = MyDirection::RotateCW(Meta.Rotation);
			PointType Off = Meta.Pos - pos;
			Off = Rotation * Off;
			Meta.Pos = pos + Off;
			if (Meta.Rotation == MyDirection::Right) {
				Meta.Pos.x() -= BlockSize.y();
			}
			if (Meta.Rotation == MyDirection::Down) {
				Meta.Pos.x() -= BlockSize.x();
			}
			if (Meta.Rotation == MyDirection::Left) {
				Meta.Pos.x() -= BlockSize.y();
			}
			if (Meta.Rotation == MyDirection::Up) {
				Meta.Pos.x() -= BlockSize.x();
			}
		}
	}
}

void VisualBlockInterior::FlipMarked(bool X) {

	auto Rotate = [](float angle) {
		return Eigen::Matrix2i{
			{(int)std::round(cos(angle)),-(int)std::round(sin(angle))},
			{(int)std::round(sin(angle)),(int)std::round(cos(angle))}
		};
		};

	auto Rotation = Rotate(-M_PI / 2.0);

	Eigen::Vector2f Mean = GetMarkedMean();

	Eigen::Vector2i pos = { int(std::round(Mean.x())),int(std::round(Mean.y())) };

	int id = 0;
	for (auto& [Index, MetaVec] : Blocks) {
		const auto& ContainedExteriorOpt = ResourceManager->GetCompressedBlockData(Index);
		if (!ContainedExteriorOpt) {
			assert(false && "you messed up");
			continue;
		}
		const auto& ContainedExterior = ContainedExteriorOpt.value();
		const auto& BlockSize = ContainedExterior.blockExteriorData.Size;
		for (auto& Meta : MetaVec) {

			id++;
			if (!MarkedBlocks[id])continue;
			if (X) {
				Meta.xflip = !Meta.xflip;
			}
			else {
				Meta.yflip = !Meta.yflip;
			}
			//Meta.Rotation = MyDirection::RotateCW(Meta.Rotation);
			/*PointType Off = Meta.Pos - pos;
			Off = Rotation * Off;
			Meta.Pos = pos + Off;*/
			//if (Meta.Rotation == MyDirection::Right) {
			//	//Meta.Pos.x() -= BlockSize.y();
			//}
			//if (Meta.Rotation == MyDirection::Down) {
			//	//Meta.Pos.x() -= BlockSize.x();
			//}
			//if (Meta.Rotation == MyDirection::Left) {
			//	//Meta.Pos.x() -= BlockSize.y();
			//}
			//if (Meta.Rotation == MyDirection::Up) {
			//	//Meta.Pos.x() -= BlockSize.x();
			//}
		}
	}
}

void VisualBlockInterior::FlipMarkedX() {
	FlipMarked(true);
}

void VisualBlockInterior::FlipMarkedY() {
	FlipMarked(false);
}

VisualBlockInterior::VisualBlockInterior(const CompressedBlockData& data, DataResourceManager* ResourceManager, Renderer* renderer)
	:ResourceManager(ResourceManager), renderer(renderer),

	CharMap([this]() {
	std::map<int, CharInfo> CharMap;
	//         0  1  2   3 4 5 6 7 8 9 10
	//bolditallic id adv l t r b l t r b
	//                   plain   atlas
	for (int i = 0; i < (sizeof(atlasMap) / sizeof(atlasMap[0]) / 11); i++) {
		CharMap[(10000 * atlasMap[i * 11]) + atlasMap[i * 11 + 1]] = {
			(float)atlasMap[i * 11 + 2],{
				(float)atlasMap[i * 11 + 3], (float)atlasMap[i * 11 + 4],
				(float)atlasMap[i * 11 + 5],(float)atlasMap[i * 11 + 6]},
			{(float)atlasMap[i * 11 + 7] / TextAtlasSize.x,(TextAtlasSize.y - (float)atlasMap[i * 11 + 8]) / TextAtlasSize.y,
			(float)atlasMap[i * 11 + 9] / TextAtlasSize.x,(TextAtlasSize.y - (float)atlasMap[i * 11 + 10]) / TextAtlasSize.y} };
	}
	return CharMap;
		}())
{
	PROFILE_FUNKTION;

	Paths.reserve(data.Paths.size());
	for (const auto& pd : data.Paths) {
		Paths.emplace_back(VisualPathData(pd, this));
	}

	std::map<CompressedBlockDataIndex, std::vector<BlockMetadata>> ToInser;
	for (const auto& [Name, Blocks] : data.blockExteriorData.ContainedBlocks) {
		for (const auto& Block : Blocks) {
			ToInser[ResourceManager->GetBlockIndex(Name)].push_back(Block);
		}
	}

	AddBlockBatched(ToInser);

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

void VisualBlockInterior::UpdateVectsForVAOs(const MyRectF& ViewRect, const float& Zoom, const PointType& Mouse) {
	PROFILE_FUNKTION;
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
		UpdateBlocks(Zoom);
	}

	if (!Dirty) return;

	Dirty = false;

	Edges.Clear();
	EdgesMarked.Clear();
	EdgesUnmarked.Clear();
	SpecialPoints.Clear();
	Verts.Clear();

	MyRectI ViewRectI{
		PointType{ViewRect.Position.x(),ViewRect.Position.y()},
		PointType{ViewRect.Size.x(),ViewRect.Size.y()},
	};

	for (auto& p : Paths) {
		const auto& e = p.ComputeAllAndGetEdges(ViewRectI);
		if (p.IsMarked()) {
			EdgesMarked.Append(e);
			Verts.Append(p.getVerts());
		}
		else {
			EdgesUnmarked.Append(e);
		}
		SpecialPoints.Append(p.getSpecialPoints());
	}

	Edges.AppendOther(EdgesMarked, EdgesUnmarked);
	Edges.AppendOther(EdgesMarked);
	Edges.AppendOther(EdgesUnmarked);
}

void VisualBlockInterior::UpdateVectsForVAOsPreview(const MyRectF& ViewRect, const PointType& Mouse) {
	PROFILE_FUNKTION;
	if (CachedBoundingBoxPreview != ViewRect) {
		CachedBoundingBoxPreview = ViewRect;
		PreviewBoundingBoxIsDirty = true;
	}

	if (MouseCachedPreview != Mouse) {
		MouseCachedPreview = Mouse;
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

		PreviewEdges.Clear();
		PreviewVerts.Clear();
		PreviewSpecialPoints.Clear();

		if (!PreviewCached.has_value()) return;

		auto& Preview = PreviewCached.value();

		MyRectI ViewRectI{
			PointType{ViewRect.Position.x(),ViewRect.Position.y()},
			PointType{ViewRect.Size.x(),ViewRect.Size.y()},
		};

		const auto& ToAppend = Preview.ComputeAllAndGetEdges(ViewRectI);
		PreviewEdges.Append(ToAppend);
		PreviewVerts.Append(Preview.getVerts());
		PreviewSpecialPoints.Append(Preview.getSpecialPoints());
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
		assert(it != end(lap));
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

bool VisualBlockInterior::hasMark(bool Preview) {
	return !GetEdgesMarked(Preview).Empty();
}

bool VisualBlockInterior::hasUnmarked(bool Preview) {
	return !GetEdgesUnmarked(Preview).Empty();
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

size_t VisualBlockInterior::GetDragSize() const {
	return PreviewData.size();
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
	PreviewEdges.Clear();
	PreviewVerts.Clear();
	PreviewSpecialPoints.Clear();
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

PointType VisualBlockInterior::GetPinPosition(const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin, const int& Expoltion) {
	const MyDirection::Direction& Rotation = Meta.Rotation;
	PointType TopLeft;
	PointType BottomLeft;
	PointType TopRight;
	PointType BottomRight;
	PointType Flip = { 1 - 2 * Meta.xflip,1 - 2 * Meta.yflip };
	if (Rotation == MyDirection::Left || Rotation == MyDirection::Right) {
		TopLeft = Meta.Pos;
		//TopLeft = Meta.Pos + PointType{ -1,-1 }*int((BlockSize.y() - BlockSize.x()) / 2.0);
		BottomLeft = TopLeft - PointType{ 0,BlockSize.x() }.cwiseProduct(Flip);
		TopRight = TopLeft + PointType{ BlockSize.y(),0 }.cwiseProduct(Flip);
		BottomRight = TopLeft + PointType{ BlockSize.y(), -BlockSize.x() }.cwiseProduct(Flip);
	}
	else {
		TopLeft = Meta.Pos;
		BottomLeft = TopLeft - PointType{ 0,BlockSize.y() }.cwiseProduct(Flip);
		TopRight = TopLeft + PointType{ BlockSize.x(),0 }.cwiseProduct(Flip);
		BottomRight = TopLeft + PointType{ BlockSize.x(), -BlockSize.y() }.cwiseProduct(Flip);
	}
	using namespace MyDirection;
	switch (Pin.Rotation) {
	case Up:
		switch (Rotation) {
		case Up: return TopLeft + PointType{ Pin.Offset, Expoltion }.cwiseProduct(Flip);
		case Right: return TopRight + PointType{ Expoltion,-Pin.Offset }.cwiseProduct(Flip);
		case Down: return BottomRight + PointType{ -Pin.Offset,-Expoltion }.cwiseProduct(Flip);
		case Left: return BottomLeft + PointType{ -Expoltion,Pin.Offset }.cwiseProduct(Flip);
		}
	case Right:
		switch (Rotation) {
		case Up: return TopRight + PointType{ Expoltion,-Pin.Offset }.cwiseProduct(Flip);
		case Right: return BottomRight + PointType{ -Pin.Offset,-Expoltion }.cwiseProduct(Flip);
		case Down: return BottomLeft + PointType{ -Expoltion,Pin.Offset }.cwiseProduct(Flip);
		case Left: return TopLeft + PointType{ Pin.Offset,Expoltion }.cwiseProduct(Flip);
		}
	case Down:
		switch (Rotation) {
		case Up: return BottomLeft + PointType{ Pin.Offset,-Expoltion }.cwiseProduct(Flip);
		case Right: return TopLeft + PointType{ -Expoltion,-Pin.Offset }.cwiseProduct(Flip);
		case Down: return TopRight + PointType{ -Pin.Offset,Expoltion }.cwiseProduct(Flip);
		case Left: return BottomRight + PointType{ Expoltion,Pin.Offset }.cwiseProduct(Flip);
		}
	case Left:
		switch (Rotation) {
		case Up: return TopLeft + PointType{ -Expoltion,-Pin.Offset }.cwiseProduct(Flip);
		case Right: return TopRight + PointType{ -Pin.Offset,Expoltion }.cwiseProduct(Flip);
		case Down: return BottomRight + PointType{ Expoltion,Pin.Offset }.cwiseProduct(Flip);
		case Left: return BottomLeft + PointType{ Pin.Offset,-Expoltion }.cwiseProduct(Flip);
		}

	}
	assert(false);
	return {};
}

MyDirection::Direction VisualBlockInterior::GetPinRotation(const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin) {
	using enum MyDirection::Direction;
	if (Meta.Rotation == Up)return Pin.Rotation;
	if (Meta.Rotation == Left)return MyDirection::RotateCCW(Pin.Rotation);
	if (Meta.Rotation == Down)return MyDirection::RotateCW(MyDirection::RotateCW(Pin.Rotation));
	if (Meta.Rotation == Right)return MyDirection::RotateCW(Pin.Rotation);
	return Neutral;
}

void VisualBlockInterior::ShowMultiplicity(const float& Zoom, const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin) {
	using namespace MyDirection;

	if (Zoom >= 0.01f) return;

	if (Pin.Multiplicity == 1) {
		return;
	}
	std::string Text;
	if (Pin.Multiplicity == CompressedBlockData::BlockExteriorData::Pin::VariableMultiplicity) {
		Text = "§";
	}
	else {
		Text = std::to_string(Pin.Multiplicity);
	}
	Point<float> Offset;

	float Scale = 0.4;
	auto ext = GetTextExtend(Text, false, false, Scale);

	float maxWidth = 0.42;
	if (ext.Width > maxWidth) {
		Scale *= maxWidth / ext.Width;
		ext = GetTextExtend(Text, false, false, Scale);
	}

	const MyDirection::Direction& d = GetPinRotation(Meta, Pin);

	Offset += d;//Up
	if (d == MyDirection::Left || d == MyDirection::Right) {
		Offset.x *= -1;
	}
	Offset *= 0.35f;

	const auto Pos = GetPinPosition(BlockSize, Meta, Pin, 1);
	AddText(Text,
		Point<float>{(float)Pos.x() + Offset.x, (float)Pos.y() + Offset.y},
		true,
		TextPlacmentFlags::x_Center | TextPlacmentFlags::y_Center,
		true, false, Scale,
		MyDirection::ToReadable(GetPinRotation(Meta, Pin)),
		ColourType{ 0.687,0.933,0.845,1.0 });
	//Do final stuff
}

void VisualBlockInterior::ShowLable(const float& Zoom, const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin) {
	using namespace MyDirection;

	if (Zoom >= 0.015f) return;

	Point<float> Offset;

	float Scale = 0.4;

	auto ext = GetTextExtend(Pin.Name, false, false, Scale);

	float Hightmul = Scale * LineHeight;

	switch (GetPinRotation(Meta, Pin)) {
	case Up:
		Offset = { 0.0f,-0.7f - ext.Width };
		break;
	case Down:
		Offset = { 0.0f,0.7f };
		break;
	case Left:
		Offset = { 0.7f, 0.0f };
		break;
	case Right:
		Offset = { -0.7f - ext.Width,0.0f };
		break;
	default:
		break;
	}
	//const MyDirection::Direction& d = GetRotation(Pin);

	//Offset += d;//Up
	//if (d == MyDirection::Left || d == MyDirection::Right) {
	//	Offset.x *= -1;
	//}
	//Offset *= 0.7f;

	const auto Pos = GetPinPosition(BlockSize, Meta, Pin, 1);
	AddText(Pin.Name,
		Point<float>{(float)Pos.x() + Offset.x, (float)Pos.y() + Offset.y},
		true,
		TextPlacmentFlags::x_Right | TextPlacmentFlags::y_Center,
		false, false, Scale,
		MyDirection::ToReadable(MyDirection::RotateCCW(GetPinRotation(Meta, Pin))),
		ColourType{ 1.0,0.5,0.0,1.0 });
	//Do final stuff
};

void VisualBlockInterior::ShowBlockLabl(const PointType& BlockSize, const BlockMetadata& Meta, const std::string& Name) {
	Point<float> Off{ 0,0 };
	PointType TopLeft;
	Point<float> Flip = { 1.0f - 2.0f * Meta.xflip,1.0f - 2.0f * Meta.yflip };
	if (Meta.Rotation == MyDirection::Left || Meta.Rotation == MyDirection::Right) {
		Off += Point<float>{BlockSize.y() / 2.0f, -BlockSize.x() / 2.0f} *Flip;
		//TopLeft = Meta.Pos + PointType{ -1,-1 }*int((BlockSize.y() - BlockSize.x()) / 2.0);
		TopLeft = Meta.Pos;
	}
	else {
		Off += Point<float>{BlockSize.x() / 2.0f, -BlockSize.y() / 2.0f} *Flip;
		TopLeft = Meta.Pos;
	}

	AddText(Name,
		Point<float>{(float)TopLeft.x(), (float)TopLeft.y()} + Off,
		true,
		TextPlacmentFlags::x_Center | TextPlacmentFlags::y_Center,
		true, false, 0.5,
		MyDirection::ToReadable(Meta.Rotation));
};

void VisualBlockInterior::UpdateBlocks(const float& Zoom) {
	if (!DirtyBlocks)return;
	DirtyBlocks = false;
	PinVerts.Clear();
	BlockVerts.Clear();
	SevenSegVerts.Clear();
	SixteenSegVerts.Clear();
	RoundedPinVerts.Clear();
	AndVerts.Clear();
	OrVerts.Clear();
	XOrVerts.Clear();
	//NDotVerts.Clear();
	//NotTriangleVerts.Clear();
	MuxVerts.Clear();
	StaticTextVerts.Clear();
	//auto& buff = renderer->GetAreaSelectVerts();
	//buff.Clear();

	/*std::string T = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnoqrstuvwxyzƒ÷‹‰ˆ¸";

	AddText(T, Point<float>{0.0, 0.0}, true, false, false, 1.0, MyDirection::Up, ColourType{ 0.0,1.0,1.0,1.0 });
	AddText(T, Point<float>{0.0, 1.0}, true, false, true, 1.0, MyDirection::Up, ColourType{ 0.0,1.0,1.0,1.0 });
	AddText(T, Point<float>{0.0, 2.0}, true, true, false, 1.0, MyDirection::Up, ColourType{ 0.0,1.0,1.0,1.0 });
	AddText(T, Point<float>{0.0, 3.0}, true, true, true, 1.0, MyDirection::Up, ColourType{ 0.0,1.0,1.0,1.0 });*/

	//AddText("RightTop", Point<float>{-30, 0}, true, TextPlacmentFlags::x_Right| TextPlacmentFlags::y_Top);
	//AddText("RightCenter", Point<float>{-30, -2}, true, TextPlacmentFlags::x_Right | TextPlacmentFlags::y_Center);
	//AddText("RightBottom", Point<float>{-30, -4}, true, TextPlacmentFlags::x_Right | TextPlacmentFlags::y_Bottom);
	//AddText("CenterTop", Point<float>{-30, -6}, true, TextPlacmentFlags::x_Center | TextPlacmentFlags::y_Top);
	//AddText("CenterCenter", Point<float>{-30, -8}, true, TextPlacmentFlags::x_Center | TextPlacmentFlags::y_Center);
	//AddText("CenterBottom", Point<float>{-30, -10}, true, TextPlacmentFlags::x_Center | TextPlacmentFlags::y_Bottom);
	//AddText("LeftTop", Point<float>{-30, -12}, true, TextPlacmentFlags::x_Left | TextPlacmentFlags::y_Top);
	//AddText("LeftCenter", Point<float>{-30, -14}, true, TextPlacmentFlags::x_Left | TextPlacmentFlags::y_Center);
	//AddText("LeftBottom", Point<float>{-30, -16}, true, TextPlacmentFlags::x_Left | TextPlacmentFlags::y_Bottom);

	int id = 0;

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

		auto BBs = renderer->GetBlockBoundingBoxes(IndexContained);

		int i = 0;
		for (const BlockMetadata& Meta : MetaVec) {
			++i;
			id++;
			const PointType& BlockSize = ContainedExterior.blockExteriorData.Size;
			const PointType& Pos = Meta.Pos;
			const MyDirection::Direction& Rotation = Meta.Rotation;

			auto BB = BBs[Meta.Rotation];
			BB.Position += Eigen::Vector2f{ Meta.Pos.x(), Meta.Pos.y() };

			if (!CachedBoundingBox.Intersectes(BB)) {
				continue;
			}

			//buff.Emplace(BB.Position, BB.Position + BB.Size, ColourType{ 0.0,1.0,0.5,1.0 });

			const auto& SB = ResourceManager->GetSpecialBlockIndex();

			PointType TopLeft;
			PointType BottomRight;
			PointType Flip = { 1 - 2 * Meta.xflip,1 - 2 * Meta.yflip };
			if (Rotation == MyDirection::Left || Rotation == MyDirection::Right) {
				TopLeft = Meta.Pos;
				BottomRight = TopLeft + PointType{ BlockSize.y(), -BlockSize.x() }.cwiseProduct(Flip);
			}
			else {
				TopLeft = Meta.Pos;
				BottomRight = TopLeft + PointType{ BlockSize.x(), -BlockSize.y() }.cwiseProduct(Flip);
			}

			assert(MarkedBlocks.size() > id);
			ColourType Color{};
			if (MarkedBlocks[id]) {
				Color = ColourType{ 1.0,0.0,1.0,1.0 };
			}
			else if (id == Highlited) {
				Color = ColourType{ 1.0,1.0,0.0,1.0 };
			}

			if (IndexContained == SB.SevengSeg) {
				SevenSegVerts.Emplace(Meta, time(0) % 0x10, ColourType{ 0.78,0.992,0.0,1.0 });
				BlockVerts.Emplace(id, TopLeft, BottomRight, ColourType{ 0.1,0.1,0.1,1.0 }, Color);
			}
			else if (IndexContained == SB.SixteenSeg) {
				static std::array<int, 218> Translation = { 73,99,104,32,104,97,98,101,32,106,101,116,122,116,32,101,105,110,102,117,110,107,116,105,111,110,105,101,114,101,110,100,101,115,49,54,32,83,101,103,109,101,110,116,32,68,105,115,112,108,97,121,100,97,115,32,97,108,108,101,32,97,115,99,105,105,32,90,101,105,99,104,101,110,100,97,114,115,116,101,108,108,101,110,32,107,97,110,110,58,48,49,50,51,52,53,54,55,56,57,116,104,101,32,113,117,105,99,107,32,98,114,111,119,110,32,102,111,120,32,106,117,109,112,115,32,111,118,101,114,32,116,104,101,32,108,97,122,121,32,100,111,103,84,72,69,32,81,85,73,67,75,32,66,82,79,87,78,32,70,79,88,32,74,85,77,80,83,32,79,86,69,82,32,84,72,69,32,76,65,90,89,32,68,79,71,33,64,35,36,37,94,38,42,40,41,95,45,43,123,125,124,58,34,60,62,63,96,126,91,93,92,59,39,44,46,47,126, };
				SixteenSegVerts.Emplace(Meta, Translation[std::max(i - 5, 0)], ColourType{ 0.992,0.43,0.0,1.0 });
				BlockVerts.Emplace(id, TopLeft, BottomRight, ColourType{ 0.1,0.1,0.1,1.0 }, Color);
			}
			else if (IndexContained == SB.And || IndexContained == SB.Or || IndexContained == SB.XOr) {
				if (IndexContained == SB.And)AndVerts.Emplace(id, Meta, Color);
				else if (IndexContained == SB.Or)OrVerts.Emplace(id, Meta, Color);
				else if (IndexContained == SB.XOr)XOrVerts.Emplace(id, Meta, Color);

				for (const auto& Pin : InputPins) {
					RoundedPinVerts.Emplace(id, GetPinPosition(BlockSize, Meta, Pin, 1), ColourType{ 1.0,0.0,0.0,0.0 });
				}
				for (const auto& Pin : OutputPins) {
					RoundedPinVerts.Emplace(id, GetPinPosition(BlockSize, Meta, Pin, 1), ColourType{ 0.0,1.0,0.0,0.0 });
				}
				ShowBlockLabl(BlockSize, Meta, Name);
				continue;
			}
			else if (IndexContained == SB.Mux) {
				MuxVerts.Emplace(id, Meta, 2, 0, Color);
			}
			else {
				BlockVerts.Emplace(id, TopLeft, BottomRight, ColourType{ 0.5,0.5,1.0,1.0 }, Color);
				ShowBlockLabl(BlockSize, Meta, Name);
				/*if (id == highlited)
					RoundedPinVerts.Emplace(Pos, ColourType{ 1.0,0.0,1.0,0.0 });*/
			}

			for (const auto& Pin : InputPins) {
				PinVerts.Emplace(id, GetPinPosition(BlockSize, Meta, Pin, 1), GetPinRotation(Meta, Pin), ColourType{ 0.7,0.0,0.0,0.0 }, Color);
				ShowMultiplicity(Zoom, BlockSize, Meta, Pin);
				ShowLable(Zoom, BlockSize, Meta, Pin);
			}
			for (const auto& Pin : OutputPins) {
				PinVerts.Emplace(id, GetPinPosition(BlockSize, Meta, Pin, 1), GetPinRotation(Meta, Pin), ColourType{ 0.0,0.7,0.0,0.0 }, Color);
				ShowMultiplicity(Zoom, BlockSize, Meta, Pin);
				ShowLable(Zoom, BlockSize, Meta, Pin);
			}
		}
	}
}

BufferedVertexVec<PointIOrientationRGBRHGHBHIDVertex>& VisualBlockInterior::GetPinVerts() {
	return PinVerts;
}

BufferedVertexVec<TwoPointIRGBAIDVertex>& VisualBlockInterior::GetBlockVerts() {
	return BlockVerts;
}

BufferedVertexVec<SevenSegVertex>& VisualBlockInterior::GetSevenSegVerts() {
	return SevenSegVerts;
}

BufferedVertexVec<SixteenSegVertex>& VisualBlockInterior::GetSixteenSegVerts() {
	return SixteenSegVerts;
}

BufferedVertexVec<PointIRGBIDVertex>& VisualBlockInterior::GetRoundedPinVerts() {
	return RoundedPinVerts;
}

BufferedVertexVec<PointIOrientationRGBIDVertex>& VisualBlockInterior::GetAndVerts() {
	return AndVerts;
}

BufferedVertexVec<PointIOrientationRGBIDVertex>& VisualBlockInterior::GetOrVerts() {
	return OrVerts;
}

BufferedVertexVec<PointIOrientationRGBIDVertex>& VisualBlockInterior::GetXOrVerts() {
	return XOrVerts;
}

//BufferedVertexVec<PointIOrientationRGBIDVertex>& VisualBlockInterior::GetNotTriangleVerts() {
//	return NotTriangleVerts;
//}
//
//BufferedVertexVec<PointIOrientationRGBIDVertex>& VisualBlockInterior::GetNDotVerts() {
//	return NDotVerts;
//}


BufferedVertexVec<MuxIDVertex>& VisualBlockInterior::GetMuxVerts() {
	return MuxVerts;
}

BufferedVertexVec<TextVertex>& VisualBlockInterior::GetStaticTextVerts() {
	return StaticTextVerts;
}

BufferedVertexVec<TextVertex>& VisualBlockInterior::GetDynamicTextVerts() {
	return DynamicTextVerts;
}


void VisualBlockInterior::AddBlock(const CompressedBlockDataIndex& bedi, const BlockMetadata& Transform) {
	Blocks[bedi].push_back(Transform);

	DirtyBlocks = true;

	size_t BlockCount = 0;
	for (const auto& [id, vec] : Blocks) {
		BlockCount += vec.size();
	}
	std::fill(begin(MarkedBlocks), end(MarkedBlocks), false);
	MarkedBlocks.resize(BlockCount + 1, false);
}

void VisualBlockInterior::AddBlockBatched(const std::map<CompressedBlockDataIndex, std::vector<BlockMetadata>>& ToInsert) {

	for (const auto& [id, vec] : ToInsert) {
		Blocks[id].insert(Blocks[id].end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
	}

	DirtyBlocks = true;

	size_t BlockCount = 0;
	for (const auto& [id, vec] : Blocks) {
		BlockCount += vec.size();
	}
	std::fill(begin(MarkedBlocks), end(MarkedBlocks), false);
	MarkedBlocks.resize(BlockCount + 1, false);
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


int VisualBlockInterior::GetCharMapIndex(const char& c, const bool& bold, const bool& italic) {
	return static_cast<unsigned char>(c) + (10000 * ((int)bold + (int)italic * 2));
}

VisualBlockInterior::TextInfo VisualBlockInterior::GetTextExtend(const std::string& Text, const bool& Bold, const bool& Italic, const float& Scale) {
	TextInfo i;
	float Highest = -LineHeight * Scale;
	float Lowest = LineHeight * Scale;
	i.LineWidths.push_back(0);
	float y = 0;

	if (!CharMap.contains(GetCharMapIndex('?', Bold, Italic)))return{};

	static CharInfo ciQuestion = CharMap.at(GetCharMapIndex('?', Bold, Italic));


	for (const auto& c : Text) {
		if (c == '\n') {
			y -= LineHeight * Scale;
			i.LineWidths.push_back(0);
			continue;
		}

		const CharInfo& ci = CharMap.contains(GetCharMapIndex(c, Bold, Italic))
			? CharMap.at(GetCharMapIndex(c, Bold, Italic))
			: ciQuestion;

		i.LineWidths.back() += ci.Advance * Scale;
		if (i.LineWidths.size() == 1)
			Highest = std::max(Highest, ci.CursorOffsets[3] * Scale);
		Lowest = std::min(Highest, y + ci.CursorOffsets[1] * Scale);
	}
	i.Width = *std::min_element(i.LineWidths.begin(), i.LineWidths.end());
	i.TheoreticalHeight = std::abs(TextAscender * Scale) + std::abs(TextDescender * Scale)
		+ (i.LineWidths.size() - 1) * LineHeight * Scale;
	i.EvendentHeight = Highest - Lowest;
	return i;
}

void VisualBlockInterior::AddText(const std::string& Text, const Point<float>& Pos, const bool& Static, int TextPlacmentFlag, const bool& Bold, const bool& Italic, const float& Scale, MyDirection::Direction d, const ColourType& ForgroundColor, const ColourType& BackgroundColor) {
	using enum TextPlacmentFlags;

	Point<float> OFF = { 0,0 };

	//static const constexpr std::map<MyDirection::Direction, Point<float>> 

	Point<float> Up;
	Point<float> Right;
	//auto d2 = d;
	//d = MyDirection::Left;

	Up += d;
	Right += MyDirection::RotateCW(d);

	//OFF += ((LineHeight / 2.0) * Up);

	if (d == MyDirection::Left || d == MyDirection::Right) {
		Right.y *= -1;
		Up.x *= -1;
	}


	/*Right.y *= -1; */

	if (!CharMap.contains(GetCharMapIndex('?', Bold, Italic)))return;

	static CharInfo ciQuestion = CharMap.at(GetCharMapIndex('?', Bold, Italic));

	if (TextPlacmentFlag != (x_Right | y_Top)) {
		auto extend = GetTextExtend(Text, Bold, Italic, Scale);

		if (TextPlacmentFlag & x_Center) {
			OFF -= Right * extend.Width / 2.0;
		}

		if (TextPlacmentFlag & y_Center) {
			OFF += Up * extend.EvendentHeight / 2.0;
		}

		if (TextPlacmentFlag & x_Left) {
			OFF -= Right * extend.Width;
		}

		if (TextPlacmentFlag & y_Bottom) {
			OFF += Up * extend.EvendentHeight;
		}
	}


	for (const char& c : Text) {
		if (c == '\n') {
			OFF -= (OFF * Right * Right);//x = 0
			OFF += LineHeight * Up * Scale;
			continue;
		}
		//c = (c + rand()) % 255;

		/*for(int y = 0; y )*/

		CharInfo ci = CharMap.contains(GetCharMapIndex(c, Bold, Italic)) ?
			ci = CharMap.at(GetCharMapIndex(c, Bold, Italic))
			: ciQuestion;

		//std::array<float, 4> Clip = ci.Clip;

		if (c == ' ') {
			OFF += ci.Advance * Right * Scale;
			continue;
		}

		/*float Clip[4] = { 0.0,0.0,-0.0,-0.0 };
		float CursorOffsets[4] = { 0.1,0.1,-0.1,-0.1 };*/
		if (Static) {
			StaticTextVerts.Dirty = true;
			StaticTextVerts.Emplace(
				Pos + OFF, ci.CursorOffsets.data(), ci.Clip.data(), Scale, d, ForgroundColor, BackgroundColor
			);

			//StaticTextVerts.Emplace(
			//	Pos + OFF, CursorOffsets, Clip, d, ForgroundColor, ColourType{ 1.0,0.0,1.0,0.5 }
			//);
		}
		else {
			DynamicTextVerts.Dirty = true;
			DynamicTextVerts.Emplace(
				Pos + OFF, ci.CursorOffsets.data(), ci.Clip.data(), Scale, d, ForgroundColor, BackgroundColor
			);
			/*	DynamicTextVerts.Emplace(
					Pos + OFF, CursorOffsets, Clip, d, ForgroundColor, ColourType{ 1.0,0.0,1.0,0.5 }
				);*/
		}

		//Clip[0] = Clip[0] / AtlasSize.x;
		//Clip[1] = Clip[1] / AtlasSize.y;
		//Clip[2] = Clip[2] / AtlasSize.x;
		//Clip[3] = Clip[3] / AtlasSize.y;

		//Point<float> TextSize = { Clip[0] - Clip[2], Clip[1] - Clip[3] };

		/*float TargetHight = 1.0;
		TextSize = { TextSize.x * TargetHight / TextSize.y,TargetHight };*/

		/*TextVerts.emplace_back(ci.CursorOffsets[0] + OFF.x, ci.CursorOffsets[1] - OFF.y, Clip[0], Clip[1]);
		TextVerts.emplace_back(ci.CursorOffsets[0] + OFF.x, ci.CursorOffsets[3] - OFF.y, Clip[0], Clip[3]);
		TextVerts.emplace_back(ci.CursorOffsets[2] + OFF.x, ci.CursorOffsets[1] - OFF.y, Clip[2], Clip[1]);
		TextVerts.emplace_back(ci.CursorOffsets[0] + OFF.x, ci.CursorOffsets[3] - OFF.y, Clip[0], Clip[3]);
		TextVerts.emplace_back(ci.CursorOffsets[2] + OFF.x, ci.CursorOffsets[1] - OFF.y, Clip[2], Clip[1]);
		TextVerts.emplace_back(ci.CursorOffsets[2] + OFF.x, ci.CursorOffsets[3] - OFF.y, Clip[2], Clip[3]);*/

		OFF += ci.Advance * Right * Scale;
	}
}