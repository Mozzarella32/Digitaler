#include "pch.hpp"

#include "Renderer.hpp"

#include "MyApp.hpp"

#include "ShaderManager.hpp"
#include "PngManager.hpp"
#include "MyFrame.hpp"

#include "RenderTextUtility.hpp"

#include "DataResourceManager.hpp"

void Renderer::RenderBackground() {
	PROFILE_FUNKTION;

	App->ContextBinder->BindContext(App->GlContext.get());

	FBOBackground.bind(FrameBufferObject::Draw);

	glClear(GL_COLOR_BUFFER_BIT /* | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT */);
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

void Renderer::UpdateViewProjectionMatrix(bool OnlyForUniforms) {
	PROFILE_FUNKTION;

	PROFILE_SCOPE_ID_START("Calc", 0);
	Eigen::Matrix3f scaleMatrix = Eigen::Matrix3f::Identity();
	scaleMatrix(0, 0) = Zoom * CanvasSize.x();
	scaleMatrix(1, 1) = Zoom * CanvasSize.y();

	Eigen::Matrix3f translationMatrix = Eigen::Matrix3f::Identity();
	translationMatrix(0, 2) = -Offset.x();
	translationMatrix(1, 2) = -Offset.y();

	ViewProjectionMatrix = translationMatrix * scaleMatrix;
	PROFILE_SCOPE_ID_END(0);

	App->ContextBinder->BindContext(App->GlContext.get());

	PROFILE_SCOPE_ID_START("UZoom", 1);
	for (const auto& [shader, Location] : ShaderManager::GetShadersWithUniform("UZoom")) {
		auto& Shader = *ShaderManager::GetShader(shader);
		Shader.bind();
		GLCALL(glUniform2f(Location, Zoom * CanvasSize.x(), Zoom * CanvasSize.y()));
		Shader.unbind();
	}
	PROFILE_SCOPE_ID_END(1);

	PROFILE_SCOPE_ID_START("UViewProjectionMatrix", 2);
	for (const auto& [shader, Location] : ShaderManager::GetShadersWithUniform("UViewProjectionMatrix")) {
		auto& Shader = *ShaderManager::GetShader(shader);
		Shader.bind();
		GLCALL(glUniformMatrix3fv(Location, 1, GL_FALSE, ViewProjectionMatrix.data()));
		Shader.unbind();
	}
	PROFILE_SCOPE_ID_END(2);

	PROFILE_SCOPE_ID_START("UOffset", 3);
	for (const auto& [shader, Location] : ShaderManager::GetShadersWithUniform("UOffset")) {
		auto& Shader = *ShaderManager::GetShader(shader);
		Shader.bind();
		GLCALL(glUniform2f(Location, Offset.x(), Offset.y()));
		Shader.unbind();
	}
	PROFILE_SCOPE_ID_END(3);

	PROFILE_SCOPE_ID_START("UZoomFactor", 4);
	for (const auto& [shader, Location] : ShaderManager::GetShadersWithUniform("UZoomFactor")) {
		auto& Shader = *ShaderManager::GetShader(shader);
		Shader.bind();
		GLCALL(glUniform1f(Location, Zoom));
		Shader.unbind();
	}
	PROFILE_SCOPE_ID_END(4);

	if (OnlyForUniforms) return;

	PROFILE_SCOPE_ID_START("Calc BB", 5);

	auto UpperLeft = ViewProjectionMatrix * Eigen::Vector3f{ -1,-1,1 };
	auto LowerRight = ViewProjectionMatrix * Eigen::Vector3f{ 1,1,1 };

	Eigen::Vector2f Corner1{ UpperLeft.x(), UpperLeft.y() };
	Eigen::Vector2f Corner2{ LowerRight.x(), LowerRight.y() };

	BoundingBox = MyRectF::FromCorners(Corner1, Corner2);
	PROFILE_SCOPE_ID_END(5);

	RenderBackground();
}

Eigen::Vector2i Renderer::CoordToNearestPoint(Eigen::Vector2f Pos) {
	//PROFILE_FUNKTION;

	return Eigen::Vector2i(
		static_cast<int>(std::round(Pos.x())),
		static_cast<int>(std::round(Pos.y()))
	);
}


Renderer::PathVAOs& Renderer::GetPathVAOs(bool Preview) {
	return Preview ? VAOsPathPreview : VAOsPath;
}

VertexArrayObject Renderer::CreateTwoPointIVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, TwoPointIRGBVertex{},1 }
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

VertexArrayObject Renderer::CreatePointIVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, PointIVertex{},1 }
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
		{ GL_DYNAMIC_DRAW, SevenSegVertex{},1}
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
		{ GL_DYNAMIC_DRAW, SixteenSegVertex{},1}
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
		{ GL_DYNAMIC_DRAW, MuxIDVertex{},1}
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
		{ GL_DYNAMIC_DRAW, TwoPointIRGBAIDVertex{},1 }
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

VertexArrayObject Renderer::CreatePointIOrientationRGBVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, PointIOrientationRGBRHGHBHIDVertex{},1 }
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

VertexArrayObject Renderer::CreatePointIOrientationVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, PointIOrientationRGBIDVertex{},1 }
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

VertexArrayObject Renderer::CreatePointIRGBIDVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, PointIRGBIDVertex{},1 }
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

VertexArrayObject Renderer::CreatePointFRGBVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, PointFRGBVertex{},1 }
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

