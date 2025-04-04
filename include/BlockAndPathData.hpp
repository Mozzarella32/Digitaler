#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "MyRect.hpp"

struct TwoPointIVertex;
struct PointIVertex;
class VisualPath;
class CompressedPathData;
class VisualBlockInterior;

namespace std {
	template<>
	struct hash<Eigen::Vector2i> {
		std::size_t operator()(const Eigen::Vector2i& vec) const noexcept {
			std::size_t h1 = std::hash<int>()(vec.x());
			std::size_t h2 = std::hash<int>()(vec.y());
			return h1 ^ (h2 << 1);
		}
	};
}

using PointType = Eigen::Vector2i;
using PointIndex = unsigned int;
using LineIndexInPoint = unsigned char;
using ColourType = Eigen::Vector4f;
//using Line = std::pair<PointIndex, PointIndex>;
//using LineIndex = unsigned int;
//
//struct BlockExteriorData;
//
//struct CompressedBlockData {
//	std::string Name;
//	
//	struct Pin {
//		PointType Pos;//Unique
//		std::string Name;//Unique
//	};
//
//	PointType Size;
//
//	std::vector<Pin> InputPin;
//	std::vector<Pin> OutputPin;
//
//	//CompressedBlockData(std::string Name) {};
//	explicit CompressedBlockData(const BlockExteriorData& vbd) 
//	:
//		Name(vbd.Name), Size(vbd.Size)
//	{
//		InputPin.reserve(vbd.InputPin.size());
//		for (const auto& P : vbd.InputPin) {
//			InputPin.emplace_back(P.Pos, P.Name);
//		}
//		OutputPin.reserve(vbd.OutputPin.size());
//		for (const auto& P : vbd.OutputPin) {
//			OutputPin.emplace_back(P.Pos, P.Name);
//		}
//	}
//};

struct BlockMetadata {
	PointType Pos;
	MyDirection::Direction Rotation;
	bool xflip : 1 = 0;
	bool yflip : 1 = 0;
	int Transform() const {
		int Transform = 0;
		switch (Rotation) {
		case MyDirection::Up: Transform = 0; break;
		case MyDirection::Right: Transform = 1; break;
		case MyDirection::Down: Transform = 2; break;
		case MyDirection::Left: Transform = 3; break;
		case MyDirection::UpLeft:assert(false && "Invalid Direction for Pin");
		case MyDirection::UpRight:assert(false && "Invalid Direction for Pin");
		case MyDirection::DownLeft:assert(false && "Invalid Direction for Pin");
		case MyDirection::DownRight:assert(false && "Invalid Direction for Pin");
		case MyDirection::Neutral:assert(false && "Invalid Direction for Pin");
		default: assert(false && "Invalid Direction for Pin");
		}
		Transform |= xflip << 2;
		Transform |= yflip << 3;
		return Transform;
	}
};

using CompressedBlockDataIndex = size_t;

static const constexpr CompressedBlockDataIndex InvalidCompressedBlockDataIndex = -2;

struct CompressedBlockData {
	std::vector<CompressedPathData> Paths;

	struct BlockExteriorData {

		std::unordered_map<std::string, std::vector<BlockMetadata>> ContainedBlocks;

		//std::string Package;
		//std::filesystem::path PathInPackage;
		std::string Name;
		ColourType ColorRGB;
		ColourType TextColorRGB;
		/*
		Special Names:
		"Seven Segment Display"

		*/

		struct Pin {
			static const constexpr unsigned int VariableMultiplicity = (unsigned int)-1;

			MyDirection::Direction Rotation;
			unsigned int Offset;
			unsigned int Multiplicity = 1;
			std::string Name;
		};

		PointType Size;

		std::vector<Pin> InputPin;
		std::vector<Pin> OutputPin;
	} blockExteriorData;
};

//struct BlockExteriorData {
//	std::string Name;
//
//	struct Pin {
//		PointType Pos;//Unique Relative
//		std::string Name;//Unique
		//MyDirection::Direction Direction;//ForDisplay
