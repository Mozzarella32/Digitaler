#include "Block.hpp"
#include "pch.hpp"

#include "OpenGlDefines.hpp"

#include "DataResourceManager.hpp"
#include "Renderer.hpp"

#include "RenderTextUtility.hpp"

#ifdef UseCollisionGrid
int VisualBlockInterior::BoxSize = 5;
#endif

BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& VisualBlockInterior::GetEdges(bool Floating) {
	if (Floating) {
		return FloatingEdges;
	}
	return Edges;
}

BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& VisualBlockInterior::GetEdgesMarked(bool Floating) {
	if (Floating) {
		return FloatingEdges;
	}
	return EdgesMarked;
}

static BufferedVertexVec<TwoPointIRGBRHGHBHVertex> EmptyPathVec = {};

BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& VisualBlockInterior::GetEdgesUnmarked(bool Floating) {
	if (Floating) {
		return EmptyPathVec;
	}
	return EdgesUnmarked;
}

BufferedVertexVec<PointIRGBVertex>& VisualBlockInterior::GetSpecialPoints(bool Floating) {
	if (Floating) {
		return FloatingSpecialPoints;
	}
	return SpecialPoints;
}

BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& VisualBlockInterior::GetVerts(bool Floating) {
	if (Floating) {
		return FloatingVerts;
	}
	return Verts;
}

