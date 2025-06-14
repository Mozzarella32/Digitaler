#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "OpenGlDefines.hpp"

#include "BlockAndPathData.hpp"

#include "MyRect.hpp"

class MyFrame;

struct KeyboardData {
	bool W : 1;
	bool A : 1;
	bool S : 1;
	bool D : 1;

	bool E : 1;
	bool Shift : 1;
	bool Strng : 1;

	//bool F : 1;

	Eigen::Vector2f MousePosition;
};

class IOHandler {
public:
	PointType MouseIndex;

	std::vector<VisualPathData> Paths;

public:
	KeyboardData Keyboarddata{};
	MyFrame* Frame;

private:
	//std::unique_ptr<Board> board;

	//Returnes if the index changed
	bool UpdateMouseIndex(const Eigen::Vector2f& Pos);

	Eigen::Vector2f PixelToScreenCoord(const Eigen::Vector2f& Pos);

	//bool ClickMenu(wxPoint Mouse);

	/*
	std::vector<PathVertex> edges;
	std::vector<PathVertex> edgesMarked;
	std::vector<PathVertex> edgesUnmarked;
	std::vector<PointVertex> specialPoints;
	std::vector<PathVertex> verts;
	*/

	//std::unique_ptr<VertexArrayObject> LinesVAO;
	
	//std::unique_ptr<VertexArrayObject> PointsVAO;

public:
	IOHandler(MyFrame* Frame);

	//void Initilise();

private:
	//void FrameGuiUpdate();
	void MoveViewUpdate();

private:
	

private:
	//bool NextPlacePossible = true;
	//float DeleterAnimation = 0.0;
	//private:
public:

	enum class State {
		Normal,
		Dragging,
		DraggingWhilePlacingLine,
		PlacingLine,
		Marking,
		DraggingWithMarking,
		DraggingMarked,
		AreaFirstPoint,
		DraggingWhileAreaFistPoint,
		GoHome,
		Loading,
	};

private:
	State state = State::Loading;
public:
	void SetState(const State& state);

private:

	State GoHomeSaveState;
public:
	std::string GetStateString()const;
	//BlockData BD;
	//std::optional<PointType> LastPoint;
public:

	void DoLoop();

private:
	bool Click = false;
	Eigen::Vector2f DraggingOrigin;
	Eigen::Vector2f AreaFirstPoint;
	Eigen::Vector2f AreaSecondPoint;
public:

	void OnMouseWheel(const Eigen::Vector2f& pos, double Rotation);
	void OnMouseMove(const Eigen::Vector2f& pos);
	void OnMouseDown(const Eigen::Vector2f& pos);
	void OnMouseUp(const Eigen::Vector2f& pos);
	void OnRightMouseDown(const Eigen::Vector2f& pos);
	void OnDClick(const Eigen::Vector2f& pos);
	void StoppDragg();

	void OnKeyDown(wxKeyEvent& evt);
	void OnKeyUp(wxKeyEvent& evt);
	void OnChar(wxKeyEvent& evt);

	void OnCanvasSizeChange();

	void OnDelete();
private:
	//Helpers for unduplication



	//void PlaceNextPlaceBuilding();

	//void RotateNextPlaceBuilding();

	//void FlipNextPlaceBuilding();

	//void PickNextPlaceBuilding();

	//void DeleteUnderCursor();

	//void RotateNextPlaceBuilding() {
	//	if(NextPlaceBuilding->)
	//}
};
