#include "BlockAndPathData.hpp"
#include "pch.hpp"

#include "Renderer.hpp"

#include "MyApp.hpp"

#include "MyFrame.hpp"
#include "PngManager.hpp"
#include "ShaderManager.hpp"

// #include "RenderTextUtility.hpp"

#include "DataResourceManager.hpp"

#include "BlockSelector.hpp"

void Renderer::RenderBackground() {
  PROFILE_FUNKTION;

  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  FBOBackground.bind(FrameBufferObject::Draw);

  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, CanvasSize.x(), CanvasSize.y());

  GLCALL(glDisable(GL_DEPTH_TEST));
  Shader& BackgroundShader =
      ShaderManager::GetShader(ShaderManager::Background);

  BackgroundShader.bind();

  Frame->HoleScreenVAO->bind();

  Frame->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);

  Frame->HoleScreenVAO->unbind();

  BackgroundShader.unbind();

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

  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  PROFILE_SCOPE_ID_START("UZoom", 1);

  ShaderManager::applyGlobal("UZoom", Shader::Data2f{float(Zoom * CanvasSize.x()), float(Zoom * CanvasSize.y())});
  PROFILE_SCOPE_ID_END(1);

  PROFILE_SCOPE_ID_START("UViewProjectionMatrix", 2);
  ShaderManager::applyGlobal("UViewProjectionMatrix", Shader::DataMatrix3fv{.count = 1, .transpose = GL_FALSE, .value = ViewProjectionMatrix.data()});
  PROFILE_SCOPE_ID_END(2);

  PROFILE_SCOPE_ID_START("UOffset", 3);
  ShaderManager::applyGlobal("UOffset", Shader::Data2f{Offset.x(), Offset.y()});
  PROFILE_SCOPE_ID_END(3);

  PROFILE_SCOPE_ID_START("UZoomFactor", 4);
  ShaderManager::applyGlobal("UZoomFactor",Shader::Data1f{float(Zoom)});
  PROFILE_SCOPE_ID_END(4);

  if (OnlyForUniforms)
    return;

  PROFILE_SCOPE_ID_START("Calc BB", 5);

  Eigen::Vector3f UpperLeft = ViewProjectionMatrix * Eigen::Vector3f{-1, -1, 1};
  Eigen::Vector3f LowerRight = ViewProjectionMatrix * Eigen::Vector3f{1, 1, 1};

  Eigen::Vector2f Corner1{UpperLeft.x(), UpperLeft.y()};
  Eigen::Vector2f Corner2{LowerRight.x(), LowerRight.y()};

  BoundingBox = MyRectF::FromCorners(Corner1, Corner2);

  PROFILE_SCOPE_ID_END(5);

  RenderBackground();
}

Eigen::Vector2i Renderer::CoordToNearestPoint(Eigen::Vector2f Pos) {
  // PROFILE_FUNKTION;

  return Eigen::Vector2i(static_cast<int>(std::round(Pos.x())),
                         static_cast<int>(std::round(Pos.y())));
}

Renderer::PathVAOs &Renderer::GetPathVAOs(bool Preview) {
  return Preview ? VAOsPathPreview : VAOsPath;
}

template <typename VertexType> VertexArrayObject Renderer::CreateVAOInstancing4() {
  VertexArrayObject VAO = {std::vector<VertexBufferObjectDescriptor>{
      {GL_STATIC_DRAW, IndexVertex{}}, {GL_DYNAMIC_DRAW, VertexType{}, 1}}};

  VAO.bind();
  VAO.ReplaceVertexBuffer(std::vector<IndexVertex>{{0}, {1}, {2}, {3}}, 0);
  VAO.unbind();
  return VAO;
}

template <typename VertexType> VertexArrayObject Renderer::CreateVAO() {
  return VertexArrayObject{std::vector<VertexBufferObjectDescriptor>{
      {GL_DYNAMIC_DRAW, VertexType{}}}};
}

void Renderer::RenderIDMap() {
  PROFILE_FUNKTION;

  if (!IdMapDirty) {
    return;
  }

  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  IdMapDirty = false;

  FBOID.bind(FrameBufferObject::Draw);

  if (!UIDRun) {
    UIDRun = true;
    ShaderManager::applyGlobal("UIDRun", Shader::Data1i{UIDRun});
  }

  GLuint clearint = 0;
  GLCALL(glClearTexImage(FBOIDTexture.GetId(), 0, GL_RED_INTEGER,
                         GL_UNSIGNED_INT, &clearint));

  VisualBlockInterior &b = Frame->BlockManager->Interior;

  GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

  auto SimplePass = [](auto VertsGetter, VertexArrayObject &VAO,
                       ShaderManager::Shaders ShaderName,
                       GLenum primitive = GL_TRIANGLE_STRIP) {
    if (!VertsGetter().empty()) {
      Shader &shader = ShaderManager::GetShader(ShaderName);

      shader.bind();
      VAO.bind();
      VAO.DrawAs(primitive);
      VAO.unbind();
      shader.unbind();
    }
  };

  //SimplePass([&b]() { return b.GetAndVBO(); }, AndVAO, ShaderManager::And);
  //SimplePass([&b]() { return b.GetOrVBO(); }, OrVAO, ShaderManager::Or);
  //SimplePass([&b]() { return b.GetXOrVBO(); }, XOrVAO, ShaderManager::XOr);
  SimplePass([&b]() { return b.GetPinVBO(); }, PinVAO, ShaderManager::Pin);
  SimplePass([&b]() { return b.GetRoundedPinVBO(); }, RoundPinVAO,
             ShaderManager::RoundPin);
  SimplePass([&b]() { return b.GetAssetVBO(); }, AssetVAO,
             ShaderManager::Assets, GL_POINTS);
  SimplePass([&b]() { return b.GetMuxVBO(); }, MuxVAO, ShaderManager::Mux);

  GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));

  FBOID.unbind();
}

