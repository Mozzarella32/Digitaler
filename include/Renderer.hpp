#pragma once

#include "pch.h"

#include "Include.h"

#include "OpenGlDefines.h"

#include "BlockAndPathData.h"

#include "MyRect.h"

#include "Block.h"

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

private:
	void RenderBackground();

	VertexArrayObject EdgesVAO;
	VertexArrayObject EdgesMarkedVAO;

	VertexArrayObject EdgesUnmarkedVAO;
	VertexArrayObject SpecialPointsVAO;
	VertexArrayObject VertsVAO;

#ifdef RenderCollisionGrid
	VertexArrayObject BlocksVAO;
#endif

	PointType MouseIndex;

	VertexArrayObject CreatePathVertexVAO() {
		VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
			{GL_STATIC_DRAW, IndexVertex{}},
			{ GL_DYNAMIC_DRAW, PathVertex{ 0,0,0,0 },1 }
		} };

		VAO.bind();
		VAO.ReplaceVertexBuffer(
			std::vector<IndexVertex>{
				{ 0 },
				{ 1 },
				{ 2 },
			{ 3 }}, 0);
		VAO.unbind();
		return VAO;
	}

	VertexArrayObject CreatePointVertexVAO() {
		VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
			{GL_STATIC_DRAW, IndexVertex{}},
			{ GL_DYNAMIC_DRAW, PointVertex{ 0,0 },1 }
		} };

		VAO.bind();
		VAO.ReplaceVertexBuffer(
			std::vector<IndexVertex>{
				{ 0 },
				{ 1 },
				{ 2 },
			{ 3 }}, 0);
		VAO.unbind();
		return VAO;
	}

#ifdef RenderCollisionGrid
	VertexArrayObject CreateBlockRGBAVAO() {
		VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
			{GL_STATIC_DRAW, IndexVertex{}},
			{ GL_DYNAMIC_DRAW, RectRGBAVertex{ 0,0,0,0, 0,0,0,0 },1 }
		} };

		VAO.bind();
		VAO.ReplaceVertexBuffer(
			std::vector<IndexVertex>{
				{ 0 },
				{ 1 },
				{ 2 },
			{ 3 }}, 0);
		VAO.unbind();
		return VAO;
	}
#endif
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