BufferedVertexVec<TwoPointIRGBRHGHBHVertex>& VisualBlockInterior::GetConflictPoints(bool Floating) {
	if (Floating) {
		return EmptyPathVec;
	}
	return ConflictPoints;
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

std::optional<CompressedBlockDataIndex> VisualBlockInterior::GetBlockIdByStencil(unsigned int Id) const {
	unsigned int id = 0;
	for (auto& [type, vec] : Blocks) {
		for (auto& block : vec) {
			id++;
			if (id != Id) continue;
			return type;
		}
	}
	return std::nullopt;
}

void VisualBlockInterior::ClearMarked() {
	bool HadMarkedBlocks = HasMarkedBlocks();
	if (HadMarkedBlocks) {
		std::fill(begin(MarkedBlocks), end(MarkedBlocks), false);
		NumMarkedBlocks = 0;
		DirtyBlocks = true;
	}

	//Dosen't use HasMarkedPaths becaus that would be two sweeps
	bool FoundPath = false;
	for (auto& p : Paths) {
		if (p.IsFree()) continue;
		FoundPath |= p.ClearMarkedArea();
	}
	if (FoundPath) {
		MergeAfterMove();
	}
	Dirty |= HadMarkedBlocks | FoundPath;
}

void VisualBlockInterior::MarkAll() {
	bool HasUnmarkedBlocks = NumMarkedBlocks != MarkedBlocks.size();
	if (HasUnmarkedBlocks) {
		std::fill(begin(MarkedBlocks), end(MarkedBlocks), true);
		NumMarkedBlocks = MarkedBlocks.size() - 1;
		DirtyBlocks = true;
	}
	bool FoundPath = false;
	for (auto& p : Paths) {
		if (p.IsFree()) continue;
		FoundPath |= p.SetMarkedArea(FullRectI);
	}
	Dirty |= HasUnmarkedBlocks | FoundPath;
}

bool VisualBlockInterior::ToggleMarkHoverPath() {
	bool Return = false;
	for (auto& p : Paths) {
		if (p.IsFree()) continue;
		Return |= p.ToggleMarkedIfHover();
	}
	if (Return)
		Dirty = true;
	return Return;
}

//Carefull O(n) with n = Paths.size()
bool VisualBlockInterior::HasAnythingMarked() const {
	return HasMarkedBlocks() || HasMarkedPaths();//Short circuit simple check
}

//Carefull O(n) with n = Paths.size()
bool VisualBlockInterior::HasMarkedPaths() const {
	bool Return = false;
	for (const auto& p : Paths) {
		if (p.IsFree()) continue;
		if (p.HasMarked()) Return = true;
	}
	return Return;
}

bool VisualBlockInterior::HasMarkedBlocks() const {
	return NumMarkedBlocks != 0;
}

//Returns if need is to redraw
bool VisualBlockInterior::SetMarked(unsigned int Mark, const bool& Value) {
	if (MarkedBlocks.size() <= Mark) return false;
	if (MarkedBlocks[Mark] == Value)return false;
	MarkedBlocks[Mark] = Value;
	if (Value) {
		NumMarkedBlocks++;
	}
	else {
		NumMarkedBlocks--;
	}
	Dirty = true;
	DirtyBlocks = true;
	return true;
}

bool VisualBlockInterior::GetMarked(unsigned int Mark) const {
	assert(MarkedBlocks.size() > Mark);
	if (Mark == 0) return false;
	return MarkedBlocks[Mark];
}

bool VisualBlockInterior::HasMarkedPathAt(const PointType& Mouse) const {
	for (const auto& p : Paths) {
		if (p.IsFree()) continue;
		if (p.HasMarkedAt(Mouse)) {
			return true;
		}
	}
	return false;
}

//Returnes if Modifyed
bool VisualBlockInterior::ApplyToMarked(const BlockApplyer& Blockapplyer, const PathApplyer& Pathapplyer) {
	if (!HasAnythingMarked()) return false;
	unsigned int id = 0;
	bool HadMarkedBlock = HasMarkedBlocks();
	if (HadMarkedBlock) {
		for (auto& [type, vec] : Blocks) {
			const auto& DataOpt = ResourceManager->GetCompressedBlockData(type);
			if (!DataOpt) {
				assert(false && "you messed up");
				continue;
			}
			const auto& BlockSize = DataOpt->blockExteriorData.Size;

			for (auto& block : vec) {
				id++;
				assert(MarkedBlocks.size() > id);
				if (!MarkedBlocks[id]) continue;
				Blockapplyer(block, BlockSize);
			}
		}
	}
	Dirty |= HadMarkedBlock;
	DirtyBlocks |= HadMarkedBlock;

	const bool FoundPath = ApplyToMarkedPath(Pathapplyer);

	return HadMarkedBlock || FoundPath;
}

//Returnes if Modifyed
bool VisualBlockInterior::ApplyToMarkedPath(const PathApplyer& Pathapplyer) {
	bool FoundPath = false;

	//split if necessary
	if (!Moving) {
		Moving = true;
		for (size_t i = 0; i < Paths.size(); i++) {
			auto& p = Paths[i];
			if (p.IsFree()) continue;
			if (!p.IsFullyMarked() && p.HasMarked()) {
				SplitPath(i);
			}
		}
	}

	//apply
	for (size_t i = 0; i < Paths.size(); i++) {
		auto& p = Paths[i];
		if (p.IsFree()) continue;
		if (!p.IsFullyMarked()) continue;
		FoundPath = true;
		Pathapplyer(p);
	}

	Dirty |= FoundPath;
	PathNeedsMerging |= FoundPath;

	return FoundPath;
}

PointType VisualBlockInterior::GetPositionDiff(const BlockMetadata& Meta, const PointType& BlockSize) {
	using enum MyDirection::Direction;
	PointType Off{};

	switch (Meta.Rotation) {
	case Up:
		Off -= PointType{ 0,0 };
		break;
	case Right:
		Off -= PointType{ BlockSize.y(),0 };
		break;
	case Down:
		Off -= PointType{ BlockSize.x(),-BlockSize.y() };
		break;
	case Left:
		Off -= PointType{ 0,-BlockSize.x() };
		break;
	}


	auto ToVec2f = [](const MyDirection::Direction& d) {
		switch (d) {
		case Up:
			return PointType{ 0,-1 };
		case Down:
			return PointType{ 0,1 };
		case Left:
			return PointType{ -1,0 };
		case Right:
			return PointType{ 1,0 };
		default:
			return PointType{ 0,0 };
		};
		};

	auto UpV = ToVec2f(Meta.Rotation);
	auto RightV = ToVec2f(MyDirection::RotateCW(Meta.Rotation));

	if (Meta.xflip) {
		int Scale = (Meta.Rotation == Up || Meta.Rotation == Down) ? BlockSize.x() : BlockSize.y();
		auto Vec = (Meta.Rotation == Up || Meta.Rotation == Down) ? RightV : -UpV;
		Off -= Vec * Scale;
	}
	if (Meta.yflip) {
		int Scale = (Meta.Rotation == Up || Meta.Rotation == Down) ? BlockSize.y() : BlockSize.x();
		auto Vec = (Meta.Rotation == Up || Meta.Rotation == Down) ? UpV : -RightV;
		Off -= Vec * Scale;
	}

	return Off;
}

PointType VisualBlockInterior::GetBasePosition(const BlockMetadata& Meta, const PointType& BlockSize) {
	return Meta.Pos - GetPositionDiff(Meta, BlockSize);
}

Eigen::Vector2f VisualBlockInterior::GetMarkedMean() const {
	using enum MyDirection::Direction;
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
			if (!MarkedBlocks[id]) continue;

			Eigen::Vector2f Diff = GetBasePosition(Meta, BlockSize).cast<float>() - Mean;
			Diff /= count + 1;
			Mean += Diff;
			count++;
		}
	}
	for (const auto& p : Paths) {
		if (p.IsFree()) continue;
		if (!p.HasMarked()) continue;

		auto Pos = p.Data.BoundingBox.Position.cast<float>() + (p.Data.BoundingBox.Size.cast<float>() / 2.0);

		Eigen::Vector2f Diff = Pos - Mean;
		Diff /= count + 1;
		Mean += Diff;
		count++;
	}
	if (count == 0) {
		return InvalidPointF;
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


	const Eigen::Vector2f floatMin = Area.Position;
	const Eigen::Vector2f floatMax = Area.Position + Area.Size;

	Eigen::Vector2f min = floatMin.cwiseMin(floatMax);
	Eigen::Vector2f max = floatMin.cwiseMax(floatMax);

	Eigen::Vector2i intMin = floatMin.array().ceil().cast<int>();
	Eigen::Vector2i intMax = floatMax.array().floor().cast<int>();
	Eigen::Vector2i intSize = (intMax - intMin).cwiseMax(Eigen::Vector2i::Zero());

	const MyRectI AreaI(intMin, intSize);

	bool hasAnyIntPoint = (min.array().ceil() <= (max.array().floor())).all();

	if (!hasAnyIntPoint)return;

	for (auto& p : Paths) {
		if (p.IsFree())continue;
		p.SetMarkedArea(AreaI);
	}
}