//	};
//
//	PointType Size;
//
//	std::vector<Pin> InputPin;
//	std::vector<Pin> OutputPin;
//
//	//RelativeTo 0,0 wich is the top left corner
//	std::optional<MyDirection::Direction> GetPinDirection(const PointType& PinPos) {
//		const auto& Corner1 = PointType{0,0};
//		const auto& Corner2 = Size;
//
//		bool XInside = Corner1.x() < PinPos.x() && PinPos.x() < Corner2.x();
//		bool YInside = Corner1.y() < PinPos.y() && PinPos.y() < Corner2.y();
//
//		//false == false has to be on some corner field
//		//true == true is inside
//		if (XInside == YInside) return std::nullopt;
//		if (XInside) {
//			if (PinPos.y() == Corner1.y() - 1) {
//				return MyDirection::Up;
//			}
//			else if (PinPos.y() == Corner2.y() + 1) {
//				return MyDirection::Down;
//			}
//		}
//		else {
//			if (PinPos.x() == Corner1.x() - 1) {
//				return MyDirection::Right;
//			}
//			else if (PinPos.x() == Corner2.x() + 1) {
//				return MyDirection::Left;
//			}
//		}
//		return std::nullopt;
//	}
//
//	//Shall be moved To VisualBlockData becaous it has positions
//	/*bool AddPin(const Pin& Pin, bool IsInput) {
//		for (const auto& P : InputPin) {
//			if (P.Pos == Pin.Pos) {
//				return false;
//			}
//		}
//		if (IsInput) {
//			for (const auto& P : InputPin) {
//				if (P.Name == Pin.Name) return false;
//			}
//		}
//		for (const auto& P : OutputPin) {
//			if (P.Pos == Pin.Pos) {
//				return false;
//			}
//		}
//		if (!IsInput) {
//			for (const auto& P : OutputPin) {
//				if (P.Name == Pin.Name) return false;
//			}
//		}
//
//		if (!GetPinDirection(Pin.Pos).has_value()) return false;
//
//
//		if (IsInput) {
//			InputPin.emplace_back(Pin);
//		}
//		else {
//			OutputPin.emplace_back(Pin);
//		}
//	}*/
//
//	/*BlockExteriorData(const std::string& Name, const PointType& Size)
//	:Name(Name), Size(Size){};*/
//
//	//Wrong Level
//	explicit BlockExteriorData(const CompressedBlockData& cbd)
//		:Name(cbd.Name),
//		Size(cbd.Size)
//	{
//		InputPin.reserve(cbd.InputPin.size());
//		for (const auto& P : cbd.InputPin) {
//			auto DirOpt = GetPinDirection(P.Pos);
//			if (!DirOpt.has_value()) {
//				assert(false);
//				continue;
//			}
//			InputPin.emplace_back(P.Pos, P.Name,DirOpt.value());
//		}
//		OutputPin.reserve(cbd.OutputPin.size());
//		for (const auto& P : cbd.OutputPin) {
//			auto DirOpt = GetPinDirection(P.Pos);
//			if (!DirOpt.has_value()) {
//				assert(false);
//				continue;
//			}
//			OutputPin.emplace_back(P.Pos, P.Name, DirOpt.value());
//		}
//	}
//};

extern const PointIndex InvalidPointIndex;
extern const PointIndex FreePointIndex;
extern const PointIndex ReservedPointIndex;
extern const PointIndex FreeListEndPointIndex;

extern const PointType InvalidPoint;


struct PointNode {
	Eigen::Vector2i Pos;
	std::array<PointIndex, 4> Connections;

	VisualBlockInterior* Block = nullptr;

	/*auto GetConnections() const noexcept {
		return Connections | std::views::filter([](const PointIndex& p) {
			return p != InvalidPointIndex;
			});
	}*/

	PointNode(const PointIndex& next) noexcept
		:Pos{ InvalidPoint },
		Connections{ FreePointIndex,next,InvalidPointIndex,InvalidPointIndex } {
	}

	PointNode(const PointType& p) noexcept
		:Pos{ p },
		Connections{ InvalidPointIndex,InvalidPointIndex,InvalidPointIndex,InvalidPointIndex } {
	}

	LineIndexInPoint ConnectionCount() const noexcept;

	//Returnes the Next Free
	PointIndex Init(const PointType& p);

	//Set this head afterwareds
	void Free(const PointIndex& head);

	bool IsFree() const;

	void AddConnection(const PointIndex& p);

	void AddConnectionReserved(const PointIndex& p);

	void AddReserved();

	void RemoveConnection(const PointIndex& p);
};

struct LineIndex {
	PointIndex A;
	PointIndex B;
	/*LineIndexInPoint ConnectionIndex;

	const PointIndex& GetOhter(const std::vector<PointNode>& Points) const {
		return Points[Point].Connections[ConnectionIndex];
	}*/

	bool operator==(const LineIndex& other) const {
		return other.A == A && other.B == B
			|| other.A == B && other.B == A;
	}

	bool operator!=(const LineIndex& other) const {
		return !(*this == other);
	}
};

extern const LineIndex InvalidLineIndex;

struct VisualPathData {

	friend VisualPath;
	friend CompressedPathData;
	friend VisualBlockInterior;

	using VisualPathDataId = unsigned int;

