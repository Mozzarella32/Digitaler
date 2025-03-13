#include "pch.hpp"

#include "Renderer.hpp"

#include "MyApp.hpp"

#include "ShaderManager.hpp"
#include "MyFrame.hpp"

#include "RenderTextUtility.hpp"

#include "DataResourceManager.hpp"

void Renderer::RenderBackground() {
	PROFILE_FUNKTION;

	App->ContextBinder->BindContext(App->GlContext.get());

	FBOBackground.bind(FrameBufferObject::Draw);

	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, CanvasSize.x(), CanvasSize.y());

	GLCALL(glDisable(GL_DEPTH_TEST));
	const auto& BackgroundShader = ShaderManager::GetShader(ShaderManager::Background);

	BackgroundShader->bind();
	//GLCALL(glUniform1f(BackgroundShader->GetLocation("UTime"), (FrameCount % 100) /100.0));

	App->HoleScreenVAO->bind();

	App->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);

	App->HoleScreenVAO->unbind();

	BackgroundShader->unbind();

	FBOBackground.unbind();
}

void Renderer::UpdateViewProjectionMatrix() {
	PROFILE_FUNKTION;

	Eigen::Matrix3f scaleMatrix = Eigen::Matrix3f::Identity();
	scaleMatrix(0, 0) = Zoom * CanvasSize.x();
	scaleMatrix(1, 1) = Zoom * CanvasSize.y();

	Eigen::Matrix3f translationMatrix = Eigen::Matrix3f::Identity();
	translationMatrix(0, 2) = -Offset.x();
	translationMatrix(1, 2) = -Offset.y();

	ViewProjectionMatrix = translationMatrix * scaleMatrix;

	App->ContextBinder->BindContext(App->GlContext.get());
	for (const ShaderManager::Shaders shaderName : {
		ShaderManager::Background,
			ShaderManager::Path,
			ShaderManager::Block,
			ShaderManager::Pin,
			ShaderManager::RoundPin,
			ShaderManager::HighlightPath,
			ShaderManager::HighlightPathCornerOverdraw,
			ShaderManager::IntersectionPath,
			ShaderManager::SevenSeg,
			ShaderManager::SixteenSeg,
			ShaderManager::Mux
	}) {
		const auto& Shader = ShaderManager::GetShader(shaderName);
		Shader->bind();
		GLCALL(glUniformMatrix3fv(Shader->GetLocation("UViewProjectionMatrix"), 1, GL_FALSE, ViewProjectionMatrix.data()));
		GLCALL(glUniform2f(Shader->GetLocation("UOffset"), Offset.x(), Offset.y()));
		GLCALL(glUniform2f(Shader->GetLocation("UZoom"), Zoom * CanvasSize.x(), Zoom * CanvasSize.y()));
		Shader->unbind();
	}

	auto UpperLeft = ViewProjectionMatrix * Eigen::Vector3f{ -1,-1,1 };
	auto LowerRight = ViewProjectionMatrix * Eigen::Vector3f{ 1,1,1 };

	Eigen::Vector2i Corner1{ UpperLeft.x(), UpperLeft.y() };
	Eigen::Vector2i Corner2{ LowerRight.x(), LowerRight.y() };

	Corner1 -= Eigen::Vector2i{ 2, 2 };
	Corner2 += Eigen::Vector2i{ 2, 2 };

	BoundingBox = MyRectI::FromCorners(Corner1, Corner2);

	RenderBackground();
}

Eigen::Vector2i Renderer::CoordToNearestPoint(Eigen::Vector2f Pos) {
	//PROFILE_FUNKTION;

	return Eigen::Vector2i(
		static_cast<int>(std::round(Pos.x())),
		static_cast<int>(std::round(Pos.y()))
	);
}

VertexArrayObject Renderer::CreateTwoPointIVertexVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, TwoPointIVertex{ 0,0,0,0 },1 }
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

VertexArrayObject Renderer::CreatePointIVertexVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, PointIVertex{ 0,0 },1 }
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

VertexArrayObject Renderer::CreateSevenSegVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, SevenSegVertex{0,0,0,0,0,0,0},1}
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

VertexArrayObject Renderer::CreateSixteenSegVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, SixteenSegVertex{0,0,0,0,0,0,0},1}
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

