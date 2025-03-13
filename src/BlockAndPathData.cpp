#include "pch.hpp"
#include "BlockAndPathData.hpp"

#include "OpenGlDefines.hpp"

#include "Block.hpp"

VisualPathData::VisualPathDataId VisualPathData::KlassInstanceCounter = 0;

const PointIndex InvalidPointIndex = PointIndex(-1);
//const LineIndex VisualPathData::InvalidLineIndex = LineIndex(-1);
const PointIndex FreePointIndex = PointIndex(-2);
const PointIndex ReservedPointIndex = PointIndex(-3);
const PointIndex FreeListEndPointIndex = PointIndex(-4);

const PointType InvalidPoint = PointType(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

const LineIndex InvalidLineIndex = LineIndex{ InvalidPointIndex,InvalidPointIndex };

//void VisualPathData::PopPCAIC() {
//	assert(PCAICSize > 0);
//	assert(!PCAIC.empty());
//	SetPCAIC(PCAICSize - 1, 0);
//	PCAICSize--;
//	if (PCAICSize % 21 == 0) {
//		PCAIC.pop_back();
//	}
//}

//void VisualPathData::PushPCAIC(unsigned char val) {
//	assert(val < 8);
//	size_t index = PCAICSize / 21;
//	size_t bitPos = (PCAICSize % 21) * 3;
//
//	if (bitPos == 0) {
//		PCAIC.push_back((uint16_t)val);
//	}
//	else {
//		PCAIC[index] |= (uint16_t)val << bitPos;
//	}
//	PCAICSize++;
//}

//void VisualPathData::ErasePCAIC(size_t i) {
//	assert(i < PCAICSize);
//
//	size_t blockCount = i / 21;
//	size_t bitPos = (i % 21) * 3;
//
//	for (size_t j = i; j < PCAICSize - 1; j++) {
//		SetPCAIC(j, GetPCAIC(j + 1));
//	}
//
//	PopPCAIC();
//}

//unsigned char VisualPathData::GetPCAIC(size_t i) const {
//	assert(i < PCAICSize);
//	size_t index = i / 21;
//	size_t bitPos = (i % 21) * 3;
//	return (unsigned char)((PCAIC[index] >> bitPos) & 0b111);
//}

//void VisualPathData::SetPCAIC(size_t i, unsigned char val) {
//	assert(val < 8);
//
//	size_t index = i / 21;
//	size_t bitPos = (i % 21) * 3;
//
//
//	PCAIC[index] = ((uint64_t)val << bitPos) | (PCAIC[index] & ~((uint64_t)0b111 << bitPos));
//}

//void VisualPathData::IncrPointConnectionCount(const PointIndex& i, bool allowNewConnect) {
//#ifdef _DEBUG 
//	IsValidCaller __vc(this);
//#endif
//	assert(!PointIsDeleted(i));
//
//	int Count = GetPCAIC(i);
//
//	assert(allowNewConnect || Count != Rejoining);
//	assert(Count != Deleted);
//
//	if (Count == Rejoining) {
//		assert(allowNewConnect);
//		Count = 1;
//	}
//	else {
//		Count += 1;
//	}
//
//	assert(Count != Rejoining);
//	assert(Count != Deleted);
//
//	SetPCAIC(i, Count);
//
//}

//void VisualPathData::DecrPointConnectionCount(const PointIndex& i, bool allowRejoining) {
//#ifdef _DEBUG 
//	IsValidCaller __vc(this);
//#endif
//	assert(!PointIsDeleted(i));
//
//	int Count = GetPCAIC(i);
//
//	assert(Count != Rejoining);
//	assert(Count != Deleted);
//
//	Count -= 1;
//
//	assert(Count != Rejoining || !allowRejoining);
//	assert(Count != Deleted);
//
//	SetPCAIC(i, Count);
//}

//int VisualPathData::GetConnectionCount(const PointIndex& i) const {
//#ifdef _DEBUG
//	IsValidCaller __vc(this);
//#endif
//	assert(!PointIsDeleted(i));
//
//	int ret = GetPCAIC(i);
//
//	assert(ret != Deleted);
//
//	return ret;
//}

PointIndex VisualPathData::AddPoint(const PointType& p) {
	BoundingBox.GrowToInclude(p);

	if (Head == FreeListEndPointIndex) {
		Points.emplace_back(p);
		return Points.size() - 1;
	}

	PointIndex OldHead = Head;
	Head = Points[OldHead].Init(p);
	return OldHead;
}

void VisualPathData::AddLine(const PointIndex& a, const PointIndex& b) {
	assert(a != b);
	assert(a < Points.size());
	assert(b < Points.size());
	assert(!Points[a].IsFree());
	assert(!Points[b].IsFree());
	assert(Block != nullptr);
	Points[a].AddConnection(b);
	Points[b].AddConnection(a);
#ifdef UseCollisionGrid
	Block->RegisterLine(LineIndex{ a,b }, Points,Id);
#endif
}

void VisualPathData::AddLineReservedUnreserved(const PointIndex& a, const PointIndex& b) {
	assert(a != b);
	assert(a < Points.size());
	assert(b < Points.size());
	assert(!Points[a].IsFree());
	assert(!Points[b].IsFree());
	assert(Block != nullptr);
	Points[a].AddConnectionReserved(b);
	Points[b].AddConnection(a);
#ifdef UseCollisionGrid
	Block->RegisterLine(LineIndex{ a,b }, Points, Id);
#endif
}

void VisualPathData::AddLineReservedReserved(const PointIndex& a, const PointIndex& b) {
	assert(a != b);
	assert(a < Points.size());
	assert(b < Points.size());
	assert(!Points[a].IsFree());
	assert(!Points[b].IsFree());
	assert(Block != nullptr);
	Points[a].AddConnectionReserved(b);
	Points[a].AddConnectionReserved(b);
#ifdef UseCollisionGrid
	Block->RegisterLine(LineIndex{ a,b }, Points, Id);
#endif
}

void VisualPathData::AddLineCorrectReserve(const PointIndex& a, const PointIndex& b, bool aHasReserved, bool bHasReserved) {
	if (aHasReserved) {
		if (bHasReserved) {
			AddLineReservedReserved(a, b);
		}
		else {
			AddLineReservedUnreserved(a, b);
		}
	}
	else {
		if (bHasReserved) {
			AddLineReservedUnreserved(b, a);
		}
		else {
			AddLine(a, b);
		}
	}
}

LineIndex VisualPathData::LineExists(const PointIndex& a, const PointIndex& b) const {
#ifdef _DEBUG 
	IsValidCaller __vc(this);
#endif
	assert(a < Points.size());
	assert(b < Points.size());
	//assert(!PointIsDeleted(a));
	//assert(!PointIsDeleted(b));
	assert(a != b);
	assert(!Points[a].IsFree());
	assert(!Points[b].IsFree());

	for (LineIndexInPoint i = 0; i < Points[a].ConnectionCount(); i++) {
		if (Points[a].Connections[i] == b) {
			return LineIndex{ a,Points[a].Connections[i]};
		}
	}
	return InvalidLineIndex;

	//Todo Modify to use index

	/*auto it = Lines.begin();
while (it != Lines.end()) {
	it = std::find_if(it, Lines.end(),
		[&](const auto& line) {
			return ((line.first == a && line.second == b)
				|| (line.second == a && line.first == b));
		});
	if (it != Lines.end() && LineIsDeleted(std::distance(Lines.begin(), it)))it++;
	else break;
}

if (it != Lines.end())return std::distance(Lines.begin(), it);
return InvalidLineIndex;*/
}

PointIndex VisualPathData::PointExists(const PointType& p) const {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(p != InvalidPoint);

	for (size_t i = 0; i < Points.size(); i++) {
		if (Points[i].IsFree()) continue;
		if (Points[i].Pos == p)return i;
	}

	return InvalidPointIndex;
}

//bool VisualPathData::PointIsDeleted(const PointIndex& p) const {
//	//#ifdef _DEBUG IsValidCaller __vc(this); #endif
//	assert(p < Points.size());
//
//	int ret = GetPCAIC(p);
//
//	return ret == Deleted;
//}

//bool VisualPathData::PointIsRejoining(const PointIndex& p) const {
//#ifdef _DEBUG
//	IsValidCaller __vc(this);
//#endif
//
//	int ret = GetPCAIC(p);
//
//	assert(ret != Deleted);
//
//	return ret == Rejoining;
//}

PointIndex VisualPathData::GetNearFromOutside(const LineIndex& l, const PointType& p) const {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);
	assert(p != InvalidPoint);
	assert(PointAllignedWithLine(l, p));

	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());

	const PointType& A = Points[l.A].Pos;
	const PointType& B = Points[l.B].Pos;

	const bool IsHorizontal = LineIsHorizontal(l);

	const int coord = !IsHorizontal;//0 = x, 1 = y

	return (A[coord] < B[coord] ? p[coord] < A[coord] : p[coord] > A[coord]) ? l.A : l.B;
}

