#include "BlockAndPathData.hpp"
#include "pch.hpp"

#include "Renderer.hpp"

#include "MyApp.hpp"

#include "MyFrame.hpp"
#include "PngManager.hpp"
#include "ShaderManager.hpp"

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
                       ShaderManager::Shaders ShaderName) {
    if (!VertsGetter().empty()) {
      Shader &shader = ShaderManager::GetShader(ShaderName);

      shader.bind();
      VAO.bind();
      VAO.DrawAs(GL_POINTS);
      VAO.unbind();
      shader.unbind();
    }
  };

  SimplePass([&b]() { return b.GetPinVBO(); }, PinVAO, ShaderManager::Assets);
  SimplePass([&b]() { return b.GetAssetVBO(); }, AssetVAO,
             ShaderManager::Assets);
  SimplePass([&b]() { return b.GetRoundPinVBO(); }, RoundPinVAO, ShaderManager::Assets);

  GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));

  FBOID.unbind();
}

void Renderer::RenderWires() {
  PROFILE_FUNKTION;

  VisualBlockInterior &b = Frame->BlockManager->Interior;
   
  FBOMain.bind(FrameBufferObject::Draw);

  Shader& AssetShader = ShaderManager::GetShader(ShaderManager::Assets);
  AssetShader.bind();

  const auto Pass = [&AssetShader, this](
     PathVAOs& VAOs,
     BufferedVertexVec<AssetVertex>& edges,
     BufferedVertexVec<AssetVertex>& verts,
     BufferedVertexVec<AssetVertex>& intersectionPoints,
     bool BlurRun,
     AssetVertex::Flags flag = AssetVertex::None){
    if(edges.empty()) return;
    GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
    GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
    GLCALL(glStencilMask(0xFF));
    
    VAOs.EdgesVAO.bind();
    edges.replaceBuffer(VAOs.EdgesVAO, 0);
    VAOs.EdgesVAO.DrawAs(GL_POINTS);
    if (!BlurRun) {
        VAOs.EdgesVAO.unbind();
    }
    else {
        AssetShader.apply("UHighlight", Shader::Data1ui{ flag });
        GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
        GLCALL(glStencilMask(0x00));
        VAOs.EdgesVAO.DrawAs(GL_POINTS);
        VAOs.EdgesVAO.unbind();
    }

    GLCALL(glStencilMask(0x00));
    GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));

    VAOs.VertsVAO.bind();
    verts.replaceBuffer(VAOs.VertsVAO, 0);
    VAOs.VertsVAO.DrawAs(GL_POINTS);
    VAOs.VertsVAO.unbind();

    GLCALL(glStencilFunc(GL_EQUAL, 2, 0xFF));

    if (!BlurRun)
        FBOBackgroundTexture.bind(AssetShader,"UBackground","", 0);

    VAOs.IntersectionPointsVAO.bind();
    intersectionPoints.replaceBuffer(VAOs.IntersectionPointsVAO, 0);
    VAOs.IntersectionPointsVAO.DrawAs(GL_POINTS);
    VAOs.IntersectionPointsVAO.unbind();

    if (BlurRun) {
        AssetShader.apply("UHighlight", Shader::Data1ui{ 0 });
    }

    if (!BlurRun)
        FBOBackgroundTexture.unbind();
  };

  b.UpdateVectsForVAOs(BoundingBox, Zoom, MouseIndex, AllowHover);
  Pass(GetPathVAOs(false), b.GetEdges(false), b.GetVerts(false), b.GetIntersectionPoints(false), false);

  b.UpdateVectsForVAOsFloating(BoundingBox, MouseIndex);
  
  if (b.HasPreview()) {
      GLCALL(glStencilMask(0xFF));
      GLCALL(glClear(GL_STENCIL_BUFFER_BIT));

      Pass(GetPathVAOs(true), b.GetEdges(true), b.GetVerts(true), b.GetIntersectionPoints(true), false);
  }

  //Preview
  GLuint clearint = 0;
  GLCALL(glClearTexImage(FBOBlurPreviewTexture.GetId(), 0, GL_RED_INTEGER,
      GL_UNSIGNED_INT, &clearint));

  FBOBlurPreview.bind(FrameBufferObject::BindMode::Draw);

  GLCALL(glStencilMask(0xFF));
  GLCALL(glClear(GL_STENCIL_BUFFER_BIT));

  if (b.HasPreview()) {
      GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

      Pass(GetPathVAOs(true), b.GetEdges(true), b.GetVerts(true), b.GetIntersectionPoints(true), true, AssetVertex::Preview);

      GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));
      AssetShader.apply("UHighlight", Shader::Data1ui{ 0 });
  }

  FBOBlurPreview.unbind();

  //Highlight
  GLCALL(glClearTexImage(FBOBlurHighlightTexture.GetId(), 0, GL_RED_INTEGER,
      GL_UNSIGNED_INT, &clearint));

  FBOBlurHighlight.bind(FrameBufferObject::BindMode::Draw);

  GLCALL(glStencilMask(0xFF));
  GLCALL(glClear(GL_STENCIL_BUFFER_BIT));

  if (b.HasHighlitedPath()) {

      GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

      Pass(GetPathVAOs(false), b.GetEdges(false), b.GetVerts(false), b.GetIntersectionPoints(false), true, AssetVertex::Highlight);

      GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));
      AssetShader.apply("UHighlight", Shader::Data1ui{ 0 });
  }

  FBOBlurHighlight.unbind();

  //Marked
  GLCALL(glClearTexImage(FBOBlurMarkedTexture.GetId(), 0, GL_RED_INTEGER,
      GL_UNSIGNED_INT, &clearint));

  FBOBlurMarked.bind(FrameBufferObject::BindMode::Draw);

  GLCALL(glStencilMask(0xFF));
  GLCALL(glClear(GL_STENCIL_BUFFER_BIT));

  if (!b.GetEdgesMarked(false).empty()) {

      GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));
      Pass(GetPathVAOs(false), b.GetEdgesMarked(false), b.GetVerts(false), b.GetIntersectionPoints(false), true, AssetVertex::Marked);
      GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));
      AssetShader.apply("UHighlight", Shader::Data1ui{ 0 });
  }

  FBOBlurHighlight.unbind();

  AssetShader.unbind();

  PROFILE_SCOPE_ID_START("Copy To FBO Path", 2);

  FBOMain.bind(FrameBufferObject::Read);
  FBOPath.bind(FrameBufferObject::Draw);

  GLCALL(glBlitFramebuffer(
      0, 0, CanvasSize.x(), CanvasSize.y(), 0, 0, CanvasSize.x(),
      CanvasSize.y(), GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST));

  FBOPath.unbind();
  FBOMain.unbind();

  PROFILE_SCOPE_ID_END(2);

  PROFILE_SCOPE_ID_START("Recreate Stencil from Path", 3);

  GLCALL(glStencilMask(0xFF));
  GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
  GLCALL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
  GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
  GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));

  FBOMain.bind(FrameBufferObject::Draw);

  // Recreate Stencli Buffer
  Shader& AssetsShader = ShaderManager::GetShader(ShaderManager::Assets);
  AssetsShader.bind();
  for (bool Floating : {false, true}) {

    if (b.GetEdges(Floating).empty())
      continue;

    auto &EdgesVAO = GetPathVAOs(Floating).EdgesVAO;

    EdgesVAO.bind();
    b.GetEdges(Floating).replaceBuffer(EdgesVAO, 0);
    EdgesVAO.DrawAs(GL_POINTS);
    EdgesVAO.unbind();

    auto& VertsVAO = GetPathVAOs(Floating).VertsVAO;

    VertsVAO.bind();
    b.GetVerts(Floating).replaceBuffer(VertsVAO, 0);
    VertsVAO.DrawAs(GL_POINTS);
    VertsVAO.unbind();
  }
  AssetsShader.unbind();

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

  GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
  GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
  GLCALL(glStencilMask(0x00));

  auto& assetShader = ShaderManager::GetShader(ShaderManager::Assets);

  assetShader.bind();

  auto SimplePass = [](auto VertsGetter, VertexArrayObject &VAO) {
    if (!VertsGetter().empty()) {
      VAO.bind();
      VertsGetter().replaceBuffer(VAO, 0);
      VAO.DrawAs(GL_POINTS);
      VAO.unbind();
    }
  };

  auto WirePass = [this,&assetShader](auto VertsGetter, VertexArrayObject &VAO) {
    if (!VertsGetter().empty()) {
      VAO.bind();
      VertsGetter().replaceBuffer(VAO, 0);  

      assetShader.apply("UWirePass", Shader::Data1i(false));
      GLCALL(glStencilFunc(GL_EQUAL, 0, 0xFF));
      VAO.DrawAs(GL_POINTS);

      assetShader.apply("UWirePass", Shader::Data1i(true));
      FBOPathColorTexture.bind(assetShader,"UPath", "", 0);

      GLCALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
      VAO.DrawAs(GL_POINTS);
      GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));

      FBOPathColorTexture.bind();
      assetShader.apply("UWirePass", Shader::Data1i(false));
  
      VAO.unbind();
    }
  };


  WirePass([&b](){return b.GetPinVBO(); }, PinVAO);

  SimplePass([&b]() { return b.GetAssetVBO(); }, AssetVAO);

  WirePass([&b](){return b.GetRoundPinVBO(); }, RoundPinVAO);


  if (b.HasHighlited()) {
      FBOBlurHighlight.bind(FrameBufferObject::BindMode::Draw);

      assetShader.apply("UHighlight", Shader::Data1ui{ AssetVertex::Flags::Highlight });

      GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

      SimplePass([&b]() {return b.GetHighlightAssetVBO();}, HighlightAssetVAO);

      GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));
      
      assetShader.apply("UHighlight", Shader::Data1ui{ 0 });

      FBOBlurHighlight.unbind();

      if(b.GetMarkedAssetVBO().empty())
        FBOMain.bind(FrameBufferObject::Draw);
  }

  if (!b.GetMarkedAssetVBO().empty()) {
      FBOBlurMarked.bind(FrameBufferObject::BindMode::Draw);

      assetShader.apply("UHighlight", Shader::Data1ui{ AssetVertex::Flags::Marked});

      GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

      SimplePass([&b]() {return b.GetMarkedAssetVBO(); }, MarkedAssetVAO);

      GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));

      assetShader.apply("UHighlight", Shader::Data1ui{ 0 });

      FBOBlurMarked.unbind();

      FBOMain.bind(FrameBufferObject::Draw);
  }

  assetShader.unbind();

  PROFILE_SCOPE_ID_END(4);

  PROFILE_SCOPE_ID_START("Pins", 5);

  GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
  GLCALL(glStencilMask(0x00));
  // GLCALL(glStencilFunc(GL_ALWAYS, 0, 0x00));
  // GLCALL(glStencilMask(0x00));

  PROFILE_SCOPE_ID_END(5);

