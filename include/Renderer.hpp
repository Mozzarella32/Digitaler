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
	MyApp* App;
	MyFrame* Frame;

public:
	double Zoom = 0.01;
	Eigen::Vector2f Offset = { 0,0 };
	Eigen::Vector2f CanvasSize;
	Eigen::Matrix3f ViewProjectionMatrix;

	MyRectF BoundingBox;

	bool Dirty = true;
	bool IdMapDirty = true;
	bool UIDRun = true;
private:

	//Order is important
	Texture FBOBackgroundTexture;
	FrameBufferObject FBOBackground;

	void RenderBackground();

	Texture FBOPathStencileDepthTexture;
	Texture FBOPathColorTexture;
	FrameBufferObject FBOPath;

	Texture FBOIDTexture;
	FrameBufferObject FBOID;

	std::array<GLenum, 1> DrawBuffer0 = { GL_COLOR_ATTACHMENT0 };
	std::array<GLenum, 2> DrawBuffer1 = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	struct PathVAOs {
		VertexArrayObject EdgesVAO;
		VertexArrayObject EdgesMarkedVAO;

		VertexArrayObject EdgesUnmarkedVAO;
		VertexArrayObject SpecialPointsVAO;
		VertexArrayObject VertsVAO;

		VertexArrayObject ConflictPointsVAO;
	};

	PathVAOs VAOsPath;
	PathVAOs VAOsPathPreview;

	PathVAOs& GetPathVAOs(bool Preview);

	VertexArrayObject SevenSegVAO;
	VertexArrayObject SixteenSegVAO;

	VertexArrayObject MuxVAO;

#ifdef RenderCollisionGrid
	VertexArrayObject CollisionGridVAO;
#endif
	VertexArrayObject BlocksVAO;
	VertexArrayObject PinVAO;
	VertexArrayObject RoundPinVAO;

	VertexArrayObject AndVAO;
	VertexArrayObject OrVAO;
	VertexArrayObject XOrVAO;
	//VertexArrayObject NotTriangleVAO;
	//VertexArrayObject NDotVAO;
	VertexArrayObject AreaSelectVAO;

#ifdef ShowBasePositionOfBlocks
	VertexArrayObject BlockBasePotitionVAO;
#endif

	BufferedVertexVec<PointFRGBVertex> AreaSelectVerts;

	PointType MouseIndex;

	template<typename VertexType>
	static VertexArrayObject CreateVAO();

	//static VertexArrayObject CreateTwoPointIRGBVAO();

	//static VertexArrayObject CreatePointIVAO();

	//static VertexArrayObject CreateSevenSegVAO();
	//static VertexArrayObject CreateSixteenSegVAO();

	//static VertexArrayObject CreateMuxVAO();

	//static VertexArrayObject CreateBlockRGBAVAO();

	//static VertexArrayObject CreatePointIOrientationRGBVAO();
	//static VertexArrayObject CreatePointIOrientationVAO();

	//static VertexArrayObject CreatePointIRGBIDVAO();
	//static VertexArrayObject CreatePointFRGBVAO();
	//static VertexArrayObject CreateTextVAO();

	VertexArrayObject StaticTextVAO;
	VertexArrayObject DynamicTextVAO;
	//Must be befor CharMap
	Texture TextAtlas;

public:
	unsigned int FrameCount = 0;

	std::string title;

	bool AllowHover = true;
private:
	void RenderWires();

	public:

	void Render();

	void RenderPlacholder();

private:
	void RenderIDMap();

public:
	void UpdateViewProjectionMatrix(bool OnlyForUniforms = false);

	Eigen::Vector2i CoordToNearestPoint(Eigen::Vector2f Pos);

	constexpr static const float ZoomDotsBorder = 0.03f;
	bool ShowDots = true;
	void CheckZoomDots();

	Renderer(MyApp* App, MyFrame* Frame);
	~Renderer();

	void UpdateSize(bool Initilized);

	BufferedVertexVec<PointFRGBVertex>& GetAreaSelectVerts();

private:

public:

	unsigned int GetHighlited(const Eigen::Vector2f& MousePos);

	void UpdateMouseIndex(const PointType& MouseIndex);

private:
	std::unordered_map<CompressedBlockDataIndex, std::array<MyRectF, 4>> BlockBoundingBoxes;

	using BBUpdateFunc = std::function<void(void)>;
	std::mutex BBUpdateFuncsMutex;
	std::queue<BBUpdateFunc> BBUpdateFuncs;

	bool Terminating = false;
	std::thread BBUpdater;
	std::mutex BBUpdaterCVMutex;
	std::condition_variable BBUpdaterCV;

	void BBUpdaterWork();

public:
	const std::array<MyRectF, 4>& GetBlockBoundingBoxes(const CompressedBlockDataIndex& cbdi);

private:
	double GoHomeZoom = 0.01;
	Eigen::Vector2f GoHomeOffset = { 0,0 };
public:
	int GoHomeFrame = 0;
	void StartGoHome();

	//Returns if finished
	bool StepGoHome();
};