VertexArrayObject Renderer::CreateMuxVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, MuxVertex{0,0,0,0,0,0,0,0},1}
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
VertexArrayObject Renderer::CreateBlockRGBAVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, TwoPointIRGBAVertex{ 0,0,0,0, 0,0,0,0 },1 }
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

VertexArrayObject Renderer::CreatePointIOrientationRGBVertexVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, PointIOrientationRGBVertex{0,0, 0, 0,0,0},1 }
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

VertexArrayObject Renderer::CreatePointIRGBVertex() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, PointIRGBVertex{0,0, 0,0,0},1 }
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

void Renderer::Render() {
	PROFILE_FUNKTION;

	if (!Dirty && !ShaderManager::IsDirty) return;
	Dirty = false;

	if (ShaderManager::IsDirty) {
		UpdateViewProjectionMatrix();
		RenderBackground();
		ShaderManager::IsDirty = false;
	}

	VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;

	/*const auto& MetadataOpt = b.GetBlockMetadata({ Frame->BlockManager->GetBlockIndex("ContainedBlock"),4 });
	if (MetadataOpt) {
		auto Metadata = MetadataOpt.value();
		Metadata.Pos = MouseIndex;
		b.SetBlockMetadata({ Frame->BlockManager->GetBlockIndex("ContainedBlock"),4 }, Metadata);
	}*/

	/*App->ContextBinder->BindContext(App->GlContext.get());

	FBOPath.bind(FrameBufferObject::Draw);

	GLCALL(glStencilMask(0xFF));

	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GLCALL(glViewport(0, 0, CanvasSize.x(), CanvasSize.y()));*/





	//FBOPath.unbind();


	Frame->Canvas->SetCurrent(*App->GlContext.get());

	GLCALL(glStencilMask(0xFF));

	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GLCALL(glViewport(0, 0, CanvasSize.x(), CanvasSize.y()));

	FBOBackground.bind(FrameBufferObject::Read);
	//FBOPath.bind(FrameBufferObject::Read);

	GLCALL(glBlitFramebuffer(
		0, 0,
		CanvasSize.x(), CanvasSize.y(),
		0, 0,
		CanvasSize.x(), CanvasSize.y(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	));

	FBOBackground.unbind();

	//const auto& CopyTextureShader = ShaderManager::GetShader(ShaderManager::CopyTexture);
	//CopyTextureShader->bind();

	////GLCALL(glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA));

	//FBOPathColorTexture.bind(CopyTextureShader.get(), "UTexture", "", 0);
	//App->HoleScreenVAO->bind();
	//App->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
	//App->HoleScreenVAO->unbind();

	////GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	//CopyTextureShader->unbind();

	std::vector<bool> PreviewNonPreview = { false };
	if (b.HasPreview()) PreviewNonPreview.push_back(true);

	//FBOBackground.GetTexture()->bind(PathHightlightShader.get(),"UBackground","",0);
	for (bool Preview : PreviewNonPreview) {
		if (Preview) {
			GLCALL(glStencilMask(0xFF));
			GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
			b.UpdateVectsForVAOsPreview(BoundingBox, MouseIndex);
		}
		else {
			b.UpdateVectsForVAOs(BoundingBox, MouseIndex);
		}

		const auto& PathHightlightShader = ShaderManager::GetShader(ShaderManager::HighlightPath);

		PathHightlightShader->bind();
		GLCALL(glUniform3f(PathHightlightShader->GetLocation("UColor"), 1.0, 1.0, 0.0));
		FBOBackgroundTexture.bind(PathHightlightShader.get(), "UBackground", "", 0);

		GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
		GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
		GLCALL(glStencilMask(0xFF));

		if (b.hasUnmarked(Preview)) {
			GLCALL(glBlendFunc(GL_ONE, GL_ZERO));
			GLCALL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));

			EdgesUnmarkedVAO.bind();
			EdgesUnmarkedVAO.ReplaceVertexBuffer(b.GetEdgesUnmarked(Preview), 1);
			EdgesUnmarkedVAO.DrawAs(GL_TRIANGLE_STRIP);
			EdgesUnmarkedVAO.unbind();

			GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
			GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

			if (!b.hasMark(Preview)) {
				PathHightlightShader->unbind();

				GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
				GLCALL(glStencilMask(0x00));
				GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
			}
		}

		if (b.hasMark(Preview)) {

			/*GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
			GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
			GLCALL(glStencilMask(0xFF));*/

			/*auto edgesMarked = b.GetEdgesMarked(Preview);

			if (!Preview) {
				for (const auto& BB : b.GetBoundingBox()) {
					edgesMarked.push_back(
						{
							BB.Position.x(),
							BB.Position.y(),
							BB.Position.x() + BB.Size.x(),
							BB.Position.y() + BB.Size.y() });
				}
			}*/

			//GLCALL(glBlendFunc(GL_ONE, GL_ZERO));

			EdgesMarkedVAO.bind();
			//EdgesMarkedVAO.ReplaceVertexBuffer(edgesMarked, 1);
			EdgesMarkedVAO.ReplaceVertexBuffer(b.GetEdgesMarked(Preview), 1);
			EdgesMarkedVAO.DrawAs(GL_TRIANGLE_STRIP);
			EdgesMarkedVAO.unbind();

			//GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

			GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
			GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
			GLCALL(glStencilMask(0x00));

			VertsVAO.bind();
			VertsVAO.ReplaceVertexBuffer(b.GetVerts(Preview), 1);
			VertsVAO.DrawAs(GL_TRIANGLE_STRIP);

			PathHightlightShader->unbind();

			const auto& HighlightPathCornerOverdrawShader = ShaderManager::GetShader(ShaderManager::HighlightPathCornerOverdraw);

			HighlightPathCornerOverdrawShader->bind();

			GLCALL(glUniform3f(HighlightPathCornerOverdrawShader->GetLocation("UColor"), 1.0, 1.0, 0.0));

			FBOBackgroundTexture.bind(HighlightPathCornerOverdrawShader.get(), "UBackground", "", 0);

			GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
			GLCALL(glStencilFunc(GL_EQUAL, 2, 0xFF));
			GLCALL(glBlendFunc(GL_ONE, GL_ZERO));

			VertsVAO.DrawAs(GL_TRIANGLE_STRIP);
			VertsVAO.unbind();

			HighlightPathCornerOverdrawShader->unbind();

			GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
			GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		}

		const auto& PathShader = ShaderManager::GetShader(ShaderManager::Path);

		PathShader->bind();

		EdgesVAO.bind();
		EdgesVAO.ReplaceVertexBuffer(b.GetEdges(Preview), 1);
		EdgesVAO.DrawAs(GL_TRIANGLE_STRIP);
		EdgesVAO.unbind();

		PathShader->unbind();

		const auto& IntersectionPathShader = ShaderManager::GetShader(ShaderManager::IntersectionPath);

		IntersectionPathShader->bind();


		GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
		GLCALL(glStencilFunc(GL_LEQUAL, 2, 0xFF));
		//GLCALL(glBlendFunc(GL_ONE, GL_ZERO));

		SpecialPointsVAO.bind();
		SpecialPointsVAO.ReplaceVertexBuffer(b.GetSpecialPoints(Preview), 1);
		SpecialPointsVAO.DrawAs(GL_TRIANGLE_STRIP);
		SpecialPointsVAO.unbind();

		IntersectionPathShader->unbind();

		GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
		GLCALL(glStencilMask(0x00));
		//GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}

	//FBOPath.unbind();

	//LinesVAO->bind();



	//GLCALL(glClearColor(0.0, 0.0, 0.0, 0.0));


	//Just fo rthe Ranged Based for loop

	//FBOPath.unbind();

	//Frame->Canvas->SetCurrent(*App->GlContext.get());

	//GLCALL(glStencilMask(0xFF));

	//GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//GLCALL(glStencilMask(0x00));

	//GLCALL(glViewport(0, 0, CanvasSize.x(), CanvasSize.y()));

	//FBOBackground.bind(FrameBufferObject::Read);

	//FBOPath.bind(FrameBufferObject::Read);

	//GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA));


	//GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

	GLCALL(glStencilMask(0xFF));
	GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
	GLCALL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
	GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
	GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));

	const auto& PathShader = ShaderManager::GetShader(ShaderManager::Path);
	PathShader->bind();
	for (bool Preview : PreviewNonPreview) {

		//GLCALL(glUniform3f(PathHightlightShader->GetLocation("UColor"), 1.0, 1.0, 0.0));
		//FBOBackgroundTexture.bind(PathHightlightShader.get(), "UBackground", "", 0);

		EdgesVAO.bind();
		EdgesVAO.ReplaceVertexBuffer(b.GetEdges(Preview), 1);
		EdgesVAO.DrawAs(GL_TRIANGLE_STRIP);
		EdgesVAO.unbind();

	}
	PathShader->unbind();

	GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
	GLCALL(glStencilMask(0x00));

	const auto& PinShader = ShaderManager::GetShader(ShaderManager::Pin);

	PinShader->bind();

	/*std::vector<PointIOrientationRGBVertex> PinVerts;

	PinVerts.emplace_back(-1, 1, 2, 0, 0.5, 1);
	PinVerts.emplace_back(-1, 2, 2, 0, 0.5, 1);
	PinVerts.emplace_back(1, -1, 3, 0, 0.5, 1);
	PinVerts.emplace_back(3, 0, 0, 0, 0.5, 1);
	PinVerts.emplace_back(3, 2, 0, 0, 0.5, 1);
	PinVerts.emplace_back(1, 4, 1, 0, 0.5, 1);*/

	const auto& Pins = b.GetPinVerts();

	PinVAO.bind();
	PinVAO.ReplaceVertexBuffer(Pins, 1);

	GLCALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
	GLCALL(glUniform1i(PinShader->GetLocation("UHasWire"), true));
	PinVAO.DrawAs(GL_TRIANGLE_STRIP);

	GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
	GLCALL(glUniform1i(PinShader->GetLocation("UHasWire"), false));
	PinVAO.DrawAs(GL_TRIANGLE_STRIP);

	PinVAO.unbind();

	PinShader->unbind();

	const auto& RoundPinShader = ShaderManager::GetShader(ShaderManager::RoundPin);

	RoundPinShader->bind(); 

	std::vector<PointIRGBVertex> RoundPinVerts;

	RoundPinVerts.emplace_back(0,-1,1.0,0.0,0.0);
	RoundPinVerts.emplace_back(3, -1, 1.0, 0.0, 0.0);

	RoundPinVAO.bind();
	RoundPinVAO.ReplaceVertexBuffer(RoundPinVerts, 1);

	GLCALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
	GLCALL(glUniform1i(RoundPinShader->GetLocation("UHasWire"), true));
	RoundPinVAO.DrawAs(GL_TRIANGLE_STRIP);

	GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
	GLCALL(glUniform1i(RoundPinShader->GetLocation("UHasWire"), false));
	RoundPinVAO.DrawAs(GL_TRIANGLE_STRIP);

	GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
	GLCALL(glStencilMask(0x00));

	RoundPinVAO.unbind();

	RoundPinShader->unbind();


	GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
	GLCALL(glStencilMask(0x00));

	const auto& BlockShader = ShaderManager::GetShader(ShaderManager::Block);

	BlockShader->bind();

	const auto& Blocks = b.GetBlockVerts();

	//Blocks.emplace_back(0, 0, 2, -3, 1.0, 0.5, 0.5, 1.0);

	BlocksVAO.bind();
	BlocksVAO.ReplaceVertexBuffer(Blocks, 1);
	BlocksVAO.DrawAs(GL_TRIANGLE_STRIP);
	BlocksVAO.unbind();

	//#ifdef RenderCollisionGrid
	//#define USETHIS