VertexArrayObject Renderer::CreateTextVAO() {
	VertexArrayObject VAO = { std::vector<VertexBufferObjectDescriptor>{
		{GL_STATIC_DRAW, IndexVertex{}},
		{ GL_DYNAMIC_DRAW, TextVertex{},1 }
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

void Renderer::RenderIDMap() {
	PROFILE_FUNKTION;
	if (!IdMapDirty) {
		return;
	}
	IdMapDirty = false;

	App->ContextBinder->BindContext(App->GlContext.get());

	FBOID.bind(FrameBufferObject::Draw);

	if (!UIDRun) {
		UIDRun = true;
		for (const auto& [shader, Location] : ShaderManager::GetShadersWithUniform("UIDRun")) {
			auto& Shader = *ShaderManager::GetShader(shader);
			Shader.bind();
			GLCALL(glUniform1i(Location, UIDRun));
			Shader.unbind();
		}
	}

	GLuint clearint = 0;
	GLCALL(glClearTexImage(FBOIDTexture.GetId(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &clearint));

	VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;

	GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

	auto SimplePass = [](auto VertsGetter, VertexArrayObject& VAO, ShaderManager::Shaders ShaderName) {
		if (!VertsGetter().Empty()) {
			const auto& Shader = ShaderManager::GetShader(ShaderName);

			Shader->bind();
			VAO.bind();
			VAO.DrawAs(GL_TRIANGLE_STRIP);
			VAO.unbind();
			Shader->unbind();
		}
		};

	SimplePass([&b]() {return b.GetAndVerts(); }, AndVAO, ShaderManager::And);
	SimplePass([&b]() {return b.GetOrVerts(); }, OrVAO, ShaderManager::Or);
	SimplePass([&b]() {return b.GetXOrVerts(); }, XOrVAO, ShaderManager::XOr);
	SimplePass([&b]() {return b.GetPinVerts(); }, PinVAO, ShaderManager::Pin);
	SimplePass([&b]() {return b.GetRoundedPinVerts(); }, RoundPinVAO, ShaderManager::RoundPin);
	SimplePass([&b]() {return b.GetBlockVerts(); }, BlocksVAO, ShaderManager::Block);
	SimplePass([&b]() {return b.GetMuxVerts(); }, MuxVAO, ShaderManager::Mux);

	GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));

	FBOID.unbind();
}

void Renderer::Render() {
	PROFILE_FUNKTION;

	if (!Dirty && !ShaderManager::IsDirty) return;
	Dirty = false;
	IdMapDirty = true;

	if (ShaderManager::IsDirty) {
		UpdateViewProjectionMatrix();
		RenderBackground();
		ShaderManager::IsDirty = false;
	}

	static MyDirection::Direction d = MyDirection::Up;
	if (FrameCount % 10 == 0) {
		d = MyDirection::RotateCW(d);
	}

	VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;

	const auto& MetadataOpt = b.GetBlockMetadata({ Frame->BlockManager->GetBlockIndex("ContainedBlock"),4 });
	if (MetadataOpt) {
		auto Metadata = MetadataOpt.value();
		//Metadata.Pos = MouseIndex;
		Metadata.Rotation = d;
		b.SetBlockMetadata({ Frame->BlockManager->GetBlockIndex("ContainedBlock"),4 }, Metadata);
	}

	/*App->ContextBinder->BindContext(App->GlContext.get());

	FBOPath.bind(FrameBufferObject::Draw);

	GLCALL(glStencilMask(0xFF));

	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GLCALL(glViewport(0, 0, CanvasSize.x(), CanvasSize.y()));*/

	//FBOPath.unbind();

	PROFILE_SCOPE_ID_START("Blit Background", 0);

	//FBOID.bind(FrameBufferObject::Draw);

	//GLCALL(glEnable(GL_DEPTH_TEST));
	//GLCALL(glDepthFunc(GL_ALWAYS));

	Frame->Canvas->SetCurrent(*App->GlContext.get());

	if (UIDRun) {
		UIDRun = false;
		for (const auto& [shader, Location] : ShaderManager::GetShadersWithUniform("UIDRun")) {
			auto& Shader = *ShaderManager::GetShader(shader);
			Shader.bind();
			GLCALL(glUniform1i(Location, UIDRun));
			Shader.unbind();
		}
	}

	GLCALL(glStencilMask(0xFF));

	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT*/));

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

	PROFILE_SCOPE_ID_END(0);

	PROFILE_SCOPE_ID_START("Render Wires", 1);

	//GLCALL(glDepthMask(GL_FALSE));

	std::vector<bool> PreviewNonPreview = { false };
	if (b.HasPreview()) PreviewNonPreview.push_back(true);

	for (bool Preview : PreviewNonPreview) {
		if (Preview) {
			GLCALL(glStencilMask(0xFF));
			GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
			b.UpdateVectsForVAOsPreview(BoundingBox, MouseIndex);
		}
		else {
			b.UpdateVectsForVAOs(BoundingBox, Zoom, MouseIndex);
		}

		const auto& PathHightlightShader = ShaderManager::GetShader(ShaderManager::HighlightPath);

		PathHightlightShader->bind();
		//GLCALL(glUniform3f(PathHightlightShader->GetLocation("UColor"), 1.0, 1.0, 0.0));
		FBOBackgroundTexture.bind(PathHightlightShader.get(), "UBackground", "", 0);

		GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
		GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
		GLCALL(glStencilMask(0xFF));

		if (b.hasUnmarked(Preview)) {
			GLCALL(glBlendFunc(GL_ONE, GL_ZERO));
			GLCALL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));

			auto& EdgesUnmarkedVAO = GetPathVAOs(Preview).EdgesUnmarkedVAO;

			EdgesUnmarkedVAO.bind();
			b.GetEdgesUnmarked(Preview).ReplaceBuffer(EdgesUnmarkedVAO, 1);
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

			auto& EdgesMarkedVAO = GetPathVAOs(Preview).EdgesMarkedVAO;

			EdgesMarkedVAO.bind();
			b.GetEdgesMarked(Preview).ReplaceBuffer(EdgesMarkedVAO, 1, !Preview);
			EdgesMarkedVAO.DrawAs(GL_TRIANGLE_STRIP);
			EdgesMarkedVAO.unbind();


			GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
			GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
			GLCALL(glStencilMask(0x00));

			auto& VertsVAO = GetPathVAOs(Preview).VertsVAO;

			if (!b.GetVerts(Preview).Empty()) {

				VertsVAO.bind();
				b.GetVerts(Preview).ReplaceBuffer(VertsVAO, 1);
				VertsVAO.DrawAs(GL_TRIANGLE_STRIP);
			}

			PathHightlightShader->unbind();

			if (!b.GetVerts(Preview).Empty()) {
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
			}

			GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
			GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		}

		if (!b.GetEdges(Preview).Empty()) {

			const auto& PathShader = ShaderManager::GetShader(ShaderManager::Path);

			PathShader->bind();

			auto& EdgesVAO = GetPathVAOs(Preview).EdgesVAO;

			EdgesVAO.bind();
			b.GetEdges(Preview).ReplaceBuffer(EdgesVAO, 1);
			EdgesVAO.DrawAs(GL_TRIANGLE_STRIP);
			EdgesVAO.unbind();
			PathShader->unbind();
		}

		if (!b.GetSpecialPoints(Preview).Empty()) {
			const auto& IntersectionPathShader = ShaderManager::GetShader(ShaderManager::IntersectionPath);

			IntersectionPathShader->bind();


			GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
			GLCALL(glStencilFunc(GL_LEQUAL, 2, 0xFF));

			auto& SpecialPointsVAO = GetPathVAOs(Preview).SpecialPointsVAO;

			SpecialPointsVAO.bind();
			b.GetSpecialPoints(Preview).ReplaceBuffer(SpecialPointsVAO, 1);
			SpecialPointsVAO.DrawAs(GL_TRIANGLE_STRIP);
			SpecialPointsVAO.unbind();

			IntersectionPathShader->unbind();

			GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
			GLCALL(glStencilMask(0x00));
		}
		//GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}



	PROFILE_SCOPE_ID_END(1);

	PROFILE_SCOPE_ID_START("Copy To FBO Path", 2);

	FBOPath.bind(FrameBufferObject::Draw);

	GLCALL(glBlitFramebuffer(
		0, 0,
		CanvasSize.x(), CanvasSize.y(),
		0, 0,
		CanvasSize.x(), CanvasSize.y(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	));

	FBOPath.unbind();

	PROFILE_SCOPE_ID_END(2);

	PROFILE_SCOPE_ID_START("Recreate Stencil from Path", 3);

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

		auto& EdgesVAO = GetPathVAOs(Preview).EdgesVAO;

		EdgesVAO.bind();
		b.GetEdges(Preview).ReplaceBuffer(EdgesVAO, 1);//Not Required as the should have been replaced aready
		//EdgesVAO.ReplaceVertexBuffer(b.GetEdges(Preview), 1);
		EdgesVAO.DrawAs(GL_TRIANGLE_STRIP);
		EdgesVAO.unbind();

	}
	PathShader->unbind();

	PROFILE_SCOPE_ID_END(3);

	PROFILE_SCOPE_ID_START("Draw And Or XOR", 4);

	//GLCALL(glDepthMask(GL_TRUE));
	GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
	GLCALL(glStencilMask(0x00));

	auto SimplePass = [](auto VertsGetter, VertexArrayObject& VAO, ShaderManager::Shaders ShaderName) {
		if (!VertsGetter().Empty()) {
			const auto& Shader = ShaderManager::GetShader(ShaderName);

			Shader->bind();
			VAO.bind();
			VertsGetter().ReplaceBuffer(VAO, 1);
			VAO.DrawAs(GL_TRIANGLE_STRIP);
			VAO.unbind();
			Shader->unbind();
		}
		};

	SimplePass([&b]() {return b.GetAndVerts(); }, AndVAO, ShaderManager::And);
	SimplePass([&b]() {return b.GetOrVerts(); }, OrVAO, ShaderManager::Or);
	SimplePass([&b]() {return b.GetXOrVerts(); }, XOrVAO, ShaderManager::XOr);

	PROFILE_SCOPE_ID_END(4);

	PROFILE_SCOPE_ID_START("Pins", 5);

	//GLCALL(glDepthMask(GL_FALSE));

	auto WirePass = [this](auto VertsGetter, VertexArrayObject& VAO, ShaderManager::Shaders ShaderName) {
		if (!VertsGetter().Empty()) {

			const auto& Shader = ShaderManager::GetShader(ShaderName);

			Shader->bind();

			FBOPathColorTexture.bind(Shader.get(), "UPath", "", 0);

			VAO.bind();
			VertsGetter().ReplaceBuffer(VAO, 1);

			GLCALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
			GLCALL(glUniform1i(Shader->GetLocation("UHasWire"), true));
			VAO.DrawAs(GL_TRIANGLE_STRIP);

			GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
			GLCALL(glUniform1i(Shader->GetLocation("UHasWire"), false));
			VAO.DrawAs(GL_TRIANGLE_STRIP);

			VAO.unbind();

			Shader->unbind();
		}
		};

	WirePass([&b]() {return b.GetPinVerts(); }, PinVAO, ShaderManager::Pin);
	WirePass([&b]() {return b.GetRoundedPinVerts(); }, RoundPinVAO, ShaderManager::RoundPin);

	GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
	GLCALL(glStencilMask(0x00));
	//GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
	//GLCALL(glStencilMask(0x00));

	PROFILE_SCOPE_ID_END(5);

	SimplePass([&b]() {return b.GetBlockVerts(); }, BlocksVAO, ShaderManager::Block);
	SimplePass([&b]() {return b.GetSevenSegVerts(); }, SevenSegVAO, ShaderManager::SevenSeg);
	SimplePass([&b]() {return b.GetSixteenSegVerts(); }, SixteenSegVAO, ShaderManager::SixteenSeg);

	//#ifndef USETHIS
	//	if (!b.GetBlockVerts().Empty()) {
	//#endif
	//		PROFILE_SCOPE("Blocks");
	//
	//		//GLCALL(glDepthMask(GL_TRUE));
	//
	//		const auto& BlockShader = ShaderManager::GetShader(ShaderManager::Block);
	//
	//		BlockShader->bind();
	//
	//		//Blocks.emplace_back(0, 0, 2, -3, 1.0, 0.5, 0.5, 1.0);
	//
	//		BlocksVAO.bind();
	////#ifndef  USETHIS
	////		b.GetBlockVerts().ReplaceBuffer(BlocksVAO, 1);
	////#else
	////		BlocksVAO.ReplaceVertexBuffer(b.GetBlockVerts().Vertices, 1);
	////#endif
	////		BlocksVAO.DrawAs(GL_TRIANGLE_STRIP);
	//		b.GetBlockVerts().ReplaceBuffer(BlocksVAO, 1);
	//		//glUniform1i(BlockShader->GetLocation("UIDRun"), true);
	//		//GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));
	//		//BlocksVAO.DrawAs(GL_TRIANGLE_STRIP);
	//		//glUniform1i(BlockShader->GetLocation("UIDRun"), false);
	//		//GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));
	//		BlocksVAO.DrawAs(GL_TRIANGLE_STRIP);
	//		BlocksVAO.unbind();
	//
	//		//#ifdef RenderCollisionGrid
	//		//#define USETHIS
	//#ifdef USETHIS
	//
	//		Blocks.clear();
	//		static std::unordered_map<int, std::tuple<float, float, float>> ColourMap;
	//		auto GetColour = [&](const int& i) {
	//			if (i == 0) return std::make_tuple(1.0f, 1.0f, 1.0f);
	//			auto it = ColourMap.find(i);
	//			if (it == ColourMap.end()) {
	//				ColourMap[i] = std::make_tuple(float(rand()) / (float)RAND_MAX, float(rand()) / (float)RAND_MAX, float(rand()) / (float)RAND_MAX);
	//				return ColourMap[i];
	//			}
	//			return it->second;
	//			};
	//
	//		/*Blocks.emplace_back(0, 0, MouseIndex.x(), MouseIndex.y(),
	//			1, 0, 0, 0.5);*/
	//
	//			//GLCALL(glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA));
	//
	//		for (const auto& pair : b.CollisionMap) {
	//			MyRectI BB = MyRectI::FromCorners(pair.first, pair.first + PointType{ VisualBlockInterior::BoxSize,VisualBlockInterior::BoxSize });
	//			if (BoundingBox.Intersectes(BB)) {
	//				const auto& size = pair.second.size();
	//				if (size == 0) {
	//					Blocks.emplace_back(
	//						BB.Position.x(),
	//						BB.Position.y(),
	//						BB.Position.x() + BB.Size.x() - 1,
	//						BB.Position.y() + BB.Size.y() - 1,
	//						0.5, 0.5, 0.5, 0.1);
	//				}
	//				else {
	//					auto [r, g, b] = GetColour(size);
	//					Blocks.emplace_back(
	//						BB.Position.x(),
	//						BB.Position.y(),
	//						BB.Position.x() + BB.Size.x() - 1,
	//						BB.Position.y() + BB.Size.y() - 1,
	//						r, g, b, 0.5);
	//				}
	//			}
	//		}
	//		//Blocks.emplace_back(0, 0, 2, 3,0.5,0.5,0.5,0.1);
	//
	//		//GLCALL(glDepthMask(GL_FALSE));
	//
	//		BlocksVAO.bind();
	//		BlocksVAO.ReplaceVertexBuffer(Blocks, 1);
	//		BlocksVAO.DrawAs(GL_TRIANGLE_STRIP);
	//		BlocksVAO.unbind();
	//
	//
	//
	//		//GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	//
	//
	//#endif
	//
	//		BlockShader->unbind();
	//
	//#ifndef USETHIS
	//	}
	//#endif

	SimplePass([&b]() {return b.GetMuxVerts(); }, MuxVAO, ShaderManager::Mux);

	if (!b.GetStaticTextVerts().Empty() || !b.GetDynamicTextVerts().Empty()) {

		PROFILE_SCOPE("Text");

		const auto& TextShader = ShaderManager::GetShader(ShaderManager::Text);

		TextShader->bind();

		TextAtlas.bind(TextShader.get(), "UTexture", "", 0);

		if (!b.GetStaticTextVerts().Empty()) {
			StaticTextVAO.bind();
			b.GetStaticTextVerts().ReplaceBuffer(StaticTextVAO, 1);
			StaticTextVAO.DrawAs(GL_TRIANGLE_STRIP);
			StaticTextVAO.unbind();
		}

		if (!b.GetDynamicTextVerts().Empty()) {
			DynamicTextVAO.bind();
			b.GetDynamicTextVerts().ReplaceBuffer(DynamicTextVAO, 1);
			DynamicTextVAO.DrawAs(GL_TRIANGLE_STRIP);
			DynamicTextVAO.unbind();
		}
		TextShader->unbind();
	}

	//const auto& RoundPinShader = ShaderManager::GetShader(ShaderManager::RoundPin);

	//RoundPinShader->bind();

	//FBOPathColorTexture.bind(RoundPinShader.get(), "UPath", "", 0);

	//RoundPinVAO.bind();
	//b.GetRoundedPinVerts().ReplaceBuffer(RoundPinVAO, 1, false);
	//GLCALL(glUniform1i(RoundPinShader->GetLocation("UHasWire"), true));
	//RoundPinVAO.DrawAs(GL_TRIANGLE_STRIP);

	////std::vector<PointIRGBIDVertex> PinVert;
	///*PinVert.emplace_back(PointType{ 10,50 }, ColourType{ 1,0,1,0 });
	//PinVert.emplace_back(PointType{ 8,50 }, ColourType{ 1,0,1,0 });
	//PinVert.emplace_back(PointType{ 8,52 }, ColourType{ 1,0,1,0 });
	//PinVert.emplace_back(PointType{ 10,52 }, ColourType{ 1,0,1,0 });*/

	////RoundPinVAO.ReplaceVertexBuffer(PinVert, 1);
	////RoundPinVAO.DrawAs(GL_TRIANGLE_STRIP);

	//RoundPinVAO.unbind();

	//FBOPathColorTexture.unbind();

	//RoundPinShader->unbind();

	PROFILE_SCOPE("Swap Buffers");


	/*std::stringstream ss;
	ss << "E: " << b.GetEdges(false).Size();
	ss << ", Em:" << b.GetEdgesMarked(false).Size();
	ss << ", Eum:" << b.GetEdgesUnmarked(false).Size();
	ss << ", Sp:" << b.GetSpecialPoints(false).Size();
	ss << ", V:" << b.GetVerts(false).Size();
	ss << ", hm:" << b.hasMark(false);
	ss << ", hum:" << b.hasUnmarked(false);
	ss << ", PE: " << b.GetEdges(true).Size();
	ss << ", PEm:" << b.GetEdgesMarked(true).Size();
	ss << ", PEum:" << b.GetEdgesUnmarked(true).Size();
	ss << ", PSp:" << b.GetSpecialPoints(true).Size();
	ss << ", PV:" << b.GetVerts(true).Size();
	ss << ", hPm:" << b.hasMark(true);
	ss << ", hPum:" << b.hasUnmarked(true);
	static int TIMES = 0;
	ss << ", T:" << TIMES++;
	ss << ", Zoom:" << Zoom;*/

	//FBOID.unbind();

	title = "Zoom: " + std::to_string(Zoom) + " Off: " + std::to_string(Offset.x()) + ", " + std::to_string(Offset.y());;

	//FBOID.unbind();

	/*Frame->Canvas->SetCurrent(*App->GlContext.get());

	FBOID.bind(FrameBufferObject::Read);

	GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0));

	GLCALL(glBlitFramebuffer(
		0, 0,
		CanvasSize.x(), CanvasSize.y(),
		0, 0,
		CanvasSize.x(), CanvasSize.y(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	));

	FBOID.unbind();*/

	if (!AreaSelectVerts.Empty()) {

		PROFILE_SCOPE("AreaSelect");

		//GLCALL(glDepthMask(GL_FALSE));

		const auto& AreaSelectShader = ShaderManager::GetShader(ShaderManager::AreaSelect);

		AreaSelectShader->bind();

		AreaSelectVAO.bind();
		AreaSelectVerts.ReplaceBuffer(AreaSelectVAO, 1);
		AreaSelectVAO.DrawAs(GL_TRIANGLE_STRIP);
		AreaSelectVAO.unbind();

		AreaSelectShader->unbind();
	}

	//GetBlockBoundingBoxes(8, true);

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
	FBOIDTexture(1, 1, nullptr, []() {
	Texture::Descriptor desc;
	desc.Format = GL_RED_INTEGER;
	desc.Internal_Format = GL_R32UI;
	desc.Depth_Stencil_Texture_Mode = GL_STENCIL_INDEX;
	desc.Type = GL_UNSIGNED_INT;
	return desc;
		}()),
	FBOID({ &FBOIDTexture }, { GL_COLOR_ATTACHMENT1 }),
