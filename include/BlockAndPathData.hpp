#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "MyRect.hpp"

struct TwoPointIRGBRHGHBHVertex;
struct PointIRGBVertex;
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


namespace std {
	template<>
	struct hash<std::pair<Eigen::Vector2i, Eigen::Vector2i>> {
		std::size_t operator()(const std::pair<Eigen::Vector2i, Eigen::Vector2i>& Pair) const noexcept {
			std::size_t h1 = std::hash<Eigen::Vector2i>()(Pair.first);
			std::size_t h2 = std::hash<Eigen::Vector2i>()(Pair.second);
			return h1 ^ (h2 << 1);
		}
	};
}

using PointType = Eigen::Vector2i;
using PointIndex = unsigned int;
using LineIndexInPoint = unsigned char;
using ColourType = Eigen::Vector4f;

struct BlockMetadata {
	PointType Pos;
	MyDirection::Direction Rotation;
	bool xflip : 1 = 0;
	bool yflip : 1 = 0;
	unsigned int Transform() const;
};

using CompressedBlockDataIndex = ssize_t;

static const constexpr CompressedBlockDataIndex InvalidCompressedBlockDataIndex = -2;

class BlockIdentifiyer {
	std::string Package;
	std::vector<std::string> Dirs;
	std::string Name;

	std::string Combined;

	void UpdateCombined();

public:
	BlockIdentifiyer(const std::string& Package, const std::vector<std::string> Dirs, const std::string Name);

	//PackageDir is used to relativize DirsAndName
	BlockIdentifiyer(const std::filesystem::path& PackageDir, const std::filesystem::path& DirsAndName);

	bool operator==(const BlockIdentifiyer& Other) const noexcept;

	//Package and Name are required
	static std::optional<BlockIdentifiyer> Parse(const std::string& str);

	//Package and Name are required
	static BlockIdentifiyer ParsePredefined(const std::string_view sv);

	const std::string& GetPackage() const;
	const std::vector<std::string>& GetDirs() const;
	const std::string& GetName() const;

	//Package:Dir1:Dir2...:Name
	const std::string& GetCombined() const;

	//Package/Dir1/Dir2/Name
	std::filesystem::path GetPath() const;

public:

	static const BlockIdentifiyer CreateNewBlockIdent;
};

namespace std {
	template<>
	struct hash<BlockIdentifiyer> {
		std::size_t operator()(const BlockIdentifiyer& Ident) const noexcept {
			return std::hash<std::string>{}(Ident.GetCombined());
		}
	};
}

struct CompressedBlockData {
	std::vector<CompressedPathData> Paths;

	struct BlockExteriorData {

		std::unordered_map<BlockIdentifiyer, std::vector<BlockMetadata>> ContainedBlocks;

		ColourType ColorRGB;
		ColourType TextColorRGB;

		BlockIdentifiyer Identifiyer;
		bool ShowIdentifiyer = true;

		struct Pin {
			static const constexpr unsigned int VariableMultiplicity = (unsigned int)-1;

			MyDirection::Direction Rotation = MyDirection::Up;
			int Offset = 0;
			unsigned int Multiplicity = 1;
			std::string Name;
			bool Round = false;
		};

		PointType Size = {1, 1};

		std::vector<Pin> InputPin;
		std::vector<Pin> OutputPin;
	} blockExteriorData;
};


extern const PointIndex InvalidPointIndex;
extern const PointIndex FreePointIndex;
extern const PointIndex ReservedPointIndex;
extern const PointIndex FreeListEndPointIndex;

extern const PointType InvalidPoint;
extern const Eigen::Vector2f InvalidPointF;
extern const int InvalidCoord;
extern const MyRectI FullRectI;
extern const MyRectF FullRectF;

struct PointNode {
	Eigen::Vector2i Pos;
	std::array<PointIndex, 4> Connections;

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

	void AddConnection(const PointIndex& p, bool allowSelf = false);