#ifdef USETHIS

	Blocks.clear();
	static std::unordered_map<int, std::tuple<float, float, float>> ColourMap;
	auto GetColour = [&](const int& i) {
		if (i == 0) return std::make_tuple(1.0f, 1.0f, 1.0f);
		auto it = ColourMap.find(i);
		if (it == ColourMap.end()) {
			ColourMap[i] = std::make_tuple(float(rand()) / (float)RAND_MAX, float(rand()) / (float)RAND_MAX, float(rand()) / (float)RAND_MAX);
			return ColourMap[i];
		}
		return it->second;
		};

	/*Blocks.emplace_back(0, 0, MouseIndex.x(), MouseIndex.y(),
		1, 0, 0, 0.5);*/

		//GLCALL(glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA));

	for (const auto& pair : b.CollisionMap) {
		MyRectI BB = MyRectI::FromCorners(pair.first, pair.first + PointType{ VisualBlockInterior::BoxSize,VisualBlockInterior::BoxSize });
		if (BoundingBox.Intersectes(BB)) {
			const auto& size = pair.second.size();
			if (size == 0) {
				Blocks.emplace_back(
					BB.Position.x(),
					BB.Position.y(),
					BB.Position.x() + BB.Size.x() - 1,
					BB.Position.y() + BB.Size.y() - 1,
					0.5, 0.5, 0.5, 0.1);
			}
			else {
				auto [r, g, b] = GetColour(size);
				Blocks.emplace_back(
					BB.Position.x(),
					BB.Position.y(),
					BB.Position.x() + BB.Size.x() - 1,
					BB.Position.y() + BB.Size.y() - 1,
					r, g, b, 0.5);
			}
		}
	}
	//Blocks.emplace_back(0, 0, 2, 3,0.5,0.5,0.5,0.1);



	BlocksVAO.bind();
	BlocksVAO.ReplaceVertexBuffer(Blocks, 1);
	BlocksVAO.DrawAs(GL_TRIANGLE_STRIP);
	BlocksVAO.unbind();



	//GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