//Returnes if Modifyed
bool VisualBlockInterior::RotateMarked(bool CW) {
	auto Rotate = [](float angle) {
		return Eigen::Matrix2i{
			{(int)std::round(cos(angle)),-(int)std::round(sin(angle))},
			{(int)std::round(sin(angle)),(int)std::round(cos(angle))}
		};
		};
	auto Rotation = Rotate(CW ? (float)M_PI / 2.0f : (float)-M_PI / 2.0);

	const Eigen::Vector2f Mean = GetMarkedMean();

	Eigen::Vector2i pos = { int(std::round(Mean.x())),int(std::round(Mean.y())) };

	return ApplyToMarked(
		[pos, Rotation, CW](BlockMetadata& meta, const PointType& BlockSize) {

			//Importent to be before meta is modified
			const PointType Base = GetBasePosition(meta, BlockSize);

			//Update Rotateion
			if ((meta.xflip && !meta.yflip) || !meta.xflip && meta.yflip) {
				meta.Rotation = CW ? MyDirection::RotateCW(meta.Rotation) : MyDirection::RotateCCW(meta.Rotation);
			}
			else {
				meta.Rotation = CW ? MyDirection::RotateCCW(meta.Rotation) : MyDirection::RotateCW(meta.Rotation);
			}

			//Update Position
			PointType Off = Base - pos;
			Off = Rotation * Off;
			meta.Pos = pos + Off;
			meta.Pos += GetPositionDiff(meta, BlockSize);
		},
		[pos, CW](VisualPath& p) {
			p.Rotate(pos, CW);
		}
	);
}

//Returnes if Modifyed
bool VisualBlockInterior::FlipMarked(bool X) {

	static const constexpr auto FlipXMat = Eigen::Matrix2i{
		{-1,0},
		{0,1},
	};

	static const auto  FlipYMat = Eigen::Matrix2i{
		{1,0},
		{0,-1},
	};

	const Eigen::Vector2f Mean = GetMarkedMean();

	Eigen::Vector2i pos = { int(std::round(Mean.x())),int(std::round(Mean.y())) };

	const auto& FlipMat = X ? FlipXMat : FlipYMat;

	return ApplyToMarked(
		[pos, FlipMat, X](BlockMetadata& meta, const PointType& BlockSize) {

			//Importent to be before meta is modified
			const PointType Base = GetBasePosition(meta, BlockSize);

			if (X) {
				meta.xflip = !meta.xflip;
			}
			else {
				meta.yflip = !meta.yflip;
			}

			PointType Off = Base - pos;
			Off = FlipMat * Off;
			meta.Pos = pos + Off;
			meta.Pos += GetPositionDiff(meta, BlockSize);
		},
		[pos, X](VisualPath& p) {
			p.Flip(X ? pos.x() : pos.y(), X);
		}
	);
}

void VisualBlockInterior::MoveMarked(const PointType& Diff) {
	ApplyToMarked(
		[Diff](BlockMetadata& meta, const PointType& BlockSize) {meta.Pos += Diff; },
		[Diff](VisualPath& p) {p.Move(Diff); }
	);
}