PointIndex VisualPathData::GetFarFromOutside(const LineIndex& l, const PointType& p) const {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);
	assert(p != InvalidPoint);
	assert(PointAllignedWithLine(l, p));

	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());

	const PointIndex& Near = GetNearFromOutside(l, p);
	return Near == l.A ? l.B : l.A;
}

void VisualPathData::AddLineIntegrateInsides(const PointIndex& a, const PointIndex& b, bool aHasReserved, bool bHasReserved) {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(a < Points.size());
	assert(b < Points.size());
	assert(a != b);

	if (LineExists(a, b) != InvalidLineIndex) return;

	//Search for Points
	//index, connected to previous
	std::vector<std::pair<PointIndex, bool>> PointsInside;
	for (size_t i = 0; i < Points.size(); i++) {
		if (i == a || i == b) continue;
		if (Points[i].IsFree())continue;
		const PointType& p = Points[i].Pos;
		if (PointStrictelyOnLine(Points[a].Pos, Points[b].Pos, p)) {
			PointsInside.emplace_back(i, false);
		}
	}

	//sort by distance to a
	std::sort(PointsInside.begin(),
		PointsInside.end(),
		[this, &a](const auto& p1, const auto& p2) {
			const PointType& P1 = Points[p1.first].Pos;
			const PointType& P2 = Points[p2.first].Pos;

			const PointType& A = Points[a].Pos;

			const int distA = std::abs(A.x() - P1.x()) + std::abs(A.y() - P1.y());//Distance A P1
			const int distB = std::abs(A.x() - P2.x()) + std::abs(A.y() - P2.y());//Distance B P1

			return distA < distB;
		});

	//For Absorbing terminating lines on the way
	std::vector<std::pair<PointIndex, LineIndex>> PointsWithLinesToRemove;

	//Mark Points that are connected to next
	for (size_t i = 0; i < PointsInside.size(); i++) {
		PointIndex Prev = i == 0 ? a : PointsInside[i - 1].first;
		if (LineExists(Prev, PointsInside[i].first) != InvalidLineIndex) {
			PointsInside[i].second = true;
		}
		if (Points[PointsInside[i].first].ConnectionCount() == 1) {
			if (i != 0) {//prev
				if (auto LineIndex = LineExists(PointsInside[i - 1].first, PointsInside[i].first); LineIndex != InvalidLineIndex) {
					PointsWithLinesToRemove.emplace_back(PointsInside[i].first, LineIndex);
				}
			}
			else {//A
				if (auto LineIndex = LineExists(a, PointsInside[i].first); LineIndex != InvalidLineIndex) {
					PointsWithLinesToRemove.emplace_back(PointsInside[i].first, LineIndex);
				}
			}
			if (i != PointsInside.size() - 1) {//next
				if (auto LineIndex = LineExists(PointsInside[i].first, PointsInside[i + 1].first); LineIndex != InvalidLineIndex) {
					PointsWithLinesToRemove.emplace_back(PointsInside[i].first, LineIndex);
				}
			}
			else {//B
				if (auto LineIndex = LineExists(PointsInside[i].first, b); LineIndex != InvalidLineIndex) {
					PointsWithLinesToRemove.emplace_back(PointsInside[i].first, LineIndex);
				}
			}
		}
	}
	for (size_t i = 0; i < PointsWithLinesToRemove.size(); i++) {
		auto [index, lineIndex] = PointsWithLinesToRemove[i];
		PointIndex Other = (index == lineIndex.A ? lineIndex.B : lineIndex.A);

		RemoveLineBetweenDeleteIndex(lineIndex, index);

		//Find Other in PointsWithLinesToRemove becaus of A   I---O   B and both have to be removed
		auto it = std::find_if(PointsWithLinesToRemove.begin() + i, PointsWithLinesToRemove.end(),
			[this, &Other](const auto& pair) {
				return pair.first == Other;
			});
		if (it != PointsWithLinesToRemove.end()) {
			RemovePoint(it->first);

			PointsWithLinesToRemove.erase(it);
		}

		for (size_t j = 0; j < PointsInside.size(); j++) {
			bool isFirst = PointsInside[j].first == index;
			bool isSecondCanBeRemoved = PointsInside[j].first == Other && Points[Other].IsFree();
			if (!isFirst && !isSecondCanBeRemoved) continue;//find index

			PointsInside.erase(PointsInside.begin() + j);
			if (j != PointsInside.size()) PointsInside[j].second = false;
			--j;
		}
	}


	PointIndex curr = a;

	for (auto& [index, connected] : PointsInside) {
		if (connected) {
			curr = index;
		}
		else {
			AddLineCorrectReserve(curr, index, curr == a && aHasReserved, false);

			curr = index;
		}
	}
	if (LineExists(curr, b) == InvalidLineIndex) AddLineCorrectReserve(curr, b, curr == a && aHasReserved, bHasReserved);
}