#endif

	BlockShader->unbind();

	const auto& SevenSegShader = ShaderManager::GetShader(ShaderManager::SevenSeg);

	SevenSegShader->bind();

	/*std::vector<SegmentDisplayVertex> SevenSegVerts;

	SevenSegVerts.emplace_back(-1, 1, 2, 0, 0.5, 1);
	SevenSegVerts.emplace_back(-1, 2, 2, 0, 0.5, 1);
	SevenSegVerts.emplace_back(1, -1, 3, 0, 0.5, 1);
	SevenSegVerts.emplace_back(3, 0, 0, 0, 0.5, 1);
	SevenSegVerts.emplace_back(3, 2, 0, 0, 0.5, 1);
	SevenSegVerts.emplace_back(1, 4, 1, 0, 0.5, 1);*/

	const auto& SevenSegs = b.GetSevenSegVerts();

	SevenSegVAO.bind();
	SevenSegVAO.ReplaceVertexBuffer(SevenSegs, 1);
	SevenSegVAO.DrawAs(GL_TRIANGLE_STRIP);
	SevenSegVAO.unbind();

	SevenSegShader->unbind();

	const auto& SixteenSegShader = ShaderManager::GetShader(ShaderManager::SixteenSeg);

	SixteenSegShader->bind();

	/*std::vector<SegmentDisplayVertex> SixteenSegVerts;

	SixteenSegVerts.emplace_back(-1, 1, 2, 0, 0.5, 1);
	SixteenSegVerts.emplace_back(-1, 2, 2, 0, 0.5, 1);
	SixteenSegVerts.emplace_back(1, -1, 3, 0, 0.5, 1);
	SixteenSegVerts.emplace_back(3, 0, 0, 0, 0.5, 1);
	SixteenSegVerts.emplace_back(3, 2, 0, 0, 0.5, 1);
	SixteenSegVerts.emplace_back(1, 4, 1, 0, 0.5, 1);*/

	const auto& SixteenSegs = b.GetSixteenSegVerts();

	SixteenSegVAO.bind();
	SixteenSegVAO.ReplaceVertexBuffer(SixteenSegs, 1);
	SixteenSegVAO.DrawAs(GL_TRIANGLE_STRIP);
	SixteenSegVAO.unbind();

	SixteenSegShader->unbind();

	const auto& MuxShader = ShaderManager::GetShader(ShaderManager::Mux);

	MuxShader->bind();

	std::vector<MuxVertex> MuxVerts;

	int MuxSize = 2;
	int MuxSelected = 0;

	MuxVerts.emplace_back(PointType{ 0,30 }, MyDirektion::Up, MuxSize, MuxSelected, ColourType{ 1,1,1,1 });
	MuxVerts.emplace_back(PointType{ 5,30 }, MyDirektion::Right, MuxSize, MuxSelected, ColourType{ 1,1,1,1 });
	MuxVerts.emplace_back(PointType{ 10,30 }, MyDirektion::Down, MuxSize, MuxSelected, ColourType{ 1,1,1,1 });
	MuxVerts.emplace_back(PointType{ 15,30 }, MyDirektion::Left, MuxSize, MuxSelected, ColourType{ 1,1,1,1 });

	MuxVAO.bind();
	MuxVAO.ReplaceVertexBuffer(MuxVerts, 1);
	MuxVAO.DrawAs(GL_TRIANGLE_STRIP);
	MuxVAO.unbind();

	MuxShader->unbind();

	Frame->Canvas->SwapBuffers();
}