#ifdef RenderCollisionGrid
  BufferedVertexVec<AssetVertex> Blocks;

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

      //const auto& Pos1 = Eigen::Vector2i{std::max(BB.Position.x(),BB.Position.x() + BB.Size.x() - 1), std::max(BB.Position.y(), BB.Position.y() + BB.Size.y() - 1)};
      //const auto& Pos2 = Eigen::Vector2i{std::min(BB.Position.x(),BB.Position.x() + BB.Size.x() - 1), std::min(BB.Position.y(), BB.Position.y() + BB.Size.y() - 1)};
      
      BB.Position.y() += BB.Size.y() - 1;

      PointType Pos1 = BB.Position;
      Pos1.y() -= BB.Size.y() - 1;
      PointType Pos2 = BB.Position;
      Pos2.x() += BB.Size.x() - 1;

      if (size == 0) {
        BlockMetadata Meta{};
        Blocks.append(AssetVertex::Box(Meta.Transform(), Pos1, Pos2, ColourType(0.5, 0.5, 0.5, 0.5), 0, 0));
      } else {
        BlockMetadata Meta{};
        auto [cr, cg, cb] = GetColour(size);
        Blocks.append(AssetVertex::Box(Meta.Transform(), Pos1, Pos2, ColourType(cr, cg, cb, 0.5), 0, 0));
      }
    }
  }

  SimplePass([&Blocks]() { return Blocks; }, CollisionGridVAO);