LineIndex VisualPathData::AddLineRemoveIfUnnecesary(const PointIndex& aIndex, const PointType& b, bool aHasReserved) {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(aIndex < Points.size());
	assert(!Points[aIndex].IsFree());
	assert(b != InvalidPoint);

	PointIndex pIndex = AddLine(aIndex, b, aHasReserved);
	return StreightLineMiddelRemove(pIndex);
	//return pIndex;
}

PointIndex VisualPathData::AddLine(const PointIndex& aIndex, const PointType& p, bool aHasReserved) {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(aIndex < Points.size());
	assert(!Points[aIndex].IsFree());
	assert(p != InvalidPoint);
	assert(PointsMakeStreightLine(Points[aIndex].Pos, p));

	if (auto PointIndex = PointExists(p); PointIndex != InvalidPointIndex) {
		if (LineExists(aIndex, PointIndex) != InvalidLineIndex) return PointIndex;
		AddLineIntegrateInsides(aIndex, PointIndex, aHasReserved, false);
		return PointIndex;
	}

	// b is inside Intercept
	if (auto InterceptIndex = Intercept(p); InterceptIndex != InvalidLineIndex) {

		// A--N--B---F      NearFar notation
		if (LineAllignedWithLine(InterceptIndex, Points[aIndex].Pos, p)) {

			//A=N--B--F
			//Is line with aIndex
			if (InterceptIndex.A == aIndex || InterceptIndex.B == aIndex) {
				return BreakUpLineAndInsert(InterceptIndex, p);
			}

			//Inside
			PointIndex pIndex = BreakUpLineAndInsert(InterceptIndex, p);
			AddLineIntegrateInsides(aIndex, pIndex, aHasReserved, false);
			return pIndex;
		}
		//Not Alligned  
		if (auto End = PointIsEndOfLine(InterceptIndex, p); End != InvalidPointIndex) {
			AddLineIntegrateInsides(aIndex, End, aHasReserved, false);
			return End;
		}
		//is inside
		PointIndex bIndex = BreakUpLineAndInsert(InterceptIndex, p);
		AddLineIntegrateInsides(aIndex, bIndex, aHasReserved, false);
		return bIndex;
	}

	//No Intercept but has aIndex has line in that direction
	for (LineIndexInPoint i = 0; i < 4; i++) {
		if (Points[aIndex].Connections[i] == InvalidPointIndex || Points[aIndex].Connections[i] == ReservedPointIndex) break;
		LineIndex aLineIndex{ aIndex,Points[aIndex].Connections[i]};
		const PointIndex OtherIndex = aLineIndex.B;
		//a -- o -- p
		if (PointStrictelyOnLine(Points[aIndex].Pos, p, Points[OtherIndex].Pos)) {
			if (Points[OtherIndex].ConnectionCount() == 1) {
				return MoveEndPointAlongItsOnlyLine(aLineIndex, OtherIndex == aLineIndex.A, p);
			}
			return AddLine(OtherIndex, p, false);
		}
	}

	assert(PointExists(p) == InvalidPointIndex);



	//PushPCAIC(0);
	//Points.emplace_back(p);
	//PointIndex bIndex = Points.size() - 1;

	//BoundingBox.GrowToInclude(p);

	const PointIndex pIndex = AddPoint(p);

	AddLineIntegrateInsides(aIndex, pIndex, aHasReserved, false);

	return pIndex;
}
//
////returnes index of created line
//LineIndex VisualPathData::AddLine(const PointIndex& aIndex, const PointIndex& bIndex) {
//#ifdef _DEBUG 
//	IsValidCaller __vc(this);
//#endif
//	assert(aIndex < Points.size());
//	assert(bIndex < Points.size());
//	assert(!PointIsDeleted(aIndex));
//	assert(!PointIsDeleted(bIndex));
//	assert(aIndex != bIndex);
//	assert(PointsMakeStreightLine(Points[aIndex], Points[bIndex]));
//	assert(LineExists(aIndex, bIndex) == InvalidLineIndex);
//	assert(Points[aIndex] != Points[bIndex]);
//	assert(GetConnectionCount(aIndex) != 0 || PointIsRejoining(aIndex) || GetConnectionCount(bIndex) != 0 || PointIsRejoining(bIndex));
//
//	IncrPointConnectionCount(aIndex, true);
//	IncrPointConnectionCount(bIndex, true);
//
//	Lines.emplace_back(aIndex, bIndex);
//	return Lines.size() - 1;
//}

PointIndex VisualPathData::MoveEndPointAlongItsOnlyLine(const LineIndex& l, bool first, const PointType& newPos) {
#ifdef _DEBUG 
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);

	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(newPos != InvalidPoint);

	PointIndex index = (first ? l.A : l.B);
	PointIndex otherIndex = (first ? l.B : l.A);

	assert(PointAllignedWithLine(l, newPos));
	assert(Points[index].ConnectionCount() == 1);

	if (auto InterceptIndex = Intercept(newPos); InterceptIndex != InvalidLineIndex) {
		PointIndex Other = RemoveLineBetweenDeleteIndex(l, index);
		if (auto End = PointIsEndOfLine(InterceptIndex, newPos); End != InvalidPointIndex) {
			AddLineIntegrateInsides(Other, End, false, false);
			return End;
		}

		PointIndex bIndex = BreakUpLineAndInsert(InterceptIndex, newPos);
		AddLineIntegrateInsides(bIndex, Other, false, false);
		return bIndex;
	}

	if (auto Existing = PointExists(newPos); Existing != InvalidPointIndex) {
		PointIndex Other = RemoveLineBetweenDeleteIndex(l, index);
		AddLineIntegrateInsides(Other, Existing, false, false);
		return Existing;
	}

	//Should be not posible
	assert(PointExists(newPos) == InvalidPointIndex);

	RemoveLineBetweenNoDelete(l);
	Points[index] = newPos;
	AddLineIntegrateInsides(otherIndex, index, false, false);

	return index;
}

void VisualPathData::RemovePoint(const PointIndex& p) {
#ifdef _DEBUG 
	IsValidCaller __vc(this);
#endif
	assert(p < Points.size());
	assert(!Points[p].IsFree());

	assert(Points[p].ConnectionCount() == 0);

	Points[p].Free(Head);
	Head = p;

	//SetPCAIC(p, Deleted);

	//p = InvalidPointIndex;
}