void Renderer::RenderWires() {
  PROFILE_FUNKTION;

  VisualBlockInterior &b = Frame->BlockManager->Interior;

  for (bool Floating : {false, true}) {
    if (Floating) {
      GLCALL(glStencilMask(0xFF));
      GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
      b.UpdateVectsForVAOsFloating(BoundingBox, MouseIndex);
    } else {
      b.UpdateVectsForVAOs(BoundingBox, Zoom, MouseIndex, AllowHover);
    }

    Shader &PathHightlightShader =
        ShaderManager::GetShader(ShaderManager::HighlightPath);

    PathHightlightShader.bind();
    FBOBackgroundTexture.bind(PathHightlightShader, "UBackground", "", 0);

    GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
    GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
    GLCALL(glStencilMask(0xFF));

    // Put Unmarked on Stencil Buffer
    if (b.HasUnmarked(Floating)) {
      GLCALL(glBlendFunc(GL_ONE, GL_ZERO));
      GLCALL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));

      auto &EdgesUnmarkedVAO = GetPathVAOs(Floating).EdgesUnmarkedVAO;

      EdgesUnmarkedVAO.bind();
      b.GetEdgesUnmarked(Floating).replaceBuffer(EdgesUnmarkedVAO, 1);
      EdgesUnmarkedVAO.DrawAs(GL_TRIANGLE_STRIP);
      EdgesUnmarkedVAO.unbind();

      GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
      GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      if (!b.HasMark(Floating)) {
        PathHightlightShader.unbind();

        GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
        GLCALL(glStencilMask(0x00));
        GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      }
    }

    if (b.HasMark(Floating)) {
      auto &EdgesMarkedVAO = GetPathVAOs(Floating).EdgesMarkedVAO;

      // Draw Highlight Stem
      EdgesMarkedVAO.bind();
      b.GetEdgesMarked(Floating).replaceBuffer(EdgesMarkedVAO, 1, !Floating);
      EdgesMarkedVAO.DrawAs(GL_TRIANGLE_STRIP);
      EdgesMarkedVAO.unbind();

      GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
      GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
      GLCALL(glStencilMask(0x00));

      auto &VertsVAO = GetPathVAOs(Floating).VertsVAO;

      if (!b.GetVerts(Floating).empty()) {
        // Draw Roundings at Stencil 0
        VertsVAO.bind();
        b.GetVerts(Floating).replaceBuffer(VertsVAO, 1);
        VertsVAO.DrawAs(GL_TRIANGLE_STRIP);
      }

      PathHightlightShader.unbind();

      if (!b.GetVerts(Floating).empty()) {
        Shader& HighlightPathCornerOverdrawShader =
            ShaderManager::GetShader(
                ShaderManager::HighlightPathCornerOverdraw);

        HighlightPathCornerOverdrawShader.bind();
  
        HighlightPathCornerOverdrawShader.apply("UColor", Shader::Data3f{1.0,1.0,0.0});

        FBOBackgroundTexture.bind(HighlightPathCornerOverdrawShader,
                                  "UBackground", "", 0);

        GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
        GLCALL(glStencilFunc(GL_EQUAL, 2, 0xFF));
        GLCALL(glBlendFunc(GL_ONE, GL_ZERO));

        // Draw Hyperbolas at Stencli 2
        VertsVAO.DrawAs(GL_TRIANGLE_STRIP);
        VertsVAO.unbind();

        HighlightPathCornerOverdrawShader.unbind();
      }

      if (!b.GetConflictPoints(Floating).empty()) {

        GLCALL(glStencilMask(0xFF));
        GLCALL(glClear(GL_STENCIL_BUFFER_BIT));

        GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
        GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
        GLCALL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
        GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        PathHightlightShader.bind();

        EdgesMarkedVAO.bind();
        EdgesMarkedVAO.DrawAs(GL_TRIANGLE_STRIP);
        EdgesMarkedVAO.unbind();

        GLCALL(glStencilMask(0x00));
        GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
        GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

        FBOBackgroundTexture.bind(PathHightlightShader, "UBackground", "",
                                  0);

        auto &ConflictPointsVAO = GetPathVAOs(Floating).ConflictPointsVAO;
        ConflictPointsVAO.bind();
        b.GetConflictPoints(Floating).replaceBuffer(ConflictPointsVAO, 1);
        ConflictPointsVAO.DrawAs(GL_TRIANGLE_STRIP);
        ConflictPointsVAO.unbind();

        GLCALL(glStencilMask(0xFF));
        GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
        GLCALL(glBlendFunc(GL_ONE, GL_ZERO));
        GLCALL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));

        auto &EdgesUnmarkedVAO = GetPathVAOs(Floating).EdgesUnmarkedVAO;

        EdgesUnmarkedVAO.bind();
        EdgesUnmarkedVAO.DrawAs(GL_TRIANGLE_STRIP);
        EdgesUnmarkedVAO.unbind();

        PathHightlightShader.unbind();

        GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
        GLCALL(glStencilMask(0x00));
        GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
        GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
        GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      }

      GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
      GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    }

    // Draw Path Edges
    if (!b.GetEdges(Floating).empty()) {

      Shader &PathShader = ShaderManager::GetShader(ShaderManager::Path);
      
      PathShader.bind();

      auto &EdgesVAO = GetPathVAOs(Floating).EdgesVAO;

      EdgesVAO.bind();
      b.GetEdges(Floating).replaceBuffer(EdgesVAO, 1);
      EdgesVAO.DrawAs(GL_TRIANGLE_STRIP);
      EdgesVAO.unbind();
      PathShader.unbind();
    }

    // Draw Path Rounded Inner Corners
    if (!b.GetSpecialPoints(Floating).empty()) {
      Shader &IntersectionPathShader =
          ShaderManager::GetShader(ShaderManager::IntersectionPath);

      IntersectionPathShader.bind();

      GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
      GLCALL(glStencilFunc(GL_LEQUAL, 2, 0xFF));

      auto &SpecialPointsVAO = GetPathVAOs(Floating).SpecialPointsVAO;

      SpecialPointsVAO.bind();
      b.GetSpecialPoints(Floating).replaceBuffer(SpecialPointsVAO, 1);
      SpecialPointsVAO.DrawAs(GL_TRIANGLE_STRIP);
      SpecialPointsVAO.unbind();

      IntersectionPathShader.unbind();

      GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
      GLCALL(glStencilMask(0x00));
    }
  }

  PROFILE_SCOPE_ID_START("Copy To FBO Path", 2);

  FBOPath.bind(FrameBufferObject::Draw);

  GLCALL(glBlitFramebuffer(
      0, 0, CanvasSize.x(), CanvasSize.y(), 0, 0, CanvasSize.x(),
      CanvasSize.y(), GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST));

  FBOPath.unbind();

  PROFILE_SCOPE_ID_END(2);

  PROFILE_SCOPE_ID_START("Recreate Stencil from Path", 3);

  GLCALL(glStencilMask(0xFF));
  GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
  GLCALL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
  GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
  GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));

  // Recreate Stencli Buffer
  Shader& PathShader = ShaderManager::GetShader(ShaderManager::Path);
  PathShader.bind();
  for (bool Floating : {false, true}) {

    if (b.GetEdges(Floating).empty())
      continue;

    auto &EdgesVAO = GetPathVAOs(Floating).EdgesVAO;

    EdgesVAO.bind();
    // Is importent as it might not have been replaced by drawing wires and the
    // buffer was cleared
    b.GetEdges(Floating).replaceBuffer(EdgesVAO, 1);
    EdgesVAO.DrawAs(GL_TRIANGLE_STRIP);
    EdgesVAO.unbind();
  }
  PathShader.unbind();

  PROFILE_SCOPE_ID_END(3);
}