#endif

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

  title = "Zoom: " + std::to_string(Zoom) +
          " Off: " + std::to_string(Offset.x()) + ", " +
          std::to_string(Offset.y());

  auto& liquidGlassShader = ShaderManager::GetShader(ShaderManager::LiquidGlass);
  liquidGlassShader.bind();
  Frame->HoleScreenVAO->bind();

  FBOBlurMarkedTexture.bind(liquidGlassShader, "UBluredBase", "", 0);
  liquidGlassShader.apply("UColor", Shader::Data3f{1.0, 0.0, 1.0});
  Frame->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
  FBOBlurMarkedTexture.unbind();

  FBOBlurHighlightTexture.bind(liquidGlassShader, "UBluredBase", "", 0);
  liquidGlassShader.apply("UColor", Shader::Data3f{1.0, 1.0, 0.0});
  Frame->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
  FBOBlurHighlightTexture.unbind();

  FBOBlurPreviewTexture.bind(liquidGlassShader, "UBluredBase", "", 0);
  liquidGlassShader.apply("UColor", Shader::Data3f{0.0, 1.0, 0.0});
  Frame->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
  FBOBlurPreviewTexture.unbind();

  Frame->HoleScreenVAO->unbind();
  liquidGlassShader.unbind();

  if (!AreaSelectVerts.empty()) {

    PROFILE_SCOPE("AreaSelect");

    assetShader.bind();

    AreaSelectVAO.bind();
    AreaSelectVerts.replaceBuffer(AreaSelectVAO, 0);
    AreaSelectVAO.DrawAs(GL_POINTS);
    AreaSelectVAO.unbind();

    assetShader.unbind();
  }