void VisualPathData::RemoveLineBetweenNoDelete(const LineIndex& l) {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);

	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(Block != nullptr);

	//This order becouase GetOther needs l.Point to be falid
	Points[l.B].RemoveConnection(l.A);
	Points[l.A].RemoveConnection(l.B);
#ifdef UseCollisionGrid
	Block->UnRegisterLine(l, Points, Id);
#endif
}

std::pair<PointIndex, PointIndex> VisualPathData::RemoveLineBetween(const LineIndex& l) {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);

	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(Block != nullptr);

	PointIndex aIndex = l.A;
	PointIndex bIndex = l.B;

	const bool aIsEnd = Points[aIndex].ConnectionCount() == 1;
	const bool bIsEnd = Points[bIndex].ConnectionCount() == 1;

	Points[aIndex].RemoveConnection(bIndex);
	Points[bIndex].RemoveConnection(aIndex);

#ifdef UseCollisionGrid
	Block->UnRegisterLine(l, Points, Id);
#endif

	if (aIsEnd && bIsEnd) {
		assert(false && "This is not posible, maby inside TryAbsorb but elswhere it should be hangging together");
	}
	else {
		if (aIsEnd) {
			RemovePoint(aIndex);
			aIndex = InvalidPointIndex;
		}
		else if (bIsEnd) {
			RemovePoint(bIndex);
			bIndex = InvalidPointIndex;
		}
	}
	if (aIsEnd || bIsEnd) {
		RecalculateBoundingBox();
	}

	assert(Points[aIndex].ConnectionCount() != 0 || Points[bIndex].ConnectionCount() != 0);


	return { aIndex,bIndex };
}

PointIndex VisualPathData::RemoveLineBetweenDeleteIndex(const LineIndex& l, const PointIndex& End) {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);
	assert(End < Points.size());
	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(End == l.A || End == l.B);
	assert(Block != nullptr);

	PointIndex Start = l.A == End ? l.B : l.A;

	assert(Start != End);
	assert(Points[End].ConnectionCount() == 1);
	assert(l.A == End || l.B == End);

	Points[Start].RemoveConnection(End);
	Points[End].RemoveConnection(Start);
#ifdef UseCollisionGrid
	Block->UnRegisterLine(l, Points, Id);
#endif

	RemovePoint(End);
	RecalculateBoundingBox();

	return Start;
}

PointIndex VisualPathData::BreakUpLineAndInsert(const LineIndex& l, const PointType& contained) {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);

	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(contained != InvalidPoint);

	//const Line& line = Lines[l];

	PointIndex aIndex = l.A;
	PointIndex bIndex = l.B;

	assert(PointStrictelyOnLine(l, contained));
	if (auto End = PointIsEndOfLine(l, contained); End != InvalidPointIndex) {
		return End;
	}

	RemoveLineBetweenNoDelete(l);
	Points[aIndex].AddReserved();
	Points[bIndex].AddReserved();
	PointIndex ContainedIndex = AddLine(aIndex, contained, true);

	assert(ContainedIndex != InvalidPointIndex);
	assert(Points[ContainedIndex].Pos == contained);

	////Not Possible todo remove
	//if (LineExists(bIndex, ContainedIndex) != InvalidLineIndex) {
	//	assert(false);
	//	return ContainedIndex;
	//}

	AddLineReservedUnreserved(bIndex, ContainedIndex);
	return ContainedIndex;
}

//For removals where it has to be a O(n) operation
void VisualPathData::RecalculateBoundingBox() {
	BoundingBox = MyRectI();
	for (const auto& p : Points) {
		if (p.IsFree())continue;
		BoundingBox.GrowToInclude(p.Pos);
	}
}
//
//void VisualPathData::Cleanup() {
//	std::sort(LinesToDelete.begin(), LinesToDelete.end(), std::greater());
//	for (const auto& i : LinesToDelete) {
//		Lines.erase(Lines.begin() + i);
//	}
//	LinesToDelete.clear();
//	if (Points.empty()) return;
//	for (size_t i = Points.size() - 1; i >= 0; i--) {
//		if (!PointIsDeleted(i)) {
//			assert(GetConnectionCount(i) != 0);
//			assert(GetConnectionCount(i) < 5);
//			if (i == 0)break;
//			continue;
//		}
//		Points.erase(Points.begin() + i);
//
//		ErasePCAIC(i);
//		for (auto& l : Lines) {
//			if (l.first > i) l.first--;
//			if (l.second > i) l.second--;
//		}
//		if (i == 0)break;
//	}
//}

bool VisualPathData::IsValidPd() const {
	if (!(Points.size() > 0) || Block == nullptr || Id == InvalidId)return false;
	//for (int i = 0; i < Lines.size(); i++) {
	//	if (LineIsDeleted(i))continue;
	//	assert(Lines[i].first < Points.size() && Lines[i].second < Points.size());
	//	if (PointIsDeleted(Lines[i].second) || PointIsDeleted(Lines[i].first) || !PointsMakeStreightLine(Points[Lines[i].first], Points[Lines[i].second])) {
	//		//auto hr = toHumanReadable();
	//		//assert(false);
	//	}
	//}
	//if (Lines.size() < 2) return true;
	//int i = Lines.size() - 1;
	////for (int i = 0; i < Lines.size(); i++) {
	//	if (LineIsDeleted(i)) return true;
	//	//if (LineIsDeleted(i)) continue;
	//	const auto& l = Lines[i];
	//	int counta = 0;
	//	int countb = 0;
	//	for (int j = 0; j < Lines.size(); j++) {
	//		if (LineIsDeleted(j)) continue;
	//		if (Lines[j].first == l.first || Lines[j].second == l.first)counta++;
	//		if (Lines[j].first == l.second || Lines[j].second == l.second)countb++;
	//	}
	//	//assert(counta > 1 || countb > 1);
	////}
	return true;
}