	void AddConnectionReserved(const PointIndex& p);

	void AddReserved();

	void RemoveConnection(const PointIndex& p);
};

struct LineIndex {
	PointIndex A;
	PointIndex B;

	bool operator==(const LineIndex& other) const {
		return (other.A == A && other.B == B)
			|| (other.A == B && other.B == A);
	}

	bool operator!=(const LineIndex& other) const {
		return !(*this == other);
	}
};

namespace std {
	template<>
	struct hash<LineIndex> {
		size_t operator()(const LineIndex& line) const {
			size_t hashA = std::hash<PointIndex>()(line.A);
			size_t hashB = std::hash<PointIndex>()(line.B);

			return std::max(hashA, hashB) ^ (std::min(hashA, hashB) << 1);
		}
	};
}



extern const LineIndex InvalidLineIndex;

struct VisualPathData {

	friend VisualPath;
	friend CompressedPathData;
	friend VisualBlockInterior;

	using VisualPathDataId = size_t;

	static constexpr const VisualPathDataId InvalidId = VisualPathDataId(-1);

private:

	PointIndex Head = FreeListEndPointIndex;
	std::vector<PointNode> Points;

	PointIndex AddPoint(const PointType& p);

	void AddLine(const PointIndex& a, const PointIndex& b, bool allowSame = false);

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
	size_t LineCount;
private:


	//Returnes index of lower point if exists
	LineIndex LineExists(const PointIndex& a, const PointIndex& b) const;
	PointIndex PointExists(const PointType& p) const;

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

	//Use carefully only when you want to destroy connectedness!!!
	void RemoveLineBetweenDeleteLonlyPoints(const LineIndex& l);

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

	//Returnes first of p or the insertet if p was deleted
	LineIndex StreightLineMiddelRemove(const PointIndex& p);

	//Remove Middlepoints
	void Sanitize();

private:
	VisualBlockInterior* Block = nullptr;
	static VisualPathDataId KlassInstanceCounter;
	VisualPathDataId Id;
public:

	//Is unusable until reassigned(every funktion fails assert)
	VisualPathData(VisualBlockInterior* Block = nullptr) : LineCount(0), Block(Block), Id(KlassInstanceCounter++) {}
	VisualPathData(const PointType& p1, const PointType& p2, VisualBlockInterior* Block);
	VisualPathData(const CompressedPathData& pd, VisualBlockInterior* Block, bool AllowSinglePoint = false);
	VisualPathData(VisualPathData&& other) noexcept;
	VisualPathData& operator=(VisualPathData&& other) noexcept;

#ifdef UseCollisionGrid
	~VisualPathData();

	void Clear() noexcept;
#endif

	//index of intesepting line
	LineIndex Intercept(const PointType& pos) const;

	VisualPathDataId GetId() const;

private:

	void RotateAround(const PointType& pos, double angle);

public:

	void RotateAround(const PointType& pos, bool CW);
	
	void Flip(const int& pos, bool X);
	
	void Move(const PointType& Off);

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

	//Returnes marked connected components(other than the one this is assigned to) and other coneccted components(unmarked)
	std::pair<std::vector<CompressedPathData>, std::vector<CompressedPathData>> Split(const std::unordered_set<LineIndex>& ToBecome);

private:
	std::string PointConnectionToString(const PointIndex& p) const;
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
	bool NeedsToBeSanitized = false;
public:

	//CompressedPathData() = default;

	CompressedPathData(const VisualPathData& pd);

	CompressedPathData(std::vector<PointType>&& Points,
		std::vector<Line>&& Lines,
		CompressedPathData::LineIndex&& LastAddedLine)
		:Points(std::move(Points)),
		Lines(std::move(Lines)),
		LastAddedLine(LastAddedLine) {
	}

	CompressedPathData(std::unordered_set<std::pair<PointType, PointType>>&& Lines, bool NeedsToBeSanitized);

	std::string toHumanReadable() const;
};