void Renderer::Render() {
  PROFILE_FUNKTION;
#ifdef HotShaderReload
  if (!Dirty && !ShaderManager::IsDirty)
    return;
#else
  if (!Dirty)
    return;
#endif
  Dirty = false;
  IdMapDirty = true;

#ifdef HotShaderReload
  if (ShaderManager::IsDirty) {
    UpdateViewProjectionMatrix();
    RenderBackground();
    ShaderManager::IsDirty = false;
  }
#endif

  VisualBlockInterior &b = Frame->BlockManager->Interior;

  PROFILE_SCOPE_ID_START("Blit Background", 0);

  // Frame->Canvas->SetCurrent(*App->GlContext.get());
  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  FBOMain.bind(FrameBufferObject::Draw);

  if (UIDRun) {
    UIDRun = false;
    ShaderManager::applyGlobal("UIDRun", Shader::Data1i{UIDRun});
  }

  GLCALL(glStencilMask(0xFF));

  GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

  GLCALL(glViewport(0, 0, CanvasSize.x(), CanvasSize.y()));

  FBOBackground.bind(FrameBufferObject::Read);

  GLCALL(glBlitFramebuffer(0, 0, CanvasSize.x(), CanvasSize.y(), 0, 0,
                           CanvasSize.x(), CanvasSize.y(), GL_COLOR_BUFFER_BIT,
                           GL_NEAREST));

  FBOBackground.unbind();

  PROFILE_SCOPE_ID_END(0);

  RenderWires();

  PROFILE_SCOPE_ID_START("Draw And Or XOR", 4);

  if (!Frame->Canvas->BindContext()) {
      wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  FBOMain.bind(FrameBufferObject::Draw);

  // GLCALL(glDepthMask(GL_TRUE));
  GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
  GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
  GLCALL(glStencilMask(0x00));

  auto SimplePass = [](auto VertsGetter, VertexArrayObject &VAO,
                       ShaderManager::Shaders ShaderName,
                       size_t BufferIndex = 1,
                       GLenum primitive = GL_TRIANGLE_STRIP) {
    if (!VertsGetter().empty()) {
      Shader &shader = ShaderManager::GetShader(ShaderName);

      shader.bind();
      VAO.bind();
      VertsGetter().replaceBuffer(VAO, BufferIndex);
      VAO.DrawAs(primitive);
      VAO.unbind();
      shader.unbind();
    }
  };

  //SimplePass([&b]() { return b.GetAndVBO(); }, AndVAO, ShaderManager::And);
  //SimplePass([&b]() { return b.GetOrVBO(); }, OrVAO, ShaderManager::Or);
  //SimplePass([&b]() { return b.GetXOrVBO(); }, XOrVAO, ShaderManager::XOr);

  PROFILE_SCOPE_ID_END(4);

  PROFILE_SCOPE_ID_START("Pins", 5);

  // GLCALL(glDepthMask(GL_FALSE));

  auto WirePass = [this](auto VertsGetter, VertexArrayObject &VAO,
                         ShaderManager::Shaders ShaderName) {
    if (!VertsGetter().empty()) {

      Shader &shader = ShaderManager::GetShader(ShaderName);

      shader.bind();

      FBOPathColorTexture.bind(shader, "UPath", "", 0);

      VAO.bind();
      VertsGetter().replaceBuffer(VAO, 1);

      GLCALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
      shader.apply("UHasWire",Shader::Data1i{true});
      VAO.DrawAs(GL_TRIANGLE_STRIP);

      GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
      shader.apply("UHasWire",Shader::Data1i{false});
      VAO.DrawAs(GL_TRIANGLE_STRIP);

      VAO.unbind();

      shader.unbind();
    }
  };

  WirePass([&b]() { return b.GetPinVBO(); }, PinVAO, ShaderManager::Pin);
  WirePass([&b]() { return b.GetRoundedPinVBO(); }, RoundPinVAO,
           ShaderManager::RoundPin);

  GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
  GLCALL(glStencilMask(0x00));
  // GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
  // GLCALL(glStencilMask(0x00));

  PROFILE_SCOPE_ID_END(5);

  SimplePass([&b]() { return b.GetAssetVBO(); }, AssetVAO,
             ShaderManager::Assets, 0, GL_POINTS);
  SimplePass([&b]() { return b.GetSevenSegVBO(); }, SevenSegVAO,
             ShaderManager::SevenSeg);
  SimplePass([&b]() { return b.GetSixteenSegVBO(); }, SixteenSegVAO,
             ShaderManager::SixteenSeg);

#ifdef RenderCollisionGrid
  BufferedVertexVec<TwoPointIRGBAIDVertex> Blocks;

  static std::unordered_map<int, std::tuple<float, float, float>> ColourMap;
  auto GetColour = [&](const int &i) {
    if (i == 0)
      return std::make_tuple(1.0f, 1.0f, 1.0f);
    auto it = ColourMap.find(i);
    if (it == ColourMap.end()) {
      ColourMap[i] = std::make_tuple(float(rand()) / (float)RAND_MAX,
                                     float(rand()) / (float)RAND_MAX,
                                     float(rand()) / (float)RAND_MAX);
      return ColourMap[i];
    }
    return it->second;
  };

  for (const auto &pair : b.CollisionMap) {
    MyRectI BB = MyRectI::FromCorners(
        pair.first, pair.first + PointType{VisualBlockInterior::BoxSize,
                                           VisualBlockInterior::BoxSize});
    if (BoundingBox.Intersectes(
            MyRectF(BB.Position.cast<float>(), BB.Size.cast<float>()))) {
      const auto &size = pair.second.size();
      if (size == 0) {
        Blocks.Emplace(0, BB.Position,
                       PointType(BB.Position.x() + BB.Size.x() - 1,
                                 BB.Position.y() + BB.Size.y() - 1),
                       ColourType(0.5, 0.5, 0.5, 0.1), ColourType(0, 0, 0, 0));
      } else {
        auto [r, g, b] = GetColour(size);
        Blocks.Emplace(0, BB.Position,
                       PointType(BB.Position.x() + BB.Size.x() - 1,
                                 BB.Position.y() + BB.Size.y() - 1),
                       // r, g, b, 0.5,
                       ColourType(r, g, b, 0.5), ColourType(0, 0, 0, 0));
      }
    }
  }

  SimplePass([&Blocks]() { return Blocks; }, CollisionGridVAO,
             ShaderManager::Block);
#endif

  // SimplePass([&b]() { return b.GetMuxVerts(); }, MuxVAO, ShaderManager::Mux);

  // auto& shaderAssets = ShaderManager::GetShader(ShaderManager::Assets);

  // shaderAssets.bind();
  // BufferedVertexVec<AssetVertex> assetVBO;
  // assetVBO.append(AssetVertex::Box(0, {0,0}, {2,3}));

  // static VertexArrayObject assetVAO = CreateVAO<AssetVertex>();
  // assetVAO.bind();
  // assetVBO.replaceBuffer(assetVAO, 0);
  // assetVAO.DrawAs(GL_POINTS);
  // assetVAO.unbind();

  // shaderAssets.unbind();
  

  if (!b.GetStaticTextVBO().empty() || !b.GetDynamicTextVBO().empty() ||
      (Frame->Blockselector && !Frame->Blockselector->GetTextVBO().empty())) {

    PROFILE_SCOPE("Text");

    Shader &TextShader = ShaderManager::GetShader(ShaderManager::Text);

    TextShader.bind();

    TextAtlas.bind(TextShader, "UTexture", "", 0);

    if (!b.GetStaticTextVBO().empty()) {
      StaticTextVAO.bind();
      b.GetStaticTextVBO().replaceBuffer(StaticTextVAO, 1);
      StaticTextVAO.DrawAs(GL_TRIANGLE_STRIP);
      StaticTextVAO.unbind();
    }

    if (!b.GetDynamicTextVBO().empty()) {
      DynamicTextVAO.bind();
      b.GetDynamicTextVBO().replaceBuffer(DynamicTextVAO, 1, false);
      DynamicTextVAO.DrawAs(GL_TRIANGLE_STRIP);
      DynamicTextVAO.unbind();
    }

    // Overlay Text
    TextShader.apply("UZoom", Shader::Data2f{
                       0.01f * CanvasSize.x() / 2.0f,
                       0.01f * CanvasSize.y() / 2.0f});
    TextShader.apply("UOffset", Shader::Data2f{0.0,0.0});
    TextShader.apply("UZoomFactor", Shader::Data1f{0.01f});

    if (Frame->Blockselector) {
      Frame->Blockselector->Update();
      auto &Blockselector = *Frame->Blockselector;
      if (!Blockselector.GetTextVBO().empty()) {
        DynamicTextVAO.bind();
        Blockselector.GetTextVBO().replaceBuffer(DynamicTextVAO, 1, false);
        DynamicTextVAO.DrawAs(GL_TRIANGLE_STRIP);
        DynamicTextVAO.unbind();
      }
    }

    TextShader.apply("UZoom", Shader::Data2f{
                            CanvasSize.x() * float(Zoom),
                            CanvasSize.y() * float(Zoom)});
    TextShader.apply("UOffset", Shader::Data2f{Offset.x(),
                       Offset.y()});
    TextShader.apply("UZoomFactor", Shader::Data1f{float(Zoom)});

    TextShader.unbind();
  }

  PROFILE_SCOPE("Swap Buffers");

  /*std::stringstream ss;
  ss << "E: " << b.GetEdges(false).Size();
  ss << ", Em:" << b.GetEdgesMarked(false).Size();
  ss << ", Eum:" << b.GetEdgesUnmarked(false).Size();
  ss << ", Sp:" << b.GetSpecialPoints(false).Size();
  ss << ", V:" << b.GetVerts(false).Size();
  ss << ", hm:" << b.HasMark(false);
  ss << ", hum:" << b.HasUnmarked(false);
  ss << ", PE: " << b.GetEdges(true).Size();
  ss << ", PEm:" << b.GetEdgesMarked(true).Size();
  ss << ", PEum:" << b.GetEdgesUnmarked(true).Size();
  ss << ", PSp:" << b.GetSpecialPoints(true).Size();
  ss << ", PV:" << b.GetVerts(true).Size();
  ss << ", hPm:" << b.HasMark(true);
  ss << ", hPum:" << b.HasUnmarked(true);
  static int TIMES = 0;
  ss << ", T:" << TIMES++;
  ss << ", Zoom:" << Zoom;*/

  // FBOID.unbind();

  title = "Zoom: " + std::to_string(Zoom) +
          " Off: " + std::to_string(Offset.x()) + ", " +
          std::to_string(Offset.y());

  // AreaSelectVerts.Clear();
  // AreaSelectVerts.Emplace(BoundingBox.Position,BoundingBox.Position +
  // BoundingBox.Size,ColourType{1.0,1.0,0.0,0.0}); PointFRGBVertex(const
  // Eigen::Vector2f& p1, const Eigen::Vector2f& p2, const ColourType& c)

  if (!AreaSelectVerts.empty()) {

    PROFILE_SCOPE("AreaSelect");

    Shader& AreaSelectShader =
        ShaderManager::GetShader(ShaderManager::AreaSelect);

    AreaSelectShader.bind();

    AreaSelectVAO.bind();
    AreaSelectVerts.replaceBuffer(AreaSelectVAO, 0, false);
    AreaSelectVAO.DrawAs(GL_POINTS);
    AreaSelectVAO.unbind();

    AreaSelectShader.unbind();
  }

#ifdef ShowBasePositionOfBlocks
  if (!b.GetBasePotitionOfBlocksVBO().empty()) {

    PROFILE_SCOPE("BasePotitionOfBlocks");

    Shader &AreaSelectShader =
        ShaderManager::GetShader(ShaderManager::AreaSelect);

    AreaSelectShader.bind();

    BlockBasePositionVAO.bind();
    b.GetBasePotitionOfBlocksVBO().replaceBuffer(BlockBasePositionVAO, 0);
    BlockBasePositionVAO.DrawAs(GL_POINTS);
    BlockBasePositionVAO.unbind();

    AreaSelectShader.unbind();
  }
#endif

  FBOMain.unbind();
  
  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  FBOMain.bind(FrameBufferObject::Read);

  GLCALL(glBlitFramebuffer(0, 0, CanvasSize.x(), CanvasSize.y(), 0, 0,
                           CanvasSize.x(), CanvasSize.y(), GL_COLOR_BUFFER_BIT,
                           GL_NEAREST));

  FBOMain.unbind();

  Frame->Canvas->SwapBuffers();
}

void Renderer::CheckZoomDots() {

  bool Update = false;

  if (Zoom < ZoomDotsBorder && !ShowDots) {
    Update = true;
    ShowDots = true;
  } else if (Zoom >= ZoomDotsBorder && ShowDots) {
    Update = true;
    ShowDots = false;
  }

  if (!Update)
    return;

  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  Shader &BackgroundShader =
      ShaderManager::GetShader(ShaderManager::Background);

  BackgroundShader.bind();
  BackgroundShader.apply("UShowDots", Shader::Data1ui{ShowDots});
  BackgroundShader.unbind();
}

void Renderer::RenderPlacholder(MyFrame& frame, Eigen::Vector2f CanvasSize) {
  PROFILE_FUNKTION;

  if (!frame.Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  GLCALL(glClear(GL_COLOR_BUFFER_BIT));

  GLCALL(glViewport(0, 0, CanvasSize.x(), CanvasSize.y()));

  auto &Shader = ShaderManager::GetPlacholderShader();
  Shader.bind();
  Shader.apply("USize", Shader::Data2f{CanvasSize.x() / 100.0f,
                     CanvasSize.y() / 100.0f});
  frame.HoleScreenVAO->bind();
  frame.HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
  frame.HoleScreenVAO->unbind();
  Shader.unbind();

  frame.Canvas->SwapBuffers();
}

Renderer::Renderer(MyApp *App, MyFrame *Frame)
    : App(App), Frame(Frame), FBOBackgroundTexture(1, 1),
      FBOBackground({&FBOBackgroundTexture}),
      FBOPathStencileDepthTexture(1, 1, nullptr,
                                  []() {
                                    Texture::Descriptor desc;
                                    desc.Format = GL_DEPTH_STENCIL;
                                    desc.Internal_Format = GL_DEPTH24_STENCIL8;
                                    desc.Depth_Stencil_Texture_Mode =
                                        GL_STENCIL_INDEX;
                                    desc.Type = GL_UNSIGNED_INT_24_8;
                                    return desc;
                                  }()),
      FBOPathColorTexture(1, 1),
      FBOPath({&FBOPathColorTexture, &FBOPathStencileDepthTexture},
              {GL_COLOR_ATTACHMENT0, GL_DEPTH_STENCIL_ATTACHMENT}),
      FBOMainStencileDepthTexture(1, 1, nullptr,
                                  []() {
                                    Texture::Descriptor desc;
                                    desc.Format = GL_DEPTH_STENCIL;
                                    desc.Internal_Format = GL_DEPTH24_STENCIL8;
                                    desc.Depth_Stencil_Texture_Mode =
                                        GL_STENCIL_INDEX;
                                    desc.Type = GL_UNSIGNED_INT_24_8;
                                    return desc;
                                  }()),
      FBOMainColorTexture(1, 1),
      FBOMain({&FBOMainColorTexture, &FBOMainStencileDepthTexture},
              {GL_COLOR_ATTACHMENT0, GL_DEPTH_STENCIL_ATTACHMENT}),
      FBOIDTexture(1, 1, nullptr,
                   []() {
                     Texture::Descriptor desc;
                     desc.Format = GL_RED_INTEGER;
                     desc.Internal_Format = GL_R32UI;
                     desc.Depth_Stencil_Texture_Mode = GL_STENCIL_INDEX;
                     desc.Type = GL_UNSIGNED_INT;
                     return desc;
                   }()),
      FBOID({&FBOIDTexture}, {GL_COLOR_ATTACHMENT1}),
#ifdef RenderCollisionGrid
      CollisionGridVAO(CreateVAO<TwoPointIRGBAIDVertex>()),
#endif
      VAOsPath(PathVAOs{
          .EdgesVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
          .EdgesMarkedVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
          .EdgesUnmarkedVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
          .SpecialPointsVAO = CreateVAOInstancing4<PointIRGBVertex>(),
          .VertsVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
          .ConflictPointsVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
      }),
      VAOsPathPreview(PathVAOs{
          .EdgesVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
          .EdgesMarkedVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
          .EdgesUnmarkedVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
          .SpecialPointsVAO = CreateVAOInstancing4<PointIRGBVertex>(),
          .VertsVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
          .ConflictPointsVAO = CreateVAOInstancing4<TwoPointIRGBRHGHBHVertex>(),
      }),
      SevenSegVAO(CreateVAOInstancing4<SevenSegVertex>()),
      SixteenSegVAO(CreateVAOInstancing4<SixteenSegVertex>()),
      MuxVAO(CreateVAOInstancing4<MuxIDVertex>()),
      AssetVAO(CreateVAO<AssetVertex>()),
      PinVAO(CreateVAOInstancing4<PointIOrientationRGBRHGHBHIDVertex>()),
      RoundPinVAO(CreateVAOInstancing4<PointIRGBIDVertex>()),
      //AndVAO(CreateVAOInstancing4<PointIOrientationRGBIDVertex>()),
      /*NotTriangleVAO(CreatePointIOrientationVAO()),
      NDotVAO(CreatePointIOrientationVAO()),*/
      //OrVAO(CreateVAOInstancing4<PointIOrientationRGBIDVertex>()),
      //XOrVAO(CreateVAOInstancing4<PointIOrientationRGBIDVertex>()),
      AreaSelectVAO(CreateVAO<PointFRGBVertex>()),
#ifdef ShowBasePositionOfBlocks
      BlockBasePositionVAO(CreateVAO<PointFRGBVertex>()),
#endif
      StaticTextVAO(CreateVAOInstancing4<TextVertex>()),
      DynamicTextVAO(CreateVAOInstancing4<TextVertex>()),
      TextAtlas(textureFromWxImage(PngManager::GetPng(PngManager::atlas), []() {
        Texture::Descriptor desc;
        desc.Min_Filter = Texture::Descriptor::Filter_Type::LINEAR;
        desc.Mag_Filter = Texture::Descriptor::Filter_Type::LINEAR;
        return desc;
      }())) {
  BBUpdater = std::thread([this]() { BBUpdaterWork(); });
}

Renderer::~Renderer() {
  Terminating = true;
  BBUpdaterCV.notify_all();
  BBUpdater.join();
}

void Renderer::UpdateSize() {
  PROFILE_FUNKTION;

  auto Size = Frame->Canvas->GetSize();
  const double scale = Frame->Canvas->GetContentScaleFactor();
  CanvasSize = {scale * Size.x, scale * Size.y};

  FBOBackgroundTexture.Resize(CanvasSize.x(), CanvasSize.y());

  FBOPathStencileDepthTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOPathColorTexture.Resize(CanvasSize.x(), CanvasSize.y());

  FBOMainStencileDepthTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOMainColorTexture.Resize(CanvasSize.x(), CanvasSize.y());

  // FBONonDefaultStencilDepthTexture.Resize(CanvasSize.x(),CanvasSize.y());
  // FBONonDefaultColorTexture.Resize(CanvasSize.x(),CanvasSize.y());
  FBOIDTexture.Resize(CanvasSize.x(), CanvasSize.y());

  UpdateViewProjectionMatrix();
  Dirty = true;
  Render();
}

BufferedVertexVec<PointFRGBVertex> &Renderer::GetAreaSelectVerts() {
  return AreaSelectVerts;
}

void Renderer::UpdateMouseIndex(const PointType &newMouseIndex) {

  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  Shader &BackgroundShader =
      ShaderManager::GetShader(ShaderManager::Background);

  BackgroundShader.bind();
  BackgroundShader.apply("UMousePos", Shader::Data2f{float(newMouseIndex.x()),
    float(newMouseIndex.y())});

  RenderBackground();

  MouseIndex = newMouseIndex;

  Dirty = true;
  Render();
}

unsigned int Renderer::GetHighlited(const Eigen::Vector2f &Mouse) {

  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  RenderIDMap();

  FBOID.bind(FrameBufferObject::Read);

  GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

  GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT1));

  auto Size = Frame->Canvas->GetSize();

  // std::array<GLushort,4> s;
  GLuint i;
  GLCALL(glReadPixels(Mouse.x(), Size.y - Mouse.y(), 1, 1, GL_RED_INTEGER,
                      GL_UNSIGNED_INT, &i));

  GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));

  FBOID.unbind();

  // title.clear();

  // std::stringstream ss;
  // ss << "R: " << s[0] << " G: " << s[1] << " B: " << s[2] << " A: " << s[3];
  // title = ss.str();

  return i;
}