LineIndex VisualPathData::StreightLineMiddelRemove(const PointIndex& p) {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(p < Points.size());
	assert(!Points[p].IsFree());
	LineIndexInPoint con = Points[p].ConnectionCount();
	assert(con != 0);
	if (con != 2) {
		for (LineIndexInPoint i = 0; i < 4; i++) {
			if (Points[p].Connections[i] != InvalidPointIndex && Points[p].Connections[i] != ReservedPointIndex) {
				return LineIndex{ p,Points[p].Connections[i] };
			}
		}
		assert(false);
		//return LineIndex{ p,0 };
	}

	//std::vector<LineIndex> lines = LinesWithPoint(p);

	LineIndex l0 = InvalidLineIndex;
	LineIndex l1 = InvalidLineIndex;
	for (LineIndexInPoint i = 0; i < 4; i++) {
		if (Points[p].Connections[i] != InvalidPointIndex && Points[p].Connections[i] != ReservedPointIndex) {
			if (l0 == InvalidLineIndex) {
				l0 = LineIndex{ p,Points[p].Connections[i] };
				continue;
			}
			if (l1 == InvalidLineIndex) {
				l1 = LineIndex{ p,Points[p].Connections[i] };
				break;
			}
		}
	}


	if (LineIsHorizontal(l0) != LineIsHorizontal(l1)) return l0;


	/*PointIndex a = p;
	PointIndex b = l
	PointIndex a = (Lines[lines[0]].first == p) ? Lines[lines[0]].second : Lines[lines[0]].first;*/

	const PointIndex a = l0.B;


	RemoveLineBetweenNoDelete(l0);
	const PointIndex b = RemoveLineBetweenDeleteIndex(l1, p);
	AddLine(a, b);
	for (LineIndexInPoint i = 0; i < Points[a].ConnectionCount(); i++) {
		if (Points[a].Connections[i] == b) {
			return LineIndex{ a,Points[a].Connections[i]};
		}
	}
	assert(false);
	return InvalidLineIndex;
	//RemoveLineBetweenNoDelete(lines[0]);
	//DecrPointConnectionCount(p, false);
	//PointIndex b = RemoveLineBetweenDeleteIndex(lines[1], p);
	//DecrPointConnectionCount(a, false);
	//AddLine(a, b);
}

VisualPathData::VisualPathData(const PointType& a, const PointType& b, VisualBlockInterior* Block)
: Block(Block), Id(KlassInstanceCounter++)
{
	assert(PointsMakeStreightLine(a, b));
	assert(a != b);

	PointIndex aIndex = AddPoint(a);
	PointIndex bIndex = AddPoint(b);

	AddLine(aIndex, bIndex);

	LastAddedLine = LineIndex{ aIndex,bIndex};
}

VisualPathData::VisualPathData(const CompressedPathData& pd, VisualBlockInterior* Block)
	:LastAddedLine(pd.LastAddedLine)/*, BoundingBox(pd.BoundingBox)*/, Block(Block), Id(KlassInstanceCounter++) {
	for (const PointType& p : pd.Points) {
		AddPoint(p);
	}
	for (const CompressedPathData::Line& l : pd.Lines) {
		AddLine(l.first, l.second);
	}
	RecalculateBoundingBox();
}

bool VisualPathData::PointsMakeStreightLine(const PointType& a, const PointType& b) {
	assert(a != InvalidPoint);
	assert(b != InvalidPoint);

	return a.x() == b.x() || a.y() == b.y();
}
//
//bool VisualPathData::LineIsDeleted(const LineIndex& l) const {
//	assert(Lines.size() > 0);
//	assert(l != InvalidLineIndex);
//
//	return LinesToDelete.end() != std::find(LinesToDelete.begin(), LinesToDelete.end(), l);
//}
//
//std::vector<LineIndex> VisualPathData::LinesWithPoint(const PointIndex& p) const {
//#ifdef _DEBUG
//	IsValidCaller __vc(this);
//#endif
//	assert(p < Points.size());
//	assert(!PointIsDeleted(p));
//
//	std::vector<LineIndex> ret;
//	//int Count = GetConnectionCount(p);
//	//int TillNow = 0;
//	//if (Count == 0) return ret;
//	for (LineIndex i = 0; i < Lines.size(); i++) {
//		if (Lines[i].first == p || Lines[i].second == p) {
//			if (LineIsDeleted(i))continue;
//			ret.push_back(i);
//			//TillNow++;
//			//if (TillNow == Count)break;
//		}
//	}
//	//if (TillNow != Count) {
//		//int a = 0;
//	//}
//	return ret;
//}

bool VisualPathData::PointStrictelyOnLine(const PointType& a, const PointType& b, const PointType& p) {
	assert(a != InvalidPoint);
	assert(b != InvalidPoint);
	assert(p != InvalidPoint);

	if (a.x() == b.x()) {//|
		if (p.x() != a.x())return false;
		if ((a.y() <= p.y() && p.y() <= b.y())
			|| (b.y() <= p.y() && p.y() <= a.y())) {
			return true;
		}
	}
	else {//-
		if (p.y() != a.y())return false;
		if ((a.x() <= p.x() && p.x() <= b.x())
			|| (b.x() <= p.x() && p.x() <= a.x())) {
			return true;
		}
	}
	return false;
}

bool VisualPathData::PointStrictelyOnLine(const LineIndex& l, const PointType& p) const {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);

	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(p != InvalidPoint);

	const PointType& a = Points[l.A].Pos;
	const PointType& b = Points[l.B].Pos;

	return PointStrictelyOnLine(a, b, p);
}

bool VisualPathData::PointAllignedWithLine(const LineIndex& l, const PointType& p) const {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);
	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(p != InvalidPoint);

	const PointType& a = Points[l.A].Pos;
	const PointType& b = Points[l.B].Pos;

	return (a.x() == b.x() && p.x() == a.x()) || (a.y() == b.y() && p.y() == a.y());
}

bool VisualPathData::LineAllignedWithLine(const LineIndex& l, const PointType& a, const PointType& b) const {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);
	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(a != InvalidPoint);
	assert(b != InvalidPoint);

	return LineIsHorizontal(l) == LineIsHorizontal(a, b);
}

PointIndex VisualPathData::PointIsEndOfLine(const LineIndex& l, const PointType& p) const {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);
	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(p != InvalidPoint);

	if (p == Points[l.A].Pos)return l.A;
	if (p == Points[l.B].Pos)return l.B;

	return InvalidPointIndex;
}

bool VisualPathData::LineIsHorizontal(const LineIndex& l) const {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);
	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());

	const PointType& a = Points[l.A].Pos;
	const PointType& b = Points[l.B].Pos;

	return a.y() == b.y();
}

bool VisualPathData::LineIsHorizontal(const PointType& a, const PointType& b) {
	return a.y() == b.y();
}

PointType VisualPathData::GetProjectionOnLine(const LineIndex& l, const PointType& p) const {
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l != InvalidLineIndex);
	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(p != InvalidPoint);

	const PointType& a = Points[l.A].Pos;
	const PointType& b = Points[l.B].Pos;

	if (a.x() == b.x()) {//|
		return { a.x(),p.y() };
	}
	//-
	return { p.x(),a.y() };
}

#ifdef UseCollisionGrid
VisualPathData::~VisualPathData() {
	//Deallcoate Points and remove them
	Clear();
}

void VisualPathData::Clear() noexcept {
	for (PointIndex pIndex = 0; pIndex < Points.size(); pIndex++) {
		for (LineIndexInPoint lIndex = 0; lIndex < 4; lIndex++) {
			if (Points[pIndex].IsFree())continue;
			const PointIndex& Other = Points[pIndex].Connections[lIndex];
			if (Other <= pIndex) {
				Block->UnRegisterLine({ pIndex,Other }, Points, Id);
			}
		}
	}
}
#endif