#ifdef RenderCollisionGrid
	BlocksVAO(CreateBlockRGBAVAO()),
#endif
	SevenSegVAO(CreateSevenSegVAO()),
	SixteenSegVAO(CreateSixteenSegVAO()),
	MuxVAO(CreateMuxVAO()),
	PinVAO(CreatePointIOrientationRGBVAO()),
	RoundPinVAO(CreatePointIRGBIDVAO()),
	AndVAO(CreatePointIOrientationVAO()),
	OrVAO(CreatePointIOrientationVAO()),
	XOrVAO(CreatePointIOrientationVAO()),
	/*NotTriangleVAO(CreatePointIOrientationVAO()),
	NDotVAO(CreatePointIOrientationVAO()),*/
	AreaSelectVAO(CreatePointFRGBVAO()),
	VAOsPath(PathVAOs{
	.EdgesVAO = CreateTwoPointIVAO(),
	.EdgesMarkedVAO = CreateTwoPointIVAO(),
	.EdgesUnmarkedVAO = CreateTwoPointIVAO(),
	.SpecialPointsVAO = CreatePointIVAO(),
	.VertsVAO = CreateTwoPointIVAO(),
		}),
		VAOsPathPreview(PathVAOs{
		.EdgesVAO = CreateTwoPointIVAO(),
		.EdgesMarkedVAO = CreateTwoPointIVAO(),
		.EdgesUnmarkedVAO = CreateTwoPointIVAO(),
		.SpecialPointsVAO = CreatePointIVAO(),
		.VertsVAO = CreateTwoPointIVAO(),
			}),
			StaticTextVAO(CreateTextVAO()),
			DynamicTextVAO(CreateTextVAO()),
			TextAtlas(PngManager::GetPng(PngManager::atlas), []() {
	Texture::Descriptor desc;
	desc.Min_Filter = Texture::Descriptor::Filter_Type::LINEAR;
	desc.Mag_Filter = Texture::Descriptor::Filter_Type::LINEAR;
	return desc;
				}()) {
	//auto extend = Get
// TextExtend("I1234567890xgÜ");

	//AddText("1234567890xgÜ", Point<float>{ 0,float(0.0 - extend.TheoreticalHeight / 2.0) },MyDirection::Up);

	//std::string Ts = "HI, This\nis\na\ntest";
	//AddText(Ts, { 10,50 }, true, MyDirection::Up, { 1.0,0.0,0.0,0.9 }, { 0.0,1.0,0.0,0.0 });
	//AddText(Ts, { 8,50 }, true, MyDirection::Right, { 1.0,0.0,0.0,0.9 }, { 0.0,1.0,0.0,0.0 });
	//AddText(Ts, { 8,52 }, true, MyDirection::Down, { 1.0,0.0,0.0,0.9 }, { 0.0,1.0,0.0,0.0 });
	//AddText(Ts, { 10,52 }, true, MyDirection::Left, { 1.0,0.0,0.0,0.9 }, { 0.0,1.0,0.0,0.0 });
	BBUpdater = std::thread([this]() {BBUpdaterWork(); });
}

