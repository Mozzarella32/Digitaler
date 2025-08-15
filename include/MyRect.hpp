#pragma once

#include "pch.hpp"

#include "Include.hpp"

template<typename T>
class MyRect {
public:
	T Position = { 0,0 };
	T Size = InvalidSize;

	static const T InvalidSize;

	MyRect(const T& Position, const T& Size)
		:
		Position(Position),
		Size(Size)
	{
	}

	static MyRect FromCorners(const T& Corner1, const T& Corner2) {
		return MyRect(
			T(
				std::min(Corner1.x(), Corner2.x()),
				std::min(Corner1.y(), Corner2.y())
			),
			T(
				std::abs(Corner1.x() - Corner2.x()),
				std::abs(Corner1.y() - Corner2.y())
			)
		);
	}

	MyRect() {};

	MyRect(const MyRect& other)
		:Position(other.Position), Size(other.Size){}

	MyRect(MyRect&& other)
		:Position(std::move(other.Position)), Size(std::move(other.Size)){}

	MyRect& operator=(const MyRect& other) {
		Position = other.Position;
		Size = other.Size;
		return *this;
	}

	MyRect& operator=(MyRect&& other) {
		Position = std::move(other.Position);
		Size = std::move(other.Size);
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const MyRect& r) {
		os << "Position: " << r.Position.x() << " " << r.Position.y() << " Size: " << r.Size.x() << " " << r.Size.y();
		return os;
	}

	bool Contains(const T& Point) const {
		if (Size == InvalidSize)return false;
		return Point.x() >= Position.x() && Point.x() <= Position.x() + Size.x()
			&& Point.y() >= Position.y() && Point.y() <= Position.y() + Size.y();
	}

	bool IsContainedIn(const MyRect<T>& other) const {
		return (Position.x() >= other.Position.x()) &&
			(Position.y() >= other.Position.y()) &&
			(Position.x() + Size.x() <= other.Position.x() + other.Size.x()) &&
			(Position.y() + Size.y() <= other.Position.y() + other.Size.y());
	}

	bool Intersectes(const MyRect& other) const {
		if (Size == InvalidSize || other.Size == InvalidSize)return false;
		return Position.x() <= other.Position.x() + other.Size.x() && Position.x() + Size.x() >= other.Position.x()
			&& Position.y() <= other.Position.y() + other.Size.y() && Position.y() + Size.y() >= other.Position.y();
	}

	void GrowToInclude(const T& p) {
		if (Size == InvalidSize) {
			Position = p;
			Size = T{ 0,0 };
			return;
		}
		T newLower = T{
			std::min(Position.x(), p.x()),
			std::min(Position.y(), p.y()) };

		T newUpper = T{
			std::max(Position.x() + Size.x(), p.x()),
			std::max(Position.y() + Size.y(), p.y()) };

		*this = MyRect::FromCorners(newLower, newUpper);
	}


	bool operator==(const MyRect& other) const {
		return Position == other.Position && Size == other.Size;
	}

	bool operator!=(const MyRect& other) const {
		return !(*this == other);
	}
};

template<typename T>
const T MyRect<T>::InvalidSize = { -1,-1 };

using MyRectF = MyRect<Eigen::Vector2f>;
using MyRectI = MyRect<Eigen::Vector2i>;