LineIndex VisualPathData::Intercept(const PointType& pos) const {
	PROFILE_FUNKTION;
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(pos != InvalidPoint);
//#define Optimized
#ifdef UseCollisionGrid
	return Block->GetLineWith(pos, *this);
#else
	for (PointIndex i = 0; i < Points.size(); i++) {
		const PointNode& p = Points[i];
		if (p.IsFree())continue;
		for (LineIndexInPoint j = 0; j < 4; j++) {
			if (p.Connections[j] == InvalidPointIndex || p.Connections[j] == ReservedPointIndex)break;
			LineIndex l{ i,p.Connections[j]};
			if (PointStrictelyOnLine(l, pos))return l;
		}
	}
	return InvalidLineIndex;
#endif
	//Todo Makover
}

void VisualPathData::SetLastAdded(const PointIndex& pIndex) {
	const PointNode& p = Points[pIndex];
	for (LineIndexInPoint i = 0; i < 4; i++) {
		if (p.Connections[i] != InvalidPointIndex) {
			LastAddedLine = { pIndex,p.Connections[i]};
		}
		else {
			return;
		}
	}
}

void VisualPathData::SetLastAdded(const LineIndex& pIndex) {
	if (pIndex != InvalidLineIndex)LastAddedLine = pIndex;
}

//
//bool VisualPathData::addTo(const LineIndex& l, const PointType& p) {
//#ifdef _DEBUG
//	IsValidCaller __vc(this);
//#endif
//	assert(!Points[l.A].IsFree());
//	assert(!Points[l.B].IsFree());
//	assert(p != InvalidPoint);
//
//	LineIndex l1 = l;
//	return addTo(l1, p);
//}

bool VisualPathData::addTo(const LineIndex& l, const PointType& p) {
	PROFILE_FUNKTION;
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(l.A < Points.size());
	assert(l.B < Points.size());
	assert(!Points[l.A].IsFree());
	assert(!Points[l.B].IsFree());
	assert(p != InvalidPoint);

	const PointType& A = Points[l.A].Pos;
	const PointType& B = Points[l.B].Pos;

	if (PointIsEndOfLine(l, p) != InvalidPointIndex)return false;

	const bool IsHorizontal = LineIsHorizontal(l);

	const int coord = IsHorizontal;//0 = x, 1 = y

	const PointType Projection = GetProjectionOnLine(l, p);

	if (Projection == InvalidPoint)return false;

	PointIndex FirstOfAOrBFromOutside = GetNearFromOutside(l, Projection);

	if (PointAllignedWithLine(l, p)) {
		//Is Contained
		if (PointStrictelyOnLine(l, Projection)) return false;

		if (Points[FirstOfAOrBFromOutside].ConnectionCount() == 1) {
			SetLastAdded(MoveEndPointAlongItsOnlyLine(l, FirstOfAOrBFromOutside == l.A, p));
			return true;
		}
		SetLastAdded(AddLineRemoveIfUnnecesary(FirstOfAOrBFromOutside, p, false));
		return true;
	}
	//not same direction
	if ((!IsHorizontal && p.x() == A.x()) || (IsHorizontal && p.y() == A.y())) {//trivial extention on A
		SetLastAdded(AddLineRemoveIfUnnecesary(l.A, p, false));
		return true;
	}
	else if ((!IsHorizontal && p.x() == B.x()) || (IsHorizontal && p.y() == B.y())) {//trivial extention on B
		SetLastAdded(AddLineRemoveIfUnnecesary(l.B, p, false));
		return true;
	}
	else if (PointStrictelyOnLine(l, Projection)) {
		PointIndex ProjIndex = BreakUpLineAndInsert(l, Projection);
		SetLastAdded(AddLineRemoveIfUnnecesary(ProjIndex, p, false));
		return true;
	}
	//Place Projection Simmularely to Point with same direciton
	if (Points[FirstOfAOrBFromOutside].ConnectionCount() == 1) {

		PointIndex MovedIndex = MoveEndPointAlongItsOnlyLine(l, FirstOfAOrBFromOutside == l.A, Projection);
		SetLastAdded(AddLineRemoveIfUnnecesary(MovedIndex, p, false));
		return true;
	}

	PointIndex ProjIndex = AddLine(FirstOfAOrBFromOutside, Projection, false);
	if (ProjIndex == InvalidPointIndex)return false;
	assert(Points[ProjIndex].Pos == Projection);
	SetLastAdded(AddLineRemoveIfUnnecesary(ProjIndex, p, false));
	return true;
}

//addToEnd
bool VisualPathData::addTo(const PointType& p) {
	PROFILE_FUNKTION;
#ifdef _DEBUG
	IsValidCaller __vc(this);
#endif
	assert(p != InvalidPoint);

	return addTo(LastAddedLine, p);
}