Renderer::~Renderer() {
	Terminating = true;
	BBUpdaterCV.notify_all();
	BBUpdater.join();
}

void Renderer::UpdateSize() {
	PROFILE_FUNKTION;

	auto Size = Frame->Canvas->GetSize();
	CanvasSize = { Size.x,Size.y };

	FBOBackgroundTexture.Resize(CanvasSize.x(), CanvasSize.y());

	FBOPathStencileDepthTexture.Resize(CanvasSize.x(), CanvasSize.y());
	FBOPathColorTexture.Resize(CanvasSize.x(), CanvasSize.y());

	//FBONonDefaultStencilDepthTexture.Resize(CanvasSize.x(),CanvasSize.y());
	//FBONonDefaultColorTexture.Resize(CanvasSize.x(),CanvasSize.y());
	FBOIDTexture.Resize(CanvasSize.x(), CanvasSize.y());

	UpdateViewProjectionMatrix();
	Dirty = true;
	Render();
}

BufferedVertexVec<PointFRGBVertex>& Renderer::GetAreaSelectVerts() {
	return AreaSelectVerts;
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

unsigned int Renderer::GetHighlited(const Eigen::Vector2f& Mouse) {
	App->ContextBinder->BindContext(App->GlContext.get());

	RenderIDMap();

	FBOID.bind(FrameBufferObject::Read);

	GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

	GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT1));

	auto Size = Frame->Canvas->GetSize();

	//std::array<GLushort,4> s;
	GLuint i;
	GLCALL(glReadPixels(Mouse.x(), Size.y - Mouse.y(), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &i));

	GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));

	FBOID.unbind();

	//title.clear();

	//std::stringstream ss;
	//ss << "R: " << s[0] << " G: " << s[1] << " B: " << s[2] << " A: " << s[3];
	//title = ss.str();

	return i;
}