const std::array<MyRectF, 4> &
Renderer::GetBlockBoundingBoxes(const CompressedBlockDataIndex &cbdi) {
  PROFILE_FUNKTION;
  auto it = BlockBoundingBoxes.find(cbdi);
  if (it != BlockBoundingBoxes.end())
    return it->second;

  auto AltZoom = Zoom;
  auto AltOffset = Offset;
  auto AltCanvasSize = CanvasSize;
  // auto AltMouseIndex = MouseIndex;

  Offset = Eigen::Vector2f{0, 0};
  float TargetZoom = 0.01f;

  Zoom = TargetZoom;

  // UpdateMouseIndex({ -1,-1 });

  // Also binds context
  // UpdateViewProjectionMatrix();

  auto &SB = Frame->BlockManager->GetSpecialBlockIndex();

  const auto &ContainedExteriorOpt =
      Frame->BlockManager->GetCompressedBlockData(cbdi);
  if (!ContainedExteriorOpt) {
    assert(false && "you messed up");
  }
  const auto &ContainedExterior = ContainedExteriorOpt.value();
  const auto &BlockSize = ContainedExterior.blockExteriorData.Size;

  auto rectVertical =
      MyRectF::FromCorners(Eigen::Vector2f{-1.5, 1.5},
                           Eigen::Vector2f{BlockSize.x(), -BlockSize.y()} +
                               Eigen::Vector2f{1.5, -1.5});

  // auto rectHorizontal =
  //     MyRectF::FromCorners(Eigen::Vector2f{-1.5, 1.5},
  //                          Eigen::Vector2f{BlockSize.y(), -BlockSize.x()} +
  //                              Eigen::Vector2f{1.5, -1.5});

  if (!UIDRun) {
    UIDRun = true;
    ShaderManager::applyGlobal("UIDRun", Shader::Data1i{UIDRun});
  }

  // auto Rotation = MyDirection::Up;

  PointType ViewportSize;
  // if (Rotation == MyDirection::Left || Rotation == MyDirection::Right) {
  //   ViewportSize = {int(rectHorizontal.Size.x()) * 1.0 / Zoom,
  //                   int(rectHorizontal.Size.y()) * 1.0 / Zoom};
  // } else {
    ViewportSize = {int(rectVertical.Size.x()) * 1.0 / Zoom,
                    int(rectVertical.Size.y()) * 1.0 / Zoom};
  // }

  FBOIDTexture.Resize(ViewportSize.x(), ViewportSize.y());

  CanvasSize = Eigen::Vector2f{ViewportSize.x(), ViewportSize.y()};

  UpdateViewProjectionMatrix(true);

  FBOID.bind();

  GLCALL(glViewport(0, 0, ViewportSize.x(), ViewportSize.y()));

  GLuint clearint = 0;
  GLCALL(glClearTexImage(FBOIDTexture.GetId(), 0, GL_RED_INTEGER,
                         GL_UNSIGNED_INT, &clearint));

  GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

  auto SimplePass = [](auto &MyBuffer,
                       VertexArrayObject &VAO,
                       ShaderManager::Shaders ShaderName,
                       size_t BufferIndex = 1,
                       GLenum primitive = GL_TRIANGLE_STRIP) {
    Shader &shader = ShaderManager::GetShader(ShaderName);

    shader.bind();
    VAO.bind();
    MyBuffer.replaceBuffer(VAO, BufferIndex);
    VAO.DrawAs(primitive);
    VAO.unbind();
    shader.unbind();
  };

  ColourType NoColor = {0, 0, 0, 0};

  BlockMetadata Meta;
  Meta.Pos = {};
  Meta.Rotation = MyDirection::Up;

  PointType TopLeft;
  PointType BottomRight;
  // if (Rotation == MyDirection::Left || Rotation == MyDirection::Right) {
  //   // Meta.Pos =
  //   TopLeft = Meta.Pos +
  //             PointType{-1, -1} * int((BlockSize.y() - BlockSize.x()) / 2.0);
  //   BottomRight = TopLeft + PointType{BlockSize.y(), -BlockSize.x()};
  // } else {
    // Meta.Pos =
    // TopLeft = Meta.Pos;
    TopLeft = {};
    BottomRight = TopLeft + PointType{BlockSize.x(), -BlockSize.y()};
  // }

  if (cbdi == SB.And || cbdi == SB.Or || cbdi == SB.Xor) {
    //if (cbdi == SB.And) {
    //  BufferedVertexVec<PointIOrientationRGBIDVertex> AndBuffer;
    //  AndBuffer.emplace(1u, Meta, NoColor);
    //  SimplePass(AndBuffer, AndVAO, ShaderManager::And);
    //}
    //if (cbdi == SB.Or) {
    //  //BufferedVertexVec<PointIOrientationRGBIDVertex> OrBuffer;
    //  //OrBuffer.emplace(1u, Meta, NoColor);
    //  //SimplePass(OrBuffer, OrVAO, ShaderManager::Or);
    //}
    //if (cbdi == SB.Xor) {
    //  //BufferedVertexVec<PointIOrientationRGBIDVertex> AssetVBO;
    //  //XOrBuffer.emplace(1u, Meta, NoColor);
    //  //SimplePass(XOrBuffer, XorVAO, ShaderManager::XOr);
    //  //AssetVBO.append(AssetVertex::Gate(0, TopLeft, BottomRight, NoColor));
    //  //SimplePass(AssetVBO, AssetVAO, ShaderManager::Assets, 0, GL_POINTS);
    //}

    BufferedVertexVec<PointIRGBIDVertex> RoundedPinBuffer;
    for (const auto &Pin : ContainedExterior.blockExteriorData.InputPin) {
      RoundedPinBuffer.emplace(
          1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1),
          NoColor);
    }
    for (const auto &Pin : ContainedExterior.blockExteriorData.OutputPin) {
      RoundedPinBuffer.emplace(
          1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1),
          NoColor);
    }
    SimplePass(RoundedPinBuffer, RoundPinVAO,
               ShaderManager::RoundPin);
  } else if (cbdi == SB.Mux) {
    BufferedVertexVec<MuxIDVertex> MuxBuffer;
    MuxBuffer.emplace(1u, Meta, 2, 0, NoColor);
    SimplePass(MuxBuffer, MuxVAO, ShaderManager::Mux);

    BufferedVertexVec<PointIOrientationRGBRHGHBHIDVertex> PinBuffer;
    for (const auto &Pin : ContainedExterior.blockExteriorData.InputPin) {
      PinBuffer.emplace(
          1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1),
          VisualBlockInterior::GetPinRotation(Meta, Pin), NoColor, NoColor);
    }
    for (const auto &Pin : ContainedExterior.blockExteriorData.OutputPin) {
      PinBuffer.emplace(
          1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1),
          VisualBlockInterior::GetPinRotation(Meta, Pin), NoColor, NoColor);
    }
    SimplePass(PinBuffer, PinVAO, ShaderManager::Pin);
  } else {
    //BufferedVertexVec<AssetVertex> AssetVBO;
    //AssetVBO.append(AssetVertex::Box(0, TopLeft, BottomRight, NoColor));
    //// BlockBuffer.emplace(1u, TopLeft, BottomRight, NoColor, NoColor);
    //SimplePass(AssetVBO, AssetVAO, ShaderManager::Assets, 0, GL_POINTS);

    BufferedVertexVec<PointIOrientationRGBRHGHBHIDVertex> PinBuffer;
    for (const auto &Pin : ContainedExterior.blockExteriorData.InputPin) {
      PinBuffer.emplace(
          1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1),
          VisualBlockInterior::GetPinRotation(Meta, Pin), NoColor, NoColor);
    }
    for (const auto &Pin : ContainedExterior.blockExteriorData.OutputPin) {
      PinBuffer.emplace(
          1u, VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1),
          VisualBlockInterior::GetPinRotation(Meta, Pin), NoColor, NoColor);
    }
    SimplePass(PinBuffer, PinVAO, ShaderManager::Pin);
  }
  FBOID.unbind();

  FBOID.bind(FrameBufferObject::Read);

  GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

  GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT1));

  std::unique_ptr<std::vector<GLuint>> vec =
      std::make_unique<std::vector<GLuint>>();
  vec->resize(ViewportSize.x() * ViewportSize.y());
  GLCALL(glReadPixels(0, 0, ViewportSize.x(), ViewportSize.y(), GL_RED_INTEGER,
                      GL_UNSIGNED_INT, vec->data()));

  GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));
  FBOID.unbind();

  MyRectF rect = MyRectF::FromCorners({0, 0}, {1, 1});
  BlockBoundingBoxes[cbdi] = {rect, rect, rect, rect};

  std::unique_lock ul_Funcs(BBUpdateFuncsMutex);

  BBUpdateFuncs.push([vec = vec.release(), ViewportSize, TargetZoom, BlockSize,
                      this, SB, cbdi]() {
    PROFILE_FUNKTION;

    int xmin = ViewportSize.x();
    int xmax = -1;
    int ymin = ViewportSize.y();
    int ymax = -1;

    for (int y = 0; y < ViewportSize.y(); y++) {
      for (int x = 0; x < ViewportSize.x(); x++) {
        if ((*vec)[y * ViewportSize.x() + x] != 1)
          continue;
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
    Eigen::Vector2f Point1{xmin * factor, -ymin * factor};
    Eigen::Vector2f Point2{xmax * factor, -ymax * factor};

    auto Rotate = [](float angle) {
      return Eigen::Matrix2f{{std::round(cos(angle)), -std::round(sin(angle))},
                             {std::round(sin(angle)), std::round(cos(angle))}};
    };

    auto rectDown =
        MyRectF::FromCorners(Rotate(0) * Point1, Rotate(0) * Point2);
    auto rectRight = MyRectF::FromCorners(Rotate((float)M_PI / 2.0f) * Point1,
                                          Rotate((float)M_PI / 2.0f) * Point2);
    auto rectUp = MyRectF::FromCorners(Rotate((float)M_PI) * Point1,
                                       Rotate((float)M_PI) * Point2);
    auto rectLeft =
        MyRectF::FromCorners(Rotate(3.0f * (float)M_PI / 2.0f) * Point1,
                             Rotate(3.0f * (float)M_PI / 2.0f) * Point2);

    auto off = PointType{-1, -1} * int((BlockSize.y() - BlockSize.x()) / 2.0);
    auto Off = Eigen::Vector2f{off.x(), off.y()};

    Off = {0, 0};

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

  return BlockBoundingBoxes.at(cbdi);
}

void Renderer::BBUpdaterWork() {
  PROFILE_FUNKTION;
  while (!Terminating) {
    std::unique_lock ul(BBUpdaterCVMutex);
    BBUpdaterCV.wait(ul, [this]() {
      if (Terminating)
        return true;
      std::unique_lock ul(BBUpdateFuncsMutex);
      return !BBUpdateFuncs.empty();
    });
    if (Terminating)
      return;
    std::unique_lock ul_Funcs(BBUpdateFuncsMutex);
    if (BBUpdateFuncs.empty())
      continue;
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

// Returns if finished
bool Renderer::StepGoHome() {
  GoHomeFrame++;

  double TargetZoom = 0.01;
  Eigen::Vector2f TargetOffset = {0, 0};

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