#ifdef ShowBasePositionOfBlocks
  if (!b.GetBasePotitionOfBlocksVBO().empty()) {

    PROFILE_SCOPE("BasePotitionOfBlocks");

    assetShader.bind();

    BlockBasePositionVAO.bind();
    b.GetBasePotitionOfBlocksVBO().replaceBuffer(BlockBasePositionVAO, 0);
    BlockBasePositionVAO.DrawAs(GL_POINTS);
    BlockBasePositionVAO.unbind();

    assetShader.unbind();
  }
#endif

#ifdef ShowBoundingBoxes
  if(!b.GetBBVBO().empty()){
    assetShader.bind();

    BBVAO.bind();
    b.GetBBVBO().replaceBuffer(BBVAO, 0);
    BBVAO.DrawAs(GL_POINTS);
    BBVAO.unbind();
    
    assetShader.unbind();
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
      FBOPathColorTexture(1, 1),
      FBOPath({&FBOPathColorTexture},
              {GL_COLOR_ATTACHMENT0}),
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
      FBOBlurHighlightTexture(CreateBlurTexture()),
      FBOBlurHighlightStencileDepthTexture(CreateBlurStencileDepthTexture()),
      FBOBlurHighlight({&FBOBlurHighlightTexture, &FBOBlurHighlightStencileDepthTexture }, {GL_COLOR_ATTACHMENT1, GL_DEPTH_STENCIL_ATTACHMENT}),
      FBOBlurPreviewTexture(CreateBlurTexture()),
      FBOBlurPreviewStencileDepthTexture(CreateBlurStencileDepthTexture()),
      FBOBlurPreview({&FBOBlurPreviewTexture, &FBOBlurPreviewStencileDepthTexture}, {GL_COLOR_ATTACHMENT1, GL_DEPTH_STENCIL_ATTACHMENT}),
      FBOBlurMarkedTexture(CreateBlurTexture()),
      FBOBlurMarkedStencileDepthTexture(CreateBlurStencileDepthTexture()),
      FBOBlurMarked({&FBOBlurMarkedTexture, &FBOBlurMarkedStencileDepthTexture}, {GL_COLOR_ATTACHMENT1, GL_DEPTH_STENCIL_ATTACHMENT}),
      VAOsPath(PathVAOs{
          .EdgesVAO = CreateVAO<AssetVertex>(),
          .EdgesMarkedVAO = CreateVAO<AssetVertex>(),
          .IntersectionPointsVAO = CreateVAO<AssetVertex>(),
          .VertsVAO = CreateVAO<AssetVertex>(),
      }),
      VAOsPathPreview(PathVAOs{
          .EdgesVAO = CreateVAO<AssetVertex>(),
          .EdgesMarkedVAO = CreateVAO<AssetVertex>(),
          .IntersectionPointsVAO = CreateVAO<AssetVertex>(),
          .VertsVAO = CreateVAO<AssetVertex>(),
      }),
#ifdef RenderCollisionGrid
      CollisionGridVAO(CreateVAO<AssetVertex>()),
#endif
      AssetVAO(CreateVAO<AssetVertex>()),
      PinVAO(CreateVAO<AssetVertex>()),
      RoundPinVAO(CreateVAO<AssetVertex>()),
      HighlightAssetVAO(CreateVAO<AssetVertex>()),
      MarkedAssetVAO(CreateVAO<AssetVertex>()),
      AreaSelectVAO(CreateVAO<AssetFVertex>()),
#ifdef ShowBoundingBoxes
      BBVAO(CreateVAO<AssetFVertex>()),
#endif
#ifdef ShowBasePositionOfBlocks
      BlockBasePositionVAO(CreateVAO<AssetFVertex>()),
#endif
      StaticTextVAO(CreateVAOInstancing4<TextVertex>()),
      DynamicTextVAO(CreateVAOInstancing4<TextVertex>()),
      TextAtlas(textureFromWxImage(PngManager::GetPng(PngManager::atlas), []() {
        Texture::Descriptor desc;
        desc.Min_Filter = Texture::Descriptor::Filter_Type::LINEAR;
        desc.Mag_Filter = Texture::Descriptor::Filter_Type::LINEAR;
        return desc;
      }())) {

  for(const auto& t : {&FBOBlurHighlightTexture, &FBOBlurPreviewTexture, &FBOBlurMarkedTexture}){
    t->bind();
    GLuint borderColor[] = {0u};
    glTexParameterIuiv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    t->unbind();
  }
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

  FBOPathColorTexture.Resize(CanvasSize.x(), CanvasSize.y());

  FBOMainStencileDepthTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOMainColorTexture.Resize(CanvasSize.x(), CanvasSize.y());

  FBOIDTexture.Resize(CanvasSize.x(), CanvasSize.y());

  FBOBlurHighlightTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOBlurHighlightStencileDepthTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOBlurPreviewTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOBlurPreviewStencileDepthTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOBlurMarkedTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOBlurMarkedStencileDepthTexture.Resize(CanvasSize.x(), CanvasSize.y());

  UpdateViewProjectionMatrix();
  Dirty = true;
  Render();
}

BufferedVertexVec<AssetFVertex> &Renderer::GetAreaSelectVerts() {
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

  GLuint i;
  GLCALL(glReadPixels(Mouse.x(), Size.y - Mouse.y(), 1, 1, GL_RED_INTEGER,
                      GL_UNSIGNED_INT, &i));

  GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));

  FBOID.unbind();

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

  Offset = Eigen::Vector2f{0, 0};
  float TargetZoom = 0.01f;

  Zoom = TargetZoom;

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

  if (!UIDRun) {
    UIDRun = true;
    ShaderManager::applyGlobal("UIDRun", Shader::Data1i{UIDRun});
  }

  PointType ViewportSize = {int(rectVertical.Size.x()) * 1.0 / Zoom,
                    int(rectVertical.Size.y()) * 1.0 / Zoom};

  FBOIDTexture.Resize(ViewportSize.x(), ViewportSize.y());

  CanvasSize = Eigen::Vector2f{ViewportSize.x(), ViewportSize.y()};

  UpdateViewProjectionMatrix(true);

  FBOID.bind();

  GLCALL(glViewport(0, 0, ViewportSize.x(), ViewportSize.y()));

  GLuint clearint = 0;
  GLCALL(glClearTexImage(FBOIDTexture.GetId(), 0, GL_RED_INTEGER,
                         GL_UNSIGNED_INT, &clearint));

  GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

  // auto SimplePass = [](auto &MyBuffer,
  //                      VertexArrayObject &VAO,
  //                      ShaderManager::Shaders ShaderName,
  //                      size_t BufferIndex = 1,
  //                      GLenum primitive = GL_TRIANGLE_STRIP) {
  //   Shader &shader = ShaderManager::GetShader(ShaderName);

  //   shader.bind();
  //   VAO.bind();
  //   MyBuffer.replaceBuffer(VAO, BufferIndex);
  //   VAO.DrawAs(primitive);
  //   VAO.unbind();
  //   shader.unbind();
  // };

  ColourType NoColor = {0.0, 0.0, 0.0, 1.0};

  BlockMetadata Meta;
  Meta.Pos = {};
  Meta.Rotation = MyDirection::Up;

  PointType Pos1 = {};
  Pos1.y() -= BlockSize.y();
  PointType Pos2 = {};
  Pos2.x() += BlockSize.x();

  BufferedVertexVec<AssetVertex> VBO;

  if (cbdi == SB.And || cbdi == SB.Or || cbdi == SB.Xor) {
    if (cbdi == SB.And) VBO.append(AssetVertex::Gate(AssetVertex::ID::And, Meta.Transform(), Meta.Pos, 1, 0));
    else if (cbdi == SB.Or) VBO.append(AssetVertex::Gate(AssetVertex::ID::Or, Meta.Transform(), Meta.Pos, 1, 0));
    else if (cbdi == SB.Xor) VBO.append(AssetVertex::Gate(AssetVertex::ID::Xor, Meta.Transform(), Meta.Pos, 1, 0));

    for (const auto& Pin : ContainedExterior.blockExteriorData.InputPin) {
        BlockMetadata PinMeta;
        PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
        VBO.append(AssetVertex::RoundPin(true, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), 1, 0));
    }
    for (const auto& Pin : ContainedExterior.blockExteriorData.OutputPin) {
        BlockMetadata PinMeta;
        PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
        VBO.append(AssetVertex::RoundPin(false, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), 1, 0));
    }
  } else if (cbdi == SB.Mux) {
      VBO.append(AssetVertex::Mux(Meta.Transform(), 1, Meta.Pos, NoColor, 1, 0));

      for (const auto& Pin : ContainedExterior.blockExteriorData.InputPin) {
          BlockMetadata PinMeta;
          PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
          VBO.append(AssetVertex::Pin(true, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor, 1, 0));
      }
      for (const auto& Pin : ContainedExterior.blockExteriorData.OutputPin) {
          BlockMetadata PinMeta;
          PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
          VBO.append(AssetVertex::Pin(false, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor, 1, 0));
      }
  } 
  else {
      VBO.append(AssetVertex::Box(Meta.Transform(), Pos1, Pos2, NoColor, 1, 0));
      for (const auto& Pin : ContainedExterior.blockExteriorData.InputPin) {
          BlockMetadata PinMeta;
          PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
          VBO.append(AssetVertex::Pin(true, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor, 1, 0));
      }
      for (const auto& Pin : ContainedExterior.blockExteriorData.OutputPin) {
          BlockMetadata PinMeta;
          PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
          VBO.append(AssetVertex::Pin(false, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor, 1, 0));
      }
  }

  Shader &assetShader = ShaderManager::GetShader(ShaderManager::Assets);

  assetShader.bind();
  AssetVAO.bind();
  VBO.replaceBuffer(AssetVAO, 0);
  AssetVAO.DrawAs(GL_POINTS);
  AssetVAO.unbind();
  assetShader.unbind();

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
    VisualBlockInterior &b = Frame->BlockManager->Interior;
    b.BBUpdate();
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