//Returnes if Modifyed
bool VisualBlockInterior::DeleteMarked() {
	std::vector<CompressedBlockDataIndex> Indecies;

	bool HadMarkedBlocks = HasMarkedBlocks();

	if (HadMarkedBlocks) {
		unsigned int id = 0;
		size_t Deleted = 0;

		for (auto& [type, vec] : Blocks) {
			Indecies.push_back(type);
			id += vec.size();
		}

		//id contains highest id
		for (auto it = rbegin(Indecies); it != rend(Indecies); it++) {
			auto& vec = Blocks.at(*it);
			if (vec.size() == 0) continue;
			for (size_t i = vec.size() - 1;; i--) {
				assert(id != 0);//Would read irrelevant
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
		assert(Deleted != 0);
		MarkedBlocks.resize(MarkedBlocks.size() - Deleted, false);
		NumMarkedBlocks = std::numeric_limits<unsigned int>::max();
		DirtyBlocks = true;
	}

	bool FoundPath = false;
	for (size_t i = 0; i < Paths.size(); i++) {
		auto& p = Paths[i];
		if (p.IsFree()) continue;
		if (p.IsFullyMarked()) {
			p.Free(PathsFreeListHead);
			PathsFreeListHead = i;
			FoundPath = true;
		}
		else if (p.HasMarked()) {
			SplitPath(i).Free(PathsFreeListHead);
			PathsFreeListHead = i;
			FoundPath = true;
		}
	}

	DirtyBlocks |= HadMarkedBlocks;
	Dirty |= HadMarkedBlocks || FoundPath;
	if (HadMarkedBlocks || FoundPath) {
		ClearMarked();
	}
	return HadMarkedBlocks || FoundPath;
}

VisualBlockInterior::VisualBlockInterior(DataResourceManager* ResourceManager, Renderer* renderer)
	:ResourceManager(ResourceManager), renderer(renderer)
{
	PROFILE_FUNKTION;
}

void VisualBlockInterior::WriteBack(CompressedBlockData& Data) {
	Data.Paths.clear();

	Data.Paths.reserve(Paths.size());
	for (const auto& p : Paths) {
		if (p.IsFree()) continue;
		Data.Paths.emplace_back(std::move(p.Data));
	}
	PathsFreeListHead = VisualPath::FreeListEnd;
	Paths.clear();

	Data.blockExteriorData.ContainedBlocks.clear();
	for (const auto& [IndexContained, MetaVec] : Blocks) {
		Data.blockExteriorData.ContainedBlocks[ResourceManager->GetIdentifyer(IndexContained).value()] = std::move(MetaVec);
	}

	Highlited = 0;

	MarkedBlocks.clear();
	NumMarkedBlocks = 0;
	Blocks.clear();
}

void VisualBlockInterior::UpdateCurrentBlock() {
	PROFILE_FUNKTION;

	Destructing = true;
	Paths.clear();
	Blocks.clear();
	Destructing = false;

	Edges.Clear();
	EdgesMarked.Clear();
	EdgesUnmarked.Clear();
	SpecialPoints.Clear();
	Verts.Clear();
	ConflictPoints.Clear();
	FloatingEdges.Clear();
	FloatingSpecialPoints.Clear();
	FloatingVerts.Clear();
	StaticTextVerts.Clear();
	DynamicTextVerts.Clear();

	PinVerts.Clear();
	BlockVerts.Clear();
	SevenSegVerts.Clear();
	SixteenSegVerts.Clear();
	RoundedPinVerts.Clear();
	AndVerts.Clear();
	OrVerts.Clear();
	XOrVerts.Clear();
	MuxVerts.Clear();

#ifdef ShowBasePositionOfBlocks
	BasePositionVerts.Clear();
#endif

	PreviewData.clear();
	PreviewCached.reset();
	PreviewIsDirty = true;
	PreviewBoundingBoxIsDirty = true;
	Dirty = true;
	Destructing = false;
	DirtyBlocks = true;
	PathNeedsMerging = false;
	Moving = false;

	CachedBoundingBox = {};
	MouseCached = InvalidPoint;
	PreviewCachedBoundingBox = {};
	MouseCachedPreview = InvalidPoint;
	CachedBoundingBoxPreview = {};

	Highlited = 0;

	MarkedBlocks.clear();
	NumMarkedBlocks = 0;

	CollisionMap.clear();
	PathsFreeListHead = VisualPath::FreeListEnd;

	//PlacingBlock.clear();

	const auto& DataIndexOpt = ResourceManager->InteriorData;

	if (!DataIndexOpt) return;
	const auto& DataOpt = ResourceManager->GetCompressedBlockData(DataIndexOpt.value());
	if (!DataOpt) return;
	auto& Data = DataOpt.value();

	Paths.reserve(Data.Paths.size());
	for (const auto& pd : Data.Paths) {
		Paths.emplace_back(VisualPathData(pd, this));
	}

	std::map<CompressedBlockDataIndex, std::vector<BlockMetadata>> ToInser;
	for (const auto& [Ident, Blocks] : Data.blockExteriorData.ContainedBlocks) {
		for (const auto& Block : Blocks) {
			ToInser[ResourceManager->GetBlockIndex(Ident)].push_back(Block);
		}
	}

	AddBlockBatched(ToInser);
}

VisualBlockInterior::~VisualBlockInterior() noexcept {
	Destructing = true;
}

void VisualBlockInterior::UpdateVectsForVAOs(const MyRectF& ViewRect, const float& Zoom, const PointType& Mouse, bool AllowHover) {
	PROFILE_FUNKTION;
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
			if (p.IsFree()) continue;
			p.SetDontShowHover(Moving || HasPreview());
			if (AllowHover && p.Intercept(Mouse)) {
				p.SetHover(true);
			}
			else {
				p.SetHover(false);
			}
		}
	}

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
	ConflictPoints.Clear();

	MyRectI ViewRectI = MyRectI::FromCorners(
		ViewRect.Position.cast<int>() - PointType(1, 1),
		ViewRect.Position.cast<int>() + ViewRect.Size.cast<int>() + PointType(1, 1)
	);
	for (auto& p : Paths) {
		if (p.IsFree()) continue;
		p.ComputeAll(ViewRectI);
		if (p.IsFullyMarked()) continue;
		EdgesMarked.Append(p.getEdgesMarked());
		EdgesUnmarked.Append(p.getEdgesUnmarked());
		Verts.Append(p.getVerts());
		SpecialPoints.Append(p.getSpecialPoints());
		ConflictPoints.Append(p.getConflictPoints());
	}

	Edges.AppendOther(EdgesMarked, EdgesUnmarked);
}

