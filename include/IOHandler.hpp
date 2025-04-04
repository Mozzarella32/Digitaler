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
	Eigen::Vector2i MouseIndex;

	std::vector<VisualPathData> Paths;

public:
	KeyboardData Keyboarddata{};
	MyFrame* Frame;

private:
	//std::unique_ptr<Board> board;

	//Returnes if the index changed
	bool UpdateMouseIndex(const wxPoint& Pos);

	Eigen::Vector2f PixelToScreenCoord(const wxPoint& Pos);

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
private:

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
	};

	State state = State::Normal;
	void SetState(const State& state);

	State GoHomeSaveState;
public:
	std::string GetStateString()const;
	//BlockData BD;
	//std::optional<PointType> LastPoint;
public:

	void DoLoop();

private:
	bool Click = false;
	wxPoint DraggingOrigin;
	Eigen::Vector2f AreaFirstPoint;
	Eigen::Vector2f AreaSecondPoint;
public:

	void OnMouseWheel(const wxPoint& wxPos, double Rotation);
	void OnMouseMove(const wxPoint& wxPos);
	void OnMouseDown(const wxPoint& wxPos);
	void OnMouseUp(const wxPoint& wxPos);
	void OnRightMouseDown(const wxPoint& wxPos);
	void OnDClick(const wxPoint& wxPos);
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