void Renderer::CheckZoomDots() {

	bool Update = false;

	if (Zoom < ZoomDotsBorder && !ShowDots) {
		Update = true;
		ShowDots = true;
	}
	else if (Zoom >= ZoomDotsBorder && ShowDots) {
		Update = true;
		ShowDots = false;
	}

	if (!Update) return;

	App->ContextBinder->BindContext(App->GlContext.get());

	const auto& BackgroundShader = ShaderManager::GetShader(ShaderManager::Background);

	BackgroundShader->bind();
	GLCALL(glUniform1ui(BackgroundShader->GetLocation("UShowDots"), ShowDots));
	BackgroundShader->unbind();
}

Renderer::Renderer(MyApp* App, MyFrame* Frame)
	:App(App), Frame(Frame),
	FBOBackgroundTexture(1, 1),
	FBOBackground({ &FBOBackgroundTexture }),
	FBOPathStencileDepthTexture(1, 1, nullptr, []() {
	Texture::Descriptor desc;
	desc.Format = GL_DEPTH_STENCIL;
	desc.Internal_Format = GL_DEPTH24_STENCIL8;
	desc.Depth_Stencil_Texture_Mode = GL_STENCIL_INDEX;
	desc.Type = GL_UNSIGNED_INT_24_8;
	return desc;
		}()),
	FBOPathColorTexture(1, 1),
	FBOPath({ &FBOPathColorTexture,&FBOPathStencileDepthTexture }, { GL_COLOR_ATTACHMENT0,GL_DEPTH_STENCIL_ATTACHMENT }),