bool VisualPathData::AbsorbIfIntercept(VisualPathData& Other) {
	PROFILE_FUNKTION;
	if (!BoundingBox.Intersectes(Other.BoundingBox))return false;
	if (Other.Points.size() == 0)return false;

	PROFILE_SCOPE_ID_START("Search in this", 0);
	//DetectCollision
	bool FoundCollision = false;
	for (PointIndex p = 0; p < Other.Points.size(); p++) {
		if (Other.Points[p].IsFree())continue;
		if (Intercept(Other.Points[p].Pos) != InvalidLineIndex) {
			FoundCollision = true;
			break;
		}
	}
	PROFILE_SCOPE_ID_END(0);

	PROFILE_SCOPE_ID_START("Search in other", 1);
	if (!FoundCollision) {
		for (PointIndex p = 0; p < Points.size(); p++) {
			if (Points[p].IsFree())continue;
			if (Other.Intercept(Points[p].Pos) != InvalidLineIndex) {
				FoundCollision = true;
				break;
			}
		}
	}
	PROFILE_SCOPE_ID_END(1);

	if (!FoundCollision)return false;

	PROFILE_SCOPE_ID_START("Insert Missing Point In This", 2);

	std::vector<PointIndex> IndeciesOfOtherPointsInSelf;
	IndeciesOfOtherPointsInSelf.reserve(Other.Points.size());

	//Figure out Indecies potentially adding points on the way
	for (PointIndex i = 0; i < Other.Points.size(); i++) {
		if (Other.Points[i].IsFree()) {
			IndeciesOfOtherPointsInSelf.push_back(InvalidPointIndex);
			continue;
		}

		const PointType& p = Other.Points[i].Pos;
		if (auto InterceptIndex = Intercept(p); InterceptIndex != InvalidLineIndex) {
			if (auto End = PointIsEndOfLine(InterceptIndex, p); End != InvalidPointIndex) {
				IndeciesOfOtherPointsInSelf.push_back(End);
			}
			else {
				IndeciesOfOtherPointsInSelf.push_back(BreakUpLineAndInsert(InterceptIndex, p));
			}
		}
		else {
			assert(PointExists(p) == InvalidPointIndex);

			PointIndex pIndex = AddPoint(p);

			IndeciesOfOtherPointsInSelf.push_back(pIndex);
		}
	}

	PROFILE_SCOPE_ID_END(2);

	PROFILE_SCOPE_ID_START("Add The Lines", 3);

	for (PointIndex i = 0; i < Other.Points.size(); i++) {
		if (IndeciesOfOtherPointsInSelf[i] == InvalidPointIndex)continue;
		if (Other.Points[i].IsFree())continue;
		for (LineIndexInPoint j = 0; j < 4; j++) {
			if (Other.Points[i].Connections[j] == InvalidPointIndex || Other.Points[i].Connections[j] == ReservedPointIndex)continue;
			const PointIndex OtherIndex = Other.Points[i].Connections[j];
			if (i > OtherIndex || IndeciesOfOtherPointsInSelf[Other.Points[i].Connections[j]] == InvalidPointIndex) continue;
			AddLineIntegrateInsides(IndeciesOfOtherPointsInSelf[i], IndeciesOfOtherPointsInSelf[Other.Points[i].Connections[j]], false, false);
		}
	}

	PROFILE_SCOPE_ID_END(3);


	/*for (const Line& l : Other.Lines) {
		PointIndex a = IndeciesOfOtherPointsInSelf[l.first];
		PointIndex b = IndeciesOfOtherPointsInSelf[l.second];
		AddLineIntegrateInsides(a, b);
	}*/

	PROFILE_SCOPE_ID_START("Make Lines Streight Again", 4);

	//A - b - B
	for (PointIndex p = 0; p < Points.size(); p++) {
		if (Points[p].IsFree())continue;

		StreightLineMiddelRemove(p);

		//if (PointIsDeleted(p))continue;
		//if (GetConnectionCount(p) != 2) continue;

		//std::vector<LineIndex> lines = LinesWithPoint(p);

		////ToDU Use the FUnktion For THis

		//if (LineIsHorizontal(lines[0]) != LineIsHorizontal(lines[1])) continue;

		//PointIndex a = (Lines[lines[0]].first == p) ? Lines[lines[0]].second : Lines[lines[0]].first;

		//PointIndex pcopy = p;

		//RemoveLineBetweenNoDelete(lines[0]);
		//DecrPointConnectionCount(p, false);
		//PointIndex b = RemoveLineBetweenDeleteIndex(lines[1], p);
		//DecrPointConnectionCount(a, false);
		//AddLine(a, b);
	}
	PROFILE_SCOPE_ID_END(4);

	//Other.Points.clear();
	//Other.PCAIC.clear();
	//Other.PCAICSize = 0;
	//Other.Lines.clear();
	//Other.BoundingBox = MyRectI();//Invalidate

	return true;
}


std::string VisualPathData::PointConnectionToString(const PointIndex& p) const {
	switch (p) {
	case ReservedPointIndex:
		return "res";
		break;
	case InvalidPointIndex:
		return "null";
		break;
	case FreePointIndex:
		return "free";
		break;
	case FreeListEndPointIndex:
		return "FLT";
		break;
	default:
		return std::to_string(p);
		break;
	}
}
//
//std::string VisualPathData::PointConnectionToString(const LineIndex& l) const {
//	return PointConnectionToString(l.B);
//}

std::string VisualPathData::toHumanReadable() const {
	//#ifdef _DEBUG IsValidCaller __vc(this); #endif
	std::stringstream ss;

	ss << "LastAdded:\t";
	ss << PointConnectionToString(LastAddedLine.A) << " - " << PointConnectionToString(LastAddedLine.B) << "\n";
	/* << "Point: " << PointConnectionToString(LastAdded.Point) <<
	" Index: " << std::to_string(LastAdded.ConnectionIndex) << "\n";*/
	ss << "BoundingBox: [(" << BoundingBox.Position.x() << ", " << BoundingBox.Position.y() << "), (" << BoundingBox.Position.x() + BoundingBox.Size.x() << ", " << BoundingBox.Position.y() + BoundingBox.Size.y() << ")]\n";
	ss << "FreeListHead:\t" << PointConnectionToString(Head) << "\n";
	ss << "Points\n";
	for (PointIndex i = 0; i < Points.size(); i++) {
		const PointNode& p = Points[i];
		ss << i << ": " << std::setw(10) << std::left;
		/*if (p.IsFree()) {
			ss << "Free";
		}
		else {*/
		if (p.Pos == InvalidPoint) {
			ss << "Invalid ";
		}
		else {
			ss << std::string("(" + std::to_string(p.Pos.x()) + ", " + std::to_string(p.Pos.y()) + ") ");
		}
		for (LineIndexInPoint j = 0; j < 4; j++) {
			std::string s;

			ss << std::setw(6) << std::left << PointConnectionToString(Points[i].Connections[j]) << "\t";
		}
		ss << "\n";
	}
	return ss.str();

	//ss << "Points: \n";
	//int i = 0;
	//for (int i = 0; i < Points.size(); i++) {
	//	const auto& p = Points[i];
	//	std::vector<LineIndex> Conn;
	//	for (int j = 0; j < Lines.size(); j++) {
	//		const auto& l = Lines[j];
	//		if ((l.first == i || l.second == i) && !LineIsDeleted(j)) {
	//			Conn.push_back(l.first == i ? l.second : l.first);
	//		}
	//	}

	//	int Con = GetPCAIC(i);

	//	std::string ConStr = Con ==/* FehlerOverflow ? "FehlerOverflow" : (*/
	//		Con == Rejoining ? "Rejoining" : (
	//			Con == Deleted ? "Deleted " : (
	//				std::to_string(Con)))/*)*/;
	//	if (PointIsDeleted(i)) ss << "-";
	//	else ss << i;
	//	ss << ":\t(" << p.x() << ", " << p.y() << ") con:"
	//		<< ConStr << " ";
	//	if (Conn.size() != Con)ss << "(" << "Should be " << Conn.size() << ") ";
	//	if (Conn.size() > 0) {
	//		ss << "(";
	//		for (int i = 0; i < Conn.size(); i++) {
	//			ss << Conn[i];
	//			if (i != Conn.size() - 1)ss << ", ";
	//		}
	//		ss << ")";
	//	}
	//	ss << "\n";
	//}
	//ss << "Lines: \n";
	//i = 0;
	//for (const auto& l : Lines) {
	//	if (LineIsDeleted(i)) ss << "-";
	//	else ss << i;
	//	i++;
	//	ss << ":\t" << l.first << " " << l.second << "\n";
	//}
	//ss << "Lines-Replaced: \n";
	//i = 0;
	//for (const auto& l : Lines) {
	//	if (LineIsDeleted(i)) ss << "-";
	//	else ss << i;
	//	ss << ":\t((";
	//	i++;
	//	if (l.first < Points.size()) ss << Points[l.first].x() << ", " << Points[l.first].y();
	//	else {
	//		ss << "null, null";
	//	}
	//	ss << "), (";
	//	if (l.second < Points.size()) ss << Points[l.second].x() << ", " << Points[l.second].y();
	//	else {
	//		ss << "null, null";
	//	}
	//	ss << "))\n";
	//}
	//return ss.str();
}
//
//VisualPathData::SceduleCleanup::~SceduleCleanup() {
//	Data->Cleanup();
//}