const std::array<MyRectF, 4>& Renderer::GetBlockBoundingBoxes(const CompressedBlockDataIndex& cbdi, bool TheOneIWant) {
	PROFILE_FUNKTION;
	//if (!TheOneIWant) {
	auto it = BlockBoundingBoxes.find(cbdi);
	if (it != BlockBoundingBoxes.end())return it->second;
	//}

	auto AltZoom = Zoom;
	auto AltOffset = Offset;
	auto AltCanvasSize = CanvasSize;
	//auto AltMouseIndex = MouseIndex;

	Offset = Eigen::Vector2f{ 0,0 };
	float TargetZoom = 0.01f;

	Zoom = TargetZoom;

	//UpdateMouseIndex({ -1,-1 });

	//Also binds context
	//UpdateViewProjectionMatrix();

	auto& SB = Frame->BlockManager->GetSpecialBlockIndex();

	const auto& ContainedExteriorOpt = Frame->BlockManager->GetCompressedBlockData(cbdi);
	if (!ContainedExteriorOpt) {
		assert(false && "you messed up");
	}
	const auto& ContainedExterior = ContainedExteriorOpt.value();
	const auto& BlockSize = ContainedExterior.blockExteriorData.Size;

	auto rectVertical = MyRectF::FromCorners(Eigen::Vector2f{ -1.5, 1.5 },
		Eigen::Vector2f{ BlockSize.x(), -BlockSize.y() } + Eigen::Vector2f{ 1.5, -1.5 });

	auto rectHorizontal = MyRectF::FromCorners(Eigen::Vector2f{ -1.5, 1.5 },
		Eigen::Vector2f{ BlockSize.y(), -BlockSize.x() } + Eigen::Vector2f{ 1.5, -1.5 });

	if (!UIDRun) {
		UIDRun = true;
		for (const auto& [shader, Location] : ShaderManager::GetShadersWithUniform("UIDRun")) {
			auto& Shader = *ShaderManager::GetShader(shader);
			Shader.bind();
			GLCALL(glUniform1i(Location, UIDRun));
			Shader.unbind();
		}
	}

	//for (auto Rotation : { MyDirection::Up,MyDirection::Down,MyDirection::Left, MyDirection::Right }) {

	auto Rotation = MyDirection::Up;

	PointType ViewportSize;
	if (Rotation == MyDirection::Left || Rotation == MyDirection::Right) {
		ViewportSize = { int(rectHorizontal.Size.x()) * 1.0 / Zoom, int(rectHorizontal.Size.y()) * 1.0 / Zoom };
	}
	else {
		ViewportSize = { int(rectVertical.Size.x()) * 1.0 / Zoom, int(rectVertical.Size.y()) * 1.0 / Zoom };
	}


	FBOIDTexture.Resize(ViewportSize.x(), ViewportSize.y());

	CanvasSize = Eigen::Vector2f{ ViewportSize.x(),ViewportSize.y() };

	UpdateViewProjectionMatrix(true);

	FBOID.bind();

	GLCALL(glViewport(0, 0, ViewportSize.x(), ViewportSize.y()));

	GLuint clearint = 0;
	GLCALL(glClearTexImage(FBOIDTexture.GetId(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &clearint));

	VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;

	GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

	auto SimplePass = [](auto& BlockBuffer, auto& MyBuffer, VertexArrayObject& VAO, ShaderManager::Shaders ShaderName) {
		BlockBuffer.Dirty = true;
		const auto& Shader = ShaderManager::GetShader(ShaderName);

		Shader->bind();
		VAO.bind();
		MyBuffer.ReplaceBuffer(VAO, 1);
		VAO.DrawAs(GL_TRIANGLE_STRIP);
		VAO.unbind();
		Shader->unbind();
		};

	/*PointType TopLeft;
	PointType BottomRight;
	if (Rotation == MyDirection::Left || Rotation == MyDirection::Right) {
		TopLeft = Meta.Pos + PointType{ -1,-1 }*int((BlockSize.y() - BlockSize.x()) / 2.0);
		BottomRight = TopLeft + PointType{ BlockSize.y(), -BlockSize.x() };
	}
	else {
		TopLeft = Meta.Pos;
		BottomRight = TopLeft + PointType{ BlockSize.x(), -BlockSize.y() };
	}*/

	ColourType NoColor = { 0,0,0,0 };


	BlockMetadata Meta;
	Meta.Pos = {};
	Meta.Rotation = Rotation;

	PointType TopLeft;
	PointType BottomRight;
	if (Rotation == MyDirection::Left || Rotation == MyDirection::Right) {
		//Meta.Pos = 
		TopLeft = Meta.Pos + PointType{ -1,-1 }*int((BlockSize.y() - BlockSize.x()) / 2.0);
		BottomRight = TopLeft + PointType{ BlockSize.y(), -BlockSize.x() };
	}
	else {
		//Meta.Pos = 
		TopLeft = Meta.Pos;
		BottomRight = TopLeft + PointType{ BlockSize.x(), -BlockSize.y() };
	}

	if (cbdi == SB.And || cbdi == SB.Or || cbdi == SB.XOr) {
		if (cbdi == SB.And) {
			BufferedVertexVec<PointIOrientationRGBIDVertex > AndBuffer;
			AndBuffer.Emplace(1u, Meta, NoColor);
			SimplePass(b.GetAndVerts(), AndBuffer, AndVAO, ShaderManager::And);
		}
		if (cbdi == SB.Or) {
			BufferedVertexVec<PointIOrientationRGBIDVertex > OrBuffer;
			OrBuffer.Emplace(1u,Meta, NoColor);
			SimplePass(b.GetOrVerts(), OrBuffer, OrVAO, ShaderManager::Or);
		}
		if (cbdi == SB.XOr) {
			BufferedVertexVec<PointIOrientationRGBIDVertex > XOrBuffer;
			XOrBuffer.Emplace(1u, Meta, NoColor);
			SimplePass(b.GetXOrVerts(), XOrBuffer, XOrVAO, ShaderManager::XOr);
		}

		BufferedVertexVec<PointIRGBIDVertex> RoundedPinBuffer;
		for (const auto& Pin : ContainedExterior.blockExteriorData.InputPin) {
			RoundedPinBuffer.Emplace(1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor);
		}
		for (const auto& Pin : ContainedExterior.blockExteriorData.OutputPin) {
			RoundedPinBuffer.Emplace(1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor);
		}
		SimplePass(b.GetRoundedPinVerts(), RoundedPinBuffer, RoundPinVAO, ShaderManager::RoundPin);
	}
	else if (cbdi == SB.Mux) {
		BufferedVertexVec<MuxIDVertex> MuxBuffer;
		MuxBuffer.Emplace(1u,Meta, 2, 0, NoColor);
		SimplePass(b.GetMuxVerts(), MuxBuffer, MuxVAO, ShaderManager::Mux);

		BufferedVertexVec<PointIOrientationRGBRHGHBHIDVertex> PinBuffer;
		for (const auto& Pin : ContainedExterior.blockExteriorData.InputPin) {
			PinBuffer.Emplace(1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), VisualBlockInterior::GetPinRotation(Meta, Pin), NoColor, NoColor);
		}
		for (const auto& Pin : ContainedExterior.blockExteriorData.OutputPin) {
			PinBuffer.Emplace(1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), VisualBlockInterior::GetPinRotation(Meta, Pin), NoColor, NoColor);
		}
		SimplePass(b.GetPinVerts(), PinBuffer, PinVAO, ShaderManager::Pin);
	}
	else {
		BufferedVertexVec<TwoPointIRGBAIDVertex> BlockBuffer;
		BlockBuffer.Emplace(1u, TopLeft, BottomRight, NoColor, NoColor);
		SimplePass(b.GetBlockVerts(), BlockBuffer, BlocksVAO, ShaderManager::Block);

		BufferedVertexVec<PointIOrientationRGBRHGHBHIDVertex> PinBuffer;
		for (const auto& Pin : ContainedExterior.blockExteriorData.InputPin) {
			PinBuffer.Emplace(1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), VisualBlockInterior::GetPinRotation(Meta, Pin), NoColor, NoColor);
		}
		for (const auto& Pin : ContainedExterior.blockExteriorData.OutputPin) {
			PinBuffer.Emplace(1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), VisualBlockInterior::GetPinRotation(Meta, Pin), NoColor, NoColor);
		}
		SimplePass(b.GetPinVerts(), PinBuffer, PinVAO, ShaderManager::Pin);
	}
	FBOID.unbind();

	FBOID.bind(FrameBufferObject::Read);

	GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

	GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT1));

	std::unique_ptr<std::vector<GLuint>> vec = std::make_unique<std::vector<GLuint>>();
	vec->resize(ViewportSize.x() * ViewportSize.y());
	GLCALL(glReadPixels(0, 0, ViewportSize.x(), ViewportSize.y(), GL_RED_INTEGER, GL_UNSIGNED_INT, vec->data()));

	GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));
	FBOID.unbind();

	MyRectF rect = MyRectF::FromCorners({ 0,0 }, { 1,1 });
	BlockBoundingBoxes[cbdi] = {rect,rect,rect,rect};

	std::unique_lock ul_Funcs(BBUpdateFuncsMutex);

	BBUpdateFuncs.push([vec = vec.release(), ViewportSize, TargetZoom, BlockSize, this, SB, cbdi]() {
		PROFILE_FUNKTION;

		int xmin = ViewportSize.x();
		int xmax = -1;
		int ymin = ViewportSize.y();
		int ymax = -1;

		for (int y = 0; y < ViewportSize.y(); y++) {
			for (int x = 0; x < ViewportSize.x(); x++) {
				if ((*vec)[y * ViewportSize.x() + x] != 1) continue;
				xmin = std::min(xmin, x);
				xmax = std::max(xmax, x);
				ymin = std::min(ymin, y);
				ymax = std::max(ymax, y);
			}
		}
		xmin -= ViewportSize.x() / 2.0;
		xmax -= ViewportSize.x() / 2.0;
		ymin -= ViewportSize.y() / 2.0;
		ymax -= ViewportSize.y() / 2.0;

		float factor = TargetZoom * 2;
		Eigen::Vector2f Point1{ xmin * factor, -ymin * factor };
		Eigen::Vector2f Point2{ xmax * factor,-ymax * factor };

		auto Rotate = [](float angle) {
			return Eigen::Matrix2f{
				{std::round(cos(angle)),-std::round(sin(angle))},
				{std::round(sin(angle)),std::round(cos(angle))}
			};
			};

		auto rectDown = MyRectF::FromCorners(Rotate(0) * Point1, Rotate(0) * Point2);
		auto rectRight = MyRectF::FromCorners(Rotate((float)M_PI / 2.0f) * Point1, Rotate((float)M_PI / 2.0f) * Point2);
		auto rectUp = MyRectF::FromCorners(Rotate((float)M_PI) * Point1, Rotate((float)M_PI) * Point2);
		auto rectLeft = MyRectF::FromCorners(Rotate(3.0f * (float)M_PI / 2.0f) * Point1, Rotate(3.0f * (float)M_PI / 2.0f) * Point2);

		auto off = PointType{ -1,-1 }*int((BlockSize.y() - BlockSize.x()) / 2.0);
		auto Off = Eigen::Vector2f{ off.x(),off.y() };

		Off = { 0,0 };

		std::array<MyRectF, 4> BBs;

		BBs[MyDirection::Up] = rectUp;
		BBs[MyDirection::Down] = rectDown;
		BBs[MyDirection::Left] = rectLeft;
		BBs[MyDirection::Right] = rectRight;

		BBs[MyDirection::Left].Position += Off;
		BBs[MyDirection::Right].Position += Off;
		BBs[MyDirection::Down].Position.y() -= BlockSize.y();
		BBs[MyDirection::Up].Position.x() += BlockSize.x();
		BBs[MyDirection::Right].Position.x() += BlockSize.y();
		BBs[MyDirection::Right].Position.y() -= BlockSize.x();
		//}

		if (cbdi == SB.Mux) {
			std::swap(BBs[MyDirection::Left], BBs[MyDirection::Right]);
			std::swap(BBs[MyDirection::Down], BBs[MyDirection::Up]);
		}
		BlockBoundingBoxes[cbdi] = BBs;

		Dirty = true;
		delete vec;
	});
	ul_Funcs.unlock();
	BBUpdaterCV.notify_one();

	IdMapDirty = true;


	Zoom = AltZoom;
	Offset = AltOffset;
	CanvasSize = AltCanvasSize;

	UpdateViewProjectionMatrix(true);

	FBOIDTexture.Resize(CanvasSize.x(), CanvasSize.y());

	//auto Off = PointType{ -1,-1 }*int((BlockSize.y() - BlockSize.x()) / 2.0);
	//rectHorizontal.Position += Eigen::Vector2f{ Off.x(), Off.y() };

	//BBs[MyDirection::Up] = rectVertical;
	//BBs[MyDirection::Down] = rectVertical;
	//BBs[MyDirection::Left] = rectHorizontal;
	//BBs[MyDirection::Right] = rectHorizontal;

	return BlockBoundingBoxes.at(cbdi);
}