#ifdef RenderCollisionGrid
	BlocksVAO(CreateBlockRGBAVAO()),
#endif
	SevenSegVAO(CreateSevenSegVAO()),
	SixteenSegVAO(CreateSixteenSegVAO()),
	MuxVAO(CreateMuxVAO()),
	PinVAO(CreatePointIOrientationRGBVertexVAO()),
	RoundPinVAO(CreatePointIRGBVertex()),
	EdgesVAO(CreateTwoPointIVertexVAO()),
	EdgesMarkedVAO(CreateTwoPointIVertexVAO()),
	EdgesUnmarkedVAO(CreateTwoPointIVertexVAO()),
	VertsVAO(CreateTwoPointIVertexVAO()),
	SpecialPointsVAO(CreatePointIVertexVAO())
{
}

void Renderer::UpdateSize() {
	PROFILE_FUNKTION;

	auto Size = Frame->Canvas->GetSize();
	CanvasSize = { Size.x,Size.y };

	FBOBackgroundTexture.Resize(CanvasSize.x(), CanvasSize.y());

	FBOPathStencileDepthTexture.Resize(CanvasSize.x(), CanvasSize.y());
	FBOPathColorTexture.Resize(CanvasSize.x(), CanvasSize.y());

	UpdateViewProjectionMatrix();
	Dirty = true;
	Render();
}

void Renderer::UpdateMouseIndex(const PointType& MouseIndex) {
	App->ContextBinder->BindContext(App->GlContext.get());

	const auto& BackgroundShader = ShaderManager::GetShader(ShaderManager::Background);

	BackgroundShader->bind();
	GLCALL(glUniform2f(BackgroundShader->GetLocation("UMousePos"), MouseIndex.x(), MouseIndex.y()));

	RenderBackground();

	this->MouseIndex = MouseIndex;

	Dirty = true;
	Render();
}