	static constexpr const VisualPathDataId InvalidId = VisualPathDataId(-1);

private:
	//std::vector<PointType> Points;
	////low high InUse
	//// 000 0 Rejoining
	//// 001 1
	//// 010 2
	//// 011 3
	//// 100 4
	//// 101 Will be Decremented
	//// 110 Will be Decremented
	//// 111 Deleted
	////Todo add a waiting for reconect state so that certain points dont get moved away

	//enum ConnectionCountMeaning {
	//	////FehlerOverflow = 5,
	//	Rejoining = 0,
	//	Deleted = 7
	//};

	////std::vector<bool> PointConectionAndInvalidCount;
	//std::vector<uint64_t> PCAIC;//Each holds 21	3 bit values
	//size_t PCAICSize = 0;//In count of 3 bit values

	//void PopPCAIC();

	//void PushPCAIC(unsigned char val);

	//void ErasePCAIC(size_t i);

	//unsigned char GetPCAIC(size_t i) const;

	//void SetPCAIC(size_t i, unsigned char val);

	//std::vector<Line> Lines;

	//std::vector<LineIndex> LinesToDelete;
	//std::vector<PointIndex> PointsToDelete;

	PointIndex Head = FreeListEndPointIndex;
	std::vector<PointNode> Points;

	PointIndex AddPoint(const PointType& p);

	void AddLine(const PointIndex& a, const PointIndex& b);

	void AddLineReservedUnreserved(const PointIndex& a, const PointIndex& b);

	void AddLineReservedReserved(const PointIndex& a, const PointIndex& b);

	void AddLineCorrectReserve(const PointIndex& a, const PointIndex& b, bool aHasReserved, bool bHasReserved);

	/*void RemovePoint(const PointIndex& p) {
		assert(p < Points.size());
		assert(!Points[p].IsFree());
		Points[p].Free(Head);
		Head = p;
	}*/

public:
	MyRectI BoundingBox;
private:

	//Is for implementation of Incr/Decr use them instead
	//void ManipulateConnectionCount(const PointIndex& i, const int& change);

	//void IncrPointConnectionCount(const PointIndex& i, bool allowNewConnect);

	//void DecrPointConnectionCount(const PointIndex& i, bool allowRejoining);

	//int GetConnectionCount(const PointIndex& i) const;
	//int GetRealConnectionCount(const PointIndex& i)const;

	//Returnes index of lower point if exists
	LineIndex LineExists(const PointIndex& a, const PointIndex& b) const;
	PointIndex PointExists(const PointType& p) const;

	//bool PointIsDeleted(const PointIndex& p) const;
	//bool PointIsRejoining(const PointIndex& p) const;

	//bool LineIsDeleted(const LineIndex& l) const;

	//std::vector<LineIndex> LinesWithPoint(const PointIndex& p) const;

public:
	static bool PointStrictelyOnLine(const PointType& a, const PointType& b, const PointType& p);
	static bool PointsMakeStreightLine(const PointType& a, const PointType& b);

private:
	bool PointStrictelyOnLine(const LineIndex& l, const PointType& p) const;
	bool PointAllignedWithLine(const LineIndex& l, const PointType& p) const;
	bool LineAllignedWithLine(const LineIndex& l, const PointType& a, const PointType& b)const;

	PointIndex PointIsEndOfLine(const LineIndex& l, const PointType& p) const;

	bool LineIsHorizontal(const LineIndex& l) const;
	static bool LineIsHorizontal(const PointType& a, const PointType& b);

	PointType GetProjectionOnLine(const LineIndex& l, const PointType& p) const;

	PointIndex GetNearFromOutside(const LineIndex& l, const PointType& p) const;
	PointIndex GetFarFromOutside(const LineIndex& l, const PointType& p) const;

	void AddLineIntegrateInsides(const PointIndex& a, const PointIndex& b, bool aHasReserved, bool bHasReserved);

	//returnes index of created line or first of p or the insertet if p was deleted
	LineIndex AddLineRemoveIfUnnecesary(const PointIndex& aIndex, const PointType& b, bool aHasReserved);
	PointIndex AddLine(const PointIndex& aIndex, const PointType& p, bool aHasReserved);

	//returnes LineIndex
	//LineIndex AddLine(const PointIndex& aIndex, const PointIndex& bIndex);

	//returnes the index of the moved end
	PointIndex MoveEndPointAlongItsOnlyLine(const LineIndex& l, bool first, const PointType& newPos);

	void RemovePoint(const PointIndex& p);

	//Does not decreas Connection out!!! You are Responsible to decreas it just before the next increas
	void RemoveLineBetweenNoDelete(const LineIndex& l);
	std::pair<PointIndex, PointIndex> RemoveLineBetween(const LineIndex& l);

	//Returens index of non deletet for convinience
	PointIndex RemoveLineBetweenDeleteIndex(const LineIndex& l, const PointIndex& End);