void Renderer::BBUpdaterWork() {
	PROFILE_FUNKTION;
	while (!Terminating) {
		std::unique_lock ul(BBUpdaterCVMutex);
		BBUpdaterCV.wait(ul, [this]() {
			if (Terminating) return true;
			std::unique_lock ul(BBUpdateFuncsMutex);
			return !BBUpdateFuncs.empty();
			});
		if (Terminating) return;
		std::unique_lock ul_Funcs(BBUpdateFuncsMutex);
		if (BBUpdateFuncs.empty()) continue;
		BBUpdateFunc func = BBUpdateFuncs.front();
		BBUpdateFuncs.pop();
		ul_Funcs.unlock();
		func();
	}
}

void Renderer::StartGoHome() {
	GoHomeZoom = Zoom;
	GoHomeOffset = Offset;
	GoHomeFrame = 0;
}

//Returns if finished
bool Renderer::StepGoHome() {
	GoHomeFrame++;

	double TargetZoom = 0.01;
	Eigen::Vector2f TargetOffset = { 0,0 };

	double t = GoHomeFrame / 10.0;

	Zoom = GoHomeZoom * (1.0 - t) + TargetZoom * t;
	Offset = GoHomeOffset * (1.0 - t) + TargetOffset * t;
	UpdateViewProjectionMatrix();

	CheckZoomDots();

	Dirty = true;
	Render();

	title = "GoHome " + std::to_string(t);

	return GoHomeFrame >= 10;
}