LineIndexInPoint PointNode::ConnectionCount() const noexcept {
	assert(Connections[0] != FreePointIndex);
	unsigned char ret = 0;
	for (LineIndexInPoint i = 0; i < 4; i++) {
		if (Connections[i] != InvalidPointIndex) {
			ret++;
		}
		//ret++;
	}
	return ret;
}

PointIndex PointNode::Init(const PointType& p) {
	assert(Connections[0] == FreePointIndex);
	Pos = p;
	const auto PointIndexNext = Connections[1];
	Connections[0] = InvalidPointIndex;
	Connections[1] = InvalidPointIndex;
	return PointIndexNext;
}

//Set this head afterwareds

void PointNode::Free(const PointIndex& head) {
	assert(Connections[0] != FreePointIndex);
	Connections = std::array{ FreePointIndex,head,InvalidPointIndex,InvalidPointIndex };
#ifdef _DEBUG
	Pos = InvalidPoint;
#endif
}

bool PointNode::IsFree() const {
	return Connections[0] == FreePointIndex;
}

void PointNode::AddConnection(const PointIndex& p) {
	assert(Connections[0] != FreePointIndex);
	assert(Connections[0] != p);
	assert(Connections[1] != p);
	assert(Connections[2] != p);
	assert(Connections[3] != p);
	for (LineIndexInPoint i = 0; i < 4; i++) {
		if (Connections[i] == InvalidPointIndex) {
			Connections[i] = p;
			return;
		}
		/*if (Connections[i] == ReservedPointIndex) {
		Connections[i] = p;
		PointIndex Last = ReservedPointIndex;
		for (LineIndexInPoint j = i + 1; j < 4; j++) {
		Last = std::exchange(Connections[j], Last);
		}
		return;
		}*/
	}
	assert(false);
	//std::unreachable();
}

void PointNode::AddConnectionReserved(const PointIndex& p) {
	assert(Connections[0] != FreePointIndex);
	assert(Connections[0] != p);
	assert(Connections[1] != p);
	assert(Connections[2] != p);
	assert(Connections[3] != p);
	for (LineIndexInPoint i = 0; i < 4; i++) {
		if (Connections[i] == ReservedPointIndex) {
			Connections[i] = p;
			return;
		}
	}
	assert(false);
	//std::unreachable();
}

void PointNode::AddReserved() {
	assert(Connections[0] != FreePointIndex);
	for (LineIndexInPoint i = 0; i < 4; i++) {
		if (Connections[i] == InvalidPointIndex) {
			Connections[i] = ReservedPointIndex;
			return;
		}
	}
	assert(false);
	//std::unreachable();
}

void PointNode::RemoveConnection(const PointIndex& p) {
	//Remove at index
	assert(Connections[0] == p
		|| Connections[1] == p
		|| Connections[2] == p
		|| Connections[3] == p);

	for (LineIndexInPoint i = 0; i < 4; i++) {
		if (Connections[i] == p) {
			Connections[i] = InvalidPointIndex;
			return;
		}
	}
	assert(false);
	//LineIndexInPoint removedAt = 4;
	//for (LineIndexInPoint i = 0; i < 4; i++) {
	//	if (Connections[i] == p) {
	//		removedAt = i;
	//		break;
	//	}
	//}
	//assert(removedAt != 4);
	////Move Rest to Left
	//for (LineIndexInPoint i = removedAt; i < 3; i++) {
	//	Connections[i] = Connections[i + 1];
	//}
	//Connections[3] = InvalidPointIndex;
	//std::unreachable();
}

CompressedPathData::CompressedPathData(const VisualPathData& pd)
	:LastAddedLine(pd.LastAddedLine) /*, BoundingBox(pd.BoundingBox)*/
{
	std::vector<PointIndex> IndexInSelf(pd.Points.size(), InvalidPointIndex);

	for (PointIndex i = 0; i < pd.Points.size(); i++) {
		if (pd.Points[i].IsFree())continue;
		Points.push_back(pd.Points[i].Pos);
		IndexInSelf[i] = Points.size() - 1;
	}

	for (PointIndex i = 0; i < pd.Points.size(); i++) {
		if (pd.Points[i].IsFree())continue;
		for (LineIndexInPoint l = 0; l < 4; l++) {
			const PointIndex& val = pd.Points[i].Connections[l];
			if (val == InvalidPointIndex)continue;
			if (i > val) continue;
			Lines.emplace_back(IndexInSelf[i], IndexInSelf[val]);
		}
	}
}

std::string CompressedPathData::toHumanReadable() const {
	std::stringstream ss;
	ss << "Points: \n";
	int i = 0;
	for (int i = 0; i < Points.size(); i++) {
		const auto& p = Points[i];
		std::vector<LineIndex> Conn;
		for (int j = 0; j < Lines.size(); j++) {
			const auto& l = Lines[j];
			if ((l.first == i || l.second == i)) {
				Conn.push_back(l.first == i ? l.second : l.first);
			}
		}

		ss << i;
		ss << ":\t(" << p.x() << ", " << p.y() << ") con:"
			<< Conn.size() << " ";
		if (Conn.size() > 0) {
			ss << "(";
			for (int i = 0; i < Conn.size(); i++) {
				ss << Conn[i];
				if (i != Conn.size() - 1)ss << ", ";
			}
			ss << ")";
		}
		ss << "\n";
	}
	ss << "Lines: \n";
	i = 0;
	for (const auto& l : Lines) {
		ss << i;
		i++;
		ss << ":\t" << l.first << " " << l.second << "\n";
	}
	ss << "Lines-Replaced: \n";
	i = 0;
	for (const auto& l : Lines) {
		ss << i;
		ss << ":\t((";
		i++;
		if (l.first < Points.size()) ss << Points[l.first].x() << ", " << Points[l.first].y();
		else {
			ss << "null, null";
		}
		ss << "), (";
		if (l.second < Points.size()) ss << Points[l.second].x() << ", " << Points[l.second].y();
		else {
			ss << "null, null";
		}
		ss << "))\n";
	}
	return ss.str();
}
