#include "pch.hpp"

#include "Renderer.hpp"

#include "MyApp.hpp"

#include "ShaderManager.hpp"
#include "MyFrame.hpp"

#include "RenderTextUtility.hpp"

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
			ShaderManager::HighlightPath,
			ShaderManager::HighlightPathCornerOverdraw,
			ShaderManager::IntersectionPath}) {
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

void Renderer::Render() {
	PROFILE_FUNKTION;
	
	if (!Dirty && !ShaderManager::IsDirty) return;
	Dirty = false;

	if (ShaderManager::IsDirty) {
		UpdateViewProjectionMatrix();
		RenderBackground();
		ShaderManager::IsDirty = false;
	}

	Frame->Canvas->SetCurrent(*App->GlContext.get());

	GLCALL(glStencilMask(0xFF));

	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	GLCALL(glStencilMask(0x00));

	GLCALL(glViewport(0, 0, CanvasSize.x(), CanvasSize.y()));

	FBOBackground.bind(FrameBufferObject::Read);

	GLCALL(glBlitFramebuffer(
		0, 0,
		CanvasSize.x(), CanvasSize.y(),
		0, 0,
		CanvasSize.x(), CanvasSize.y(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	));

	FBOBackground.unbind();

	//LinesVAO->bind();


	//GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA));


	//GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

	VisualBlock& b = *Frame->CurrentBlock;


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

			EdgesMarkedVAO.bind();
			//EdgesMarkedVAO.ReplaceVertexBuffer(edgesMarked, 1);
			EdgesMarkedVAO.ReplaceVertexBuffer(b.GetEdgesMarked(Preview), 1);
			EdgesMarkedVAO.DrawAs(GL_TRIANGLE_STRIP);
			EdgesMarkedVAO.unbind();

			GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
			GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
			GLCALL(glStencilMask(0x00));

			VertsVAO.bind();
			VertsVAO.ReplaceVertexBuffer(b.GetVerts(Preview), 1);
			VertsVAO.DrawAs(GL_TRIANGLE_STRIP);

			PathHightlightShader->unbind();


			const auto& HighlightPathCornerOverdrawShader = ShaderManager::GetShader(ShaderManager::HighlightPathCornerOverdraw);

			HighlightPathCornerOverdrawShader->bind();

			FBOBackground.GetTexture()->bind(HighlightPathCornerOverdrawShader.get(), "UBackground", "", 0);

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

#ifdef RenderCollisionGrid
	const auto& BlockShader = ShaderManager::GetShader(ShaderManager::Block);

	BlockShader->bind();

	std::vector<RectRGBAVertex> Blocks;

	static std::unordered_map<int, std::tuple<float, float, float>> ColourMap;
	auto GetColour = [&](const int& i) {
		if (i == 0) return std::make_tuple(1.0f, 1.0f, 1.0f);
		auto it = ColourMap.find(i);
		if (it == ColourMap.end()) {
			ColourMap[i] = std::make_tuple(float(rand())/ (float)RAND_MAX, float(rand()) / (float)RAND_MAX, float(rand()) / (float)RAND_MAX);
			return ColourMap[i];
		}
		return it->second;
		};

	/*Blocks.emplace_back(0, 0, MouseIndex.x(), MouseIndex.y(),
		1, 0, 0, 0.5);*/

		//GLCALL(glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA));

	for (const auto& pair : b.CollisionMap) {
		MyRectI BB = MyRectI::FromCorners(pair.first, pair.first + PointType{ VisualBlock::BoxSize,VisualBlock::BoxSize });
		if (BoundingBox.Intersectes(BB)) {
			const auto& size = pair.second.size();
			if (size == 0) {
				Blocks.emplace_back(
					BB.Position.x(),
					BB.Position.y(),
					BB.Position.x() + BB.Size.x(),
					BB.Position.y() + BB.Size.y(),
					0.5, 0.5, 0.5, 0.1);
			}
			else {
				auto [r, g, b] = GetColour(size);
				Blocks.emplace_back(
					BB.Position.x(),
					BB.Position.y(),
					BB.Position.x() + BB.Size.x(),
					BB.Position.y() + BB.Size.y(),
					r, g, b, 0.5);
			}
		}
	}



	BlocksVAO.bind();
	BlocksVAO.ReplaceVertexBuffer(Blocks, 1);
	BlocksVAO.DrawAs(GL_TRIANGLE_STRIP);
	BlocksVAO.unbind();

	//GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


	BlockShader->unbind();
#endif

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
	FBOBackground(&FBOBackgroundTexture),
#ifdef RenderCollisionGrid
	BlocksVAO(CreateBlockRGBAVAO()),
#endif
	EdgesVAO(CreatePathVertexVAO()),
	EdgesMarkedVAO(CreatePathVertexVAO()),
	EdgesUnmarkedVAO(CreatePathVertexVAO()),
	VertsVAO(CreatePathVertexVAO()), 
	SpecialPointsVAO(CreatePointVertexVAO())
{
}

void Renderer::UpdateSize() {
	PROFILE_FUNKTION;

	auto Size = Frame->Canvas->GetSize();
	CanvasSize = { Size.x,Size.y };

	FBOBackgroundTexture.Resize(CanvasSize.x(), CanvasSize.y());

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