	PointIndex BreakUpLineAndInsert(const LineIndex& l, const PointType& contained);

	//For removals where it has to be a O(n) operation
	void RecalculateBoundingBox();

	//void Cleanup();

	struct IsValidCaller {
		const VisualPathData* Data;
		IsValidCaller(const VisualPathData* Data) :Data(Data) {
			Data->IsValidPd();
		}
		~IsValidCaller() {
			Data->IsValidPd();
		}
	};

	bool IsValidPd() const;

	/*struct SceduleCleanup {
		PathData* Data;
		SceduleCleanup(PathData* Data) :Data(Data) {}
		~SceduleCleanup();
	};*/

	//Returnes first of p or the insertet if p was deleted
	LineIndex StreightLineMiddelRemove(const PointIndex& p);



private:
	VisualBlockInterior* Block = nullptr;
	static VisualPathDataId KlassInstanceCounter;
	VisualPathDataId Id;
public:

	//Is unusable until reassigned(every funktion fails assert)
	VisualPathData(VisualBlockInterior* Block = nullptr) : Block(Block), Id(KlassInstanceCounter++) {}
	VisualPathData(const PointType& p1, const PointType& p2, VisualBlockInterior* Block);
	VisualPathData(const CompressedPathData& pd, VisualBlockInterior* Block);
	VisualPathData(VisualPathData&& other) noexcept
		:Points(std::move(other.Points)),
		Head(std::exchange(other.Head, FreeListEndPointIndex)),
		BoundingBox(std::exchange(other.BoundingBox, {})),
		Block(std::exchange(other.Block, nullptr)),
		Id(std::exchange(other.Id, InvalidId)),
		LastAddedLine(std::exchange(other.LastAddedLine, InvalidLineIndex)) {
	}

	VisualPathData& operator=(VisualPathData&& other) noexcept {
#ifdef UseCollisionGrid
		Clear();
#endif
		PROFILE_FUNKTION;

		Points = std::move(other.Points);
		Head = std::exchange(other.Head, FreeListEndPointIndex);
		BoundingBox = std::exchange(other.BoundingBox, {});
		Block = std::exchange(other.Block, nullptr);
		LastAddedLine = std::exchange(other.LastAddedLine, InvalidLineIndex);
		Id = std::exchange(other.Id, InvalidId);
		return *this;
	}

#ifdef UseCollisionGrid
	~VisualPathData();

	void Clear() noexcept;
#endif

	//index of intesepting line
	LineIndex Intercept(const PointType& pos) const;

private:

	void RotateAround(const PointType& pos, double angle);

public:

	//Clockwise
	void RotateAroundCW(const PointType& pos);
	//Counter Clockwise
	void RotateAroundCCW(const PointType& pos);
	//Halfe Way
	void RotateAroundHW(const PointType& pos);

private:
	//Todo set
	LineIndex LastAddedLine = InvalidLineIndex;
	void SetLastAdded(const PointIndex& pIndex);

	void SetLastAdded(const LineIndex& pIndex);

public:
	//addToEnd
	bool addTo(const PointType& p);
private:
	bool addTo(const LineIndex& lineIndex, const PointType& p);
public:
	//bool addTo(const LineIndex& lineIndex, const PointType& p);

	bool AbsorbIfIntercept(VisualPathData& Other);

private:
	std::string PointConnectionToString(const PointIndex& p) const;
	//std::string PointConnectionToString(const LineIndex& l) const;
public:

	std::string toHumanReadable() const;
};

class CompressedPathData {

public:

	friend VisualPathData;

	using LineIndex = unsigned int;
	using Line = std::pair<PointIndex, PointIndex>;

public:
	//MyRectI BoundingBox;

public:
	std::vector<PointType> Points;
	std::vector<Line> Lines;
	::LineIndex LastAddedLine;
public:

	//CompressedPathData() = default;

	CompressedPathData(const VisualPathData& pd);

	CompressedPathData(std::vector<PointType>&& Points,
		std::vector<Line>&& Lines,
		::LineIndex&& LastAddedLine)
		:Points(std::move(Points)),
		Lines(std::move(Lines)),
		LastAddedLine(LastAddedLine) {
	}

	std::string toHumanReadable() const;

	/*std::ofstream operator<<(std::ofstream& of) const {
		SerializedWrite(of,)
		of << "0.0.0\n";
		of << BoundingBox.Position.x() << BoundingBox.Position.y() <<
		of << Points.size();
		for (const auto& p : Points) {
			of << p.x() << p.y();
		}
		of << Lines.size();
		for (const auto& l : Lines) {
			of << l.first << l.second;
		}
	}

	std::ifstream operator>>(std::)*/
};