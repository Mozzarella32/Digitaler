#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "OpenGlDefines.hpp"

#include "BlockAndPathData.hpp"

#include "MyRect.hpp"

#include "Block.hpp"

class MyApp;
class MyFrame;

class Renderer {
public:
	MyFrame* Frame;
	MyApp* App;

public:
	double Zoom = 0.01;
	Eigen::Vector2f Offset = { 0,0 };
	Eigen::Vector2f CanvasSize;
	Eigen::Matrix3f ViewProjectionMatrix;

	MyRectI BoundingBox;

	bool Dirty = true;
private:

	//Order is important
	Texture FBOBackgroundTexture;
	FrameBufferObject FBOBackground;

	void RenderBackground();

	Texture FBOPathStencileDepthTexture;
	Texture FBOPathColorTexture;
	FrameBufferObject FBOPath;

	VertexArrayObject EdgesVAO;
	VertexArrayObject EdgesMarkedVAO;

	VertexArrayObject EdgesUnmarkedVAO;
	VertexArrayObject SpecialPointsVAO;
	VertexArrayObject VertsVAO;

	VertexArrayObject SevenSegVAO;
	VertexArrayObject SixteenSegVAO;

	VertexArrayObject MuxVAO;

#ifdef RenderCollisionGrid
	VertexArrayObject BlocksVAO;
#endif
	VertexArrayObject PinVAO;
	VertexArrayObject RoundPinVAO;

	VertexArrayObject AndVAO;
	VertexArrayObject OrVAO;
	VertexArrayObject XOrVAO;

	PointType MouseIndex;

	VertexArrayObject CreateTwoPointIVertexVAO();

	VertexArrayObject CreatePointIVertexVAO();

	VertexArrayObject CreateSevenSegVAO();
	VertexArrayObject CreateSixteenSegVAO();

	VertexArrayObject CreateMuxVAO();

#ifdef RenderCollisionGrid
	VertexArrayObject CreateBlockRGBAVAO();
#endif
	VertexArrayObject CreatePointIOrientationRGBVertexVAO();

	VertexArrayObject CreatePointIRGBVertex();

public:
	unsigned int FrameCount = 0;

	void Render();

	void UpdateViewProjectionMatrix();

	Eigen::Vector2i CoordToNearestPoint(Eigen::Vector2f Pos);

	constexpr static const float ZoomDotsBorder = 0.03;
	bool ShowDots = true;
	void CheckZoomDots();

	Renderer(MyApp* App, MyFrame* Frame);

	void UpdateSize();

	void UpdateMouseIndex(const PointType& MouseIndex);
};