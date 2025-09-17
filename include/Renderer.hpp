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
	Eigen::Vector2f CanvasSize = { 1, 1 };
	Eigen::Matrix3f ViewProjectionMatrix;

	MyRectF BoundingBox;

	bool Dirty = true;
	bool IdMapDirty = true;
private:

	//Order is important
	Texture FBOBackgroundTexture;
	FrameBufferObject FBOBackground;

	void RenderBackground();

	Texture FBOPathColorTexture;
	FrameBufferObject FBOPath;

	Texture FBOMainStencileDepthTexture;
	Texture FBOMainColorTexture;
	FrameBufferObject FBOMain;

	Texture FBOIDTexture;
	FrameBufferObject FBOID;

	Texture FBOBlurHighlightTexture;
	Texture FBOBlurHighlightStencileDepthTexture;
	FrameBufferObject FBOBlurHighlight;

	Texture FBOBlurPreviewTexture;
	Texture FBOBlurPreviewStencileDepthTexture;
	FrameBufferObject FBOBlurPreview;

	Texture FBOBlurMarkedTexture;
	Texture FBOBlurMarkedStencileDepthTexture;
	FrameBufferObject FBOBlurMarked;

	//std::array<GLenum, 2> DrawBuffer0 = { GL_COLOR_ATTACHMENT0, GL_NONE };
	//std::array<GLenum, 2> DrawBuffer1 = { GL_NONE, GL_COLOR_ATTACHMENT1 };

	struct PathVAOs {
		VertexArrayObject EdgesVAO;
		VertexArrayObject VertsVAO;
		VertexArrayObject IntersectionPointsVAO;
	};

	PathVAOs VAOsPath;
	PathVAOs VAOsPathPreview;
	PathVAOs VAOsPathHighlighted;
	PathVAOs VAOsPathMarked;

#ifdef RenderCollisionGrid
	VertexArrayObject CollisionGridVAO;
#endif
	VertexArrayObject AssetVAO;
	VertexArrayObject PinVAO;
	VertexArrayObject RoundPinVAO;

	VertexArrayObject HighlightAssetVAO;
	VertexArrayObject MarkedAssetVAO;

	VertexArrayObject AreaSelectVAO;

#ifdef ShowBoundingBoxes
	VertexArrayObject BBVAO;
#endif

#ifdef ShowBasePositionOfBlocks
	VertexArrayObject BlockBasePositionVAO;
#endif

	BufferedVertexVec<AssetFVertex> AreaSelectVerts;

	PointType MouseIndex;

	template<typename VertexType>
	static VertexArrayObject CreateVAOInstancing4();

	template<typename VertexType>
	static VertexArrayObject CreateVAO();

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

	static void RenderPlacholder(MyFrame& frame, Eigen::Vector2f CanvasSize);

private:
	void RenderIDMap();

public:
	void UpdateViewProjectionMatrix(bool OnlyForUniforms = false);

	Eigen::Vector2i CoordToNearestPoint(Eigen::Vector2f Pos);

	constexpr static const float ZoomDotsBorder = 0.03f;
	bool ShowDots = true;
	void CheckZoomDots();

	private:
	static Texture CreateBlurTexture() {
		Texture::Descriptor desc;
		desc.Format = GL_RED_INTEGER;
		desc.Internal_Format = GL_R32UI;
		desc.Depth_Stencil_Texture_Mode = GL_STENCIL_INDEX;
		desc.Type = GL_UNSIGNED_INT;
		desc.Wrap_R = Texture::Descriptor::CLAMP_TO_BORDER;
		desc.Wrap_S = Texture::Descriptor::CLAMP_TO_BORDER;
		desc.Wrap_T = Texture::Descriptor::CLAMP_TO_BORDER;
		desc.Min_Filter = Texture::Descriptor::NEAREST;
		desc.Mag_Filter = Texture::Descriptor::NEAREST;
		return Texture(1, 1, nullptr, desc);
	}

	static Texture CreateBlurStencileDepthTexture() {
		Texture::Descriptor desc;
		desc.Format = GL_DEPTH_STENCIL;
		desc.Internal_Format = GL_DEPTH24_STENCIL8;
		desc.Depth_Stencil_Texture_Mode =
			GL_STENCIL_INDEX;
		desc.Type = GL_UNSIGNED_INT_24_8;
		return Texture(1, 1, nullptr, desc);
	}

	public:

	Renderer(MyApp* App, MyFrame* Frame);
	~Renderer();

	void UpdateSize();

	BufferedVertexVec<AssetFVertex>& GetAreaSelectVerts();

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