void VisualBlockInterior::UpdateVectsForVAOsFloating(const MyRectF& ViewRect, const PointType& Mouse) {
	PROFILE_FUNKTION;

	if (!HasPreview()) {
		FloatingEdges.Clear();
		FloatingVerts.Clear();
		FloatingSpecialPoints.Clear();

		for (auto& p : Paths) {
			if (p.IsFree()) continue;
			if (!p.IsFullyMarked()) continue;
			FloatingEdges.Append(p.getEdgesMarked());
			FloatingVerts.Append(p.getVerts());
			FloatingSpecialPoints.Append(p.getSpecialPoints());
		}
		return;
	}

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

		FloatingEdges.Clear();
		FloatingVerts.Clear();
		FloatingSpecialPoints.Clear();

		if (!PreviewCached.has_value()) return;

		auto& Preview = PreviewCached.value();

		Preview.SetPreview(true);

		MyRectI ViewRectI = MyRectI::FromCorners(
			ViewRect.Position.cast<int>() - PointType(1, 1),
			ViewRect.Position.cast<int>() + ViewRect.Size.cast<int>() + PointType(1, 1)
		);

		Preview.ComputeAll(ViewRectI);
		FloatingEdges.Append(Preview.getEdgesMarked());
		FloatingVerts.Append(Preview.getVerts());
		FloatingSpecialPoints.Append(Preview.getSpecialPoints());
	}
}

#ifdef UseCollisionGrid
std::vector<PointType> VisualBlockInterior::GetBoxesFromLine(const PointType& A, const PointType& B) {
	PROFILE_FUNKTION;

	std::vector<PointType> ret;

	auto GetBoxCorner = [](const PointType& p) -> PointType {
		return {
			std::floor((float)p.x() / (float)BoxSize) * BoxSize,
			std::floor((float)p.y() / (float)BoxSize) * BoxSize
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
	PROFILE_FUNKTION;

	for (const auto& p : GetBoxesFromLine(Points[l.A].Pos, Points[l.B].Pos)) {
		CollisionMap[p].emplace_back(l, Id);
	}
}

void VisualBlockInterior::UnRegisterLine(const LineIndex& l, const std::vector<PointNode>& Points, const VisualPathData::VisualPathDataId& Id) {
	PROFILE_FUNKTION;

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

LineIndex VisualBlockInterior::GetLineWith(const PointType& p, const VisualPathData& pd) {
	PROFILE_FUNKTION;

	if (Destructing)return {};
	for (const auto& Box : GetBoxesFromLine(p, p)) {
		auto it = CollisionMap.find(Box);
		if (it == CollisionMap.end())continue;
		for (const auto& l : it->second) {
			if (l.PathId != pd.Id)continue;
			if (!pd.PointStrictelyOnLine(l.Line, p))continue;
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

bool VisualBlockInterior::TryAbsorb(VisualPathData& Path) {
	PROFILE_FUNKTION;


	bool MergedAtLeastOnes = false;
	std::optional<size_t> AbsorbIndex = std::nullopt;

	for (size_t i = 0; i < Paths.size(); i++) {
		if (Paths[i].IsFree()) continue;
		if (!Paths[i].TryAbsorb(Path)) continue;
		MergedAtLeastOnes = true;
		AbsorbIndex = i;
		break;
	}
	while (AbsorbIndex) {
		bool found = false;
		for (size_t i = 0; i < Paths.size(); i++) {
			if (Paths[i].IsFree()) continue;
			if (i == AbsorbIndex.value()) continue;
			if (!Paths[i].TryAbsorb(Paths[AbsorbIndex.value()].Data)) continue;
			Paths[AbsorbIndex.value()].Free(PathsFreeListHead);
			PathsFreeListHead = AbsorbIndex.value();
			AbsorbIndex = i;
			found = true;
			break;
		}
		if (!found) break;
	}

	if (MergedAtLeastOnes) {
		Dirty = true;
		PreviewIsDirty = true;
	}
	return MergedAtLeastOnes;
}

void VisualBlockInterior::AbsorbOrAdd(VisualPathData&& Data) {
	if (TryAbsorb(Data)) {
		return;
	}
	AddPath(std::move(Data));
}

//Be carfull, may invalidate Paths ref/ptr/it
VisualPath& VisualBlockInterior::SplitPath(size_t PathIndex) {
	assert(PathIndex < Paths.size());
	assert(!Paths[PathIndex].IsFree());
	assert(!Paths[PathIndex].IsFullyMarked());
	assert(Paths[PathIndex].HasMarked());
	auto [Marked, Unmarked] = Paths[PathIndex].Split();
	for (auto& CCMarked : Marked) {
		const VisualPath::PathIndex i = AddPath(VisualPathData(std::move(CCMarked), this));
		Paths[i].MarkAll();
	}
	for (auto& CCUnmarked : Unmarked) {
		AddPath(VisualPathData(std::move(CCUnmarked), this));
	}
	Dirty = true;
	return Paths[PathIndex];
}

void VisualBlockInterior::MergeAfterMove() {
	Moving = false;

	std::queue<size_t> PathsToMerge;
	for (size_t i = 0; i < Paths.size(); i++) {
		auto& p = Paths[i];
		if (p.IsFree()) continue;
		if (!p.NeedsMerging()) continue;
		PathsToMerge.push(i);
	}

	while (!PathsToMerge.empty()) {
		size_t curr = PathsToMerge.front();
		PathsToMerge.pop();
		bool Merged = false;
		for (size_t i = 0; i < Paths.size(); i++) {
			auto& p = Paths[i];
			if (p.IsFree()) continue;
			if (p.NeedsMerging()) continue;
			if (!p.TryAbsorb(Paths[curr].Data)) continue;
			Paths[curr].Free(PathsFreeListHead);
			PathsFreeListHead = curr;
			curr = i;
			Merged = true;
		}
		if (Merged) continue;
		Paths[curr].ClearNeedsMerging();
	}
}

bool VisualBlockInterior::HasMark(bool Preview) {
	return !GetEdgesMarked(Preview).Empty();
}

bool VisualBlockInterior::HasUnmarked(bool Preview) {
	return !GetEdgesUnmarked(Preview).Empty();
}


std::optional<VisualPathData > VisualBlockInterior::GeneratePreviewPath(const PointType& Mouse) {
	if (PreviewData.size() < 1) return std::nullopt;

	VisualPathData pd;

	if (PreviewData.size() == 1) {
		if (PreviewData[0] == Mouse)return std::nullopt;
		if (VisualPathData::PointsMakeStreightLine(PreviewData[0], Mouse)) {
			pd = VisualPathData(PreviewData[0], Mouse, this);
			return pd;
		}
		pd = VisualPathData(PreviewData[0], { PreviewData[0].x(),Mouse.y() }, this);
		pd.addTo(Mouse);
		return pd;
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
	return pd;
}

std::optional<VisualPathData> VisualBlockInterior::GeneratePreviewPath() {
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
	return pd;
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
	if (PathOpt && PathOpt.value().Intercept(mouse) != InvalidLineIndex) {
		PreviewData.emplace_back(mouse);
		return true;
	}
	PreviewData.emplace_back(mouse);
	for (const auto& p : Paths) {
		if (p.IsFree()) continue;
		if (p.Intercept(mouse))
			return true;
	}
	return false;
}

void VisualBlockInterior::EndDrag() {
	auto PathOpt = GeneratePreviewPath();
	PreviewData.clear();
	PreviewCached.reset();
	FloatingEdges.Clear();
	FloatingVerts.Clear();
	FloatingSpecialPoints.Clear();
	PreviewIsDirty = true;
	if (!PathOpt) {
		return;
	}
	Dirty = true;
	AbsorbOrAdd(std::move(PathOpt.value()));
}

void VisualBlockInterior::CancleDrag() {
	PreviewData.clear();
	PreviewCached = std::nullopt;
}

bool VisualBlockInterior::HasPreview() const {
	return !PreviewData.empty();
}

bool VisualBlockInterior::HasFloating() const {
	return !FloatingEdges.Empty() || HasPreview();
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
	PointType FlipOff = { Meta.xflip * BlockSize.x(),-int(Meta.yflip) * BlockSize.y() };
	if (Rotation == MyDirection::Left || Rotation == MyDirection::Right) {
		//TopLeft = Meta.Off + FlipOff;
		TopLeft = Meta.Pos + FlipOff + PointType{ Meta.xflip * (BlockSize.y() - BlockSize.x()), Meta.yflip * (BlockSize.y() - BlockSize.x()) };
		BottomLeft = TopLeft - PointType{ 0,BlockSize.x() }.cwiseProduct(Flip);
		TopRight = TopLeft + PointType{ BlockSize.y(),0 }.cwiseProduct(Flip);
		BottomRight = TopLeft + PointType{ BlockSize.y(), -BlockSize.x() }.cwiseProduct(Flip);
	}
	else {
		TopLeft = Meta.Pos + FlipOff;
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
		assert(false);

	case Right:
		switch (Rotation) {
		case Up: return TopRight + PointType{ Expoltion,-Pin.Offset }.cwiseProduct(Flip);
		case Right: return BottomRight + PointType{ -Pin.Offset,-Expoltion }.cwiseProduct(Flip);
		case Down: return BottomLeft + PointType{ -Expoltion,Pin.Offset }.cwiseProduct(Flip);
		case Left: return TopLeft + PointType{ Pin.Offset,Expoltion }.cwiseProduct(Flip);
		}
		assert(false);

	case Down:
		switch (Rotation) {
		case Up: return BottomLeft + PointType{ Pin.Offset,-Expoltion }.cwiseProduct(Flip);
		case Right: return TopLeft + PointType{ -Expoltion,-Pin.Offset }.cwiseProduct(Flip);
		case Down: return TopRight + PointType{ -Pin.Offset,Expoltion }.cwiseProduct(Flip);
		case Left: return BottomRight + PointType{ Expoltion,Pin.Offset }.cwiseProduct(Flip);
		}
		assert(false);

	case Left:
		switch (Rotation) {
		case Up: return TopLeft + PointType{ -Expoltion,-Pin.Offset }.cwiseProduct(Flip);
		case Right: return TopRight + PointType{ -Pin.Offset,Expoltion }.cwiseProduct(Flip);
		case Down: return BottomRight + PointType{ Expoltion,Pin.Offset }.cwiseProduct(Flip);
		case Left: return BottomLeft + PointType{ Pin.Offset,-Expoltion }.cwiseProduct(Flip);
		}
		assert(false);

	}
	assert(false);
	return {};
}

MyDirection::Direction VisualBlockInterior::GetPinRotation(const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin) {
	using enum MyDirection::Direction;
	MyDirection::Direction d;
	if (Meta.Rotation == Up) d = Pin.Rotation;
	else if (Meta.Rotation == Left) d = MyDirection::RotateCCW(Pin.Rotation);
	else if (Meta.Rotation == Down) d = MyDirection::RotateCW(MyDirection::RotateCW(Pin.Rotation));
	else if (Meta.Rotation == Right) d = MyDirection::RotateCW(Pin.Rotation);
	else d = Neutral;
	if (Meta.xflip) {
		d = MyDirection::FlipH(d);
	}
	if (Meta.yflip) {
		d = MyDirection::FlipV(d);
	}
	return d;
}

void VisualBlockInterior::ShowMultiplicity(const float& Zoom, const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin) {
	using namespace MyDirection;

	if (Zoom >= 0.01f) return;

	if (Pin.Multiplicity == 1) {
		return;
	}
	std::string Text;
	if (Pin.Multiplicity == CompressedBlockData::BlockExteriorData::Pin::VariableMultiplicity) {
		Text = "�";
	}
	else {
		Text = std::to_string(Pin.Multiplicity);
	}
	Point<float> Offset;

	float Scale = 0.4f;
	auto ext = RenderTextUtility::GetTextExtend(Text, false, false, Scale);

	float maxWidth = 0.42f;
	if (ext.Width > maxWidth) {
		Scale *= maxWidth / ext.Width;
		ext = RenderTextUtility::GetTextExtend(Text, false, false, Scale);
	}

	const MyDirection::Direction& d = GetPinRotation(Meta, Pin);

	Offset += d;//Up
	if (d == MyDirection::Left || d == MyDirection::Right) {
		Offset.x *= -1;
	}
	Offset *= 0.35f;

	const auto Pos = GetPinPosition(BlockSize, Meta, Pin, 1);
	RenderTextUtility::AddText(Text,
		Point<float>{(float)Pos.x() + Offset.x, (float)Pos.y() + Offset.y},
		StaticTextVerts,
		RenderTextUtility::TextPlacmentFlags::x_Center | RenderTextUtility::TextPlacmentFlags::y_Center,
		true, false, Scale,
		MyDirection::ToReadable(GetPinRotation(Meta, Pin)),
		ColourType{ 0.687f,0.933f,0.845f,1.0f });
	//Do final stuff
}

void VisualBlockInterior::ShowLable(const float& Zoom, const PointType& BlockSize, const BlockMetadata& Meta, const CompressedBlockData::BlockExteriorData::Pin& Pin) {
	using namespace MyDirection;

	if (Zoom >= 0.015f) return;

	Point<float> Offset;

	float Scale = 0.4f;

	auto ext = RenderTextUtility::GetTextExtend(Pin.Name, false, false, Scale);

	float Hightmul = Scale * RenderTextUtility::LineHeight;

	MyDirection::Direction d = GetPinRotation(Meta, Pin);
	switch (d) {
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

	const auto Pos = GetPinPosition(BlockSize, Meta, Pin, 1);
	RenderTextUtility::AddText(Pin.Name,
		Point<float>{(float)Pos.x() + Offset.x, (float)Pos.y() + Offset.y},
		StaticTextVerts,
		RenderTextUtility::TextPlacmentFlags::x_Right | RenderTextUtility::TextPlacmentFlags::y_Center,
		false, false, Scale,
		MyDirection::ToReadable(MyDirection::RotateCCW(GetPinRotation(Meta, Pin))),
		ColourType{ 1.0f,0.5f,0.0f,1.0f });
	//Do final stuff
};

void VisualBlockInterior::ShowBlockLabl(const PointType& BlockSize, const BlockMetadata& Meta, const std::string& Name) {
	Point<float> Off{ 0,0 };
	PointType TopLeft;
	Point<float> Flip = { 1.0f - 2.0f * Meta.xflip,1.0f - 2.0f * Meta.yflip };
	PointType FlipOff = { Meta.xflip * BlockSize.x(),-int(Meta.yflip) * BlockSize.y() };
	if (Meta.Rotation == MyDirection::Left || Meta.Rotation == MyDirection::Right) {
		Off += Point<float>{BlockSize.y() / 2.0f, -BlockSize.x() / 2.0f} *Flip;
		//TopLeft = Meta.Off + PointType{ -1,-1 }*int((BlockSize.y() - BlockSize.x()) / 2.0);
		//TopLeft = Meta.Off + FlipOff;
		TopLeft = Meta.Pos + FlipOff + PointType{ Meta.xflip * (BlockSize.y() - BlockSize.x()), Meta.yflip * (BlockSize.y() - BlockSize.x()) };
	}
	else {
		Off += Point<float>{BlockSize.x() / 2.0f, -BlockSize.y() / 2.0f} *Flip;
		TopLeft = Meta.Pos + FlipOff;
	}

	RenderTextUtility::AddText(Name,
		Point<float>{(float)TopLeft.x(), (float)TopLeft.y()} + Off,
		StaticTextVerts,
		RenderTextUtility::TextPlacmentFlags::x_Center | RenderTextUtility::TextPlacmentFlags::y_Center,
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
#ifdef ShowBasePositionOfBlocks
	BasePositionVerts.Clear();
#endif

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
		const auto& Name = ContainedExterior.blockExteriorData.Identifiyer.GetName();

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

#ifdef ShowBasePositionOfBlocks
			BasePositionVerts.Emplace(GetBasePosition(Meta, BlockSize).cast<float>() + Eigen::Vector2f(0.1, 0.1), GetBasePosition(Meta, BlockSize).cast<float>() - Eigen::Vector2f(0.1, 0.1), ColourType{ 1.0f,1.0f,0.0f,1.0f });
#endif

			const auto& SB = ResourceManager->GetSpecialBlockIndex();

			PointType TopLeft;
			PointType BottomRight;
			PointType Flip = { 1 - 2 * Meta.xflip,1 - 2 * Meta.yflip };
			PointType FlipOff = { Meta.xflip * BlockSize.x(),-int(Meta.yflip) * BlockSize.y() };
			if (Rotation == MyDirection::Left || Rotation == MyDirection::Right) {
				TopLeft = Meta.Pos + FlipOff + PointType{ Meta.xflip * (BlockSize.y() - BlockSize.x()), Meta.yflip * (BlockSize.y() - BlockSize.x()) };
				BottomRight = TopLeft + PointType{ BlockSize.y(), -BlockSize.x() }.cwiseProduct(Flip);
			}
			else {
				TopLeft = Meta.Pos + FlipOff;
				BottomRight = TopLeft + PointType{ BlockSize.x(), -BlockSize.y() }.cwiseProduct(Flip);
			}

			assert(MarkedBlocks.size() > id);
			ColourType Color{};
			if (MarkedBlocks[id]) {
				Color = ColourType{ 1.0f,0.0f,1.0f,1.0f };
			}
			else if (id == Highlited) {
				Color = ColourType{ 1.0f,1.0f,0.0f,1.0f };
			}

			if (IndexContained == SB.SevengSeg) {
				SevenSegVerts.Emplace(Meta, time(0) % 0x10, ColourType{ 0.78f,0.992f,0.0f,1.0f });
				BlockVerts.Emplace(id, TopLeft, BottomRight, ColourType{ 0.1f,0.1f,0.1f,1.0f }, Color);
			}
			else if (IndexContained == SB.SixteenSeg) {
				static std::array<int, 218> Translation = { 73,99,104,32,104,97,98,101,32,106,101,116,122,116,32,101,105,110,102,117,110,107,116,105,111,110,105,101,114,101,110,100,101,115,49,54,32,83,101,103,109,101,110,116,32,68,105,115,112,108,97,121,100,97,115,32,97,108,108,101,32,97,115,99,105,105,32,90,101,105,99,104,101,110,100,97,114,115,116,101,108,108,101,110,32,107,97,110,110,58,48,49,50,51,52,53,54,55,56,57,116,104,101,32,113,117,105,99,107,32,98,114,111,119,110,32,102,111,120,32,106,117,109,112,115,32,111,118,101,114,32,116,104,101,32,108,97,122,121,32,100,111,103,84,72,69,32,81,85,73,67,75,32,66,82,79,87,78,32,70,79,88,32,74,85,77,80,83,32,79,86,69,82,32,84,72,69,32,76,65,90,89,32,68,79,71,33,64,35,36,37,94,38,42,40,41,95,45,43,123,125,124,58,34,60,62,63,96,126,91,93,92,59,39,44,46,47,126, };
				SixteenSegVerts.Emplace(Meta, Translation[std::max(i - 5, 0)], ColourType{ 0.992f,0.43f,0.0f,1.0f });
				BlockVerts.Emplace(id, TopLeft, BottomRight, ColourType{ 0.1f,0.1f,0.1f,1.0f }, Color);
			}
			else if (IndexContained == SB.And || IndexContained == SB.Or || IndexContained == SB.XOr) {
				if (IndexContained == SB.And)AndVerts.Emplace(id, Meta, Color);
				else if (IndexContained == SB.Or)OrVerts.Emplace(id, Meta, Color);
				else if (IndexContained == SB.XOr)XOrVerts.Emplace(id, Meta, Color);

				for (const auto& Pin : InputPins) {
					RoundedPinVerts.Emplace(id, GetPinPosition(BlockSize, Meta, Pin, 1), ColourType{ 1.0f,0.0f,0.0f,0.0f });
				}
				for (const auto& Pin : OutputPins) {
					RoundedPinVerts.Emplace(id, GetPinPosition(BlockSize, Meta, Pin, 1), ColourType{ 0.0f,1.0f,0.0f,0.0f });
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
			}

			for (const auto& Pin : InputPins) {
				PinVerts.Emplace(id, GetPinPosition(BlockSize, Meta, Pin, 1), GetPinRotation(Meta, Pin), ColourType{ 0.7f,0.0f,0.0f,0.0f }, Color);
				ShowMultiplicity(Zoom, BlockSize, Meta, Pin);
				ShowLable(Zoom, BlockSize, Meta, Pin);
			}
			for (const auto& Pin : OutputPins) {
				PinVerts.Emplace(id, GetPinPosition(BlockSize, Meta, Pin, 1), GetPinRotation(Meta, Pin), ColourType{ 0.0f,0.7f,0.0f,0.0f }, Color);
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

#ifdef ShowBasePositionOfBlocks
BufferedVertexVec<PointFRGBVertex>& VisualBlockInterior::GetBasePotitionOfBlocksVerts() {
	return BasePositionVerts;
}
#endif

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
