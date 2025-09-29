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

  GLCALL(glClear(GL_COLOR_BUFFER_BIT));
  GLCALL(glViewport(0, 0, CanvasSize.x(), CanvasSize.y()));

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

  PreviewBlurDirty = true;
  HighlightedBlurDirty = true;
  MarkedBlurDirty = true;

  RenderBackground();
}

Eigen::Vector2i Renderer::CoordToNearestPoint(Eigen::Vector2f Pos) {
  // PROFILE_FUNKTION;

  return Eigen::Vector2i(static_cast<int>(std::round(Pos.x())),
                         static_cast<int>(std::round(Pos.y())));
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

  Shader& assetShader = ShaderManager::GetShader(ShaderManager::Assets);

  assetShader.bind();

  assetShader.apply("UIDRun", Shader::Data1i{true});

  GLuint clearint = 0;
  GLCALL(glClearTexImage(FBOIDTexture.GetId(), 0, GL_RED_INTEGER,
                         GL_UNSIGNED_INT, &clearint));

  VisualBlockInterior &b = Frame->BlockManager->Interior;

  auto SimplePass = [](auto VertsGetter, VertexArrayObject &VAO) {
    if (!VertsGetter().empty()) {

      VAO.bind();
      VAO.DrawAs(GL_POINTS);
      VAO.unbind();
    }
  };

  SimplePass([&b]() { return b.AssetVBO; }, AssetVAO);

  assetShader.unbind();

  FBOID.unbind();
}

void Renderer::RenderWires() {
  PROFILE_FUNKTION;

  VisualBlockInterior &b = Frame->BlockManager->Interior;
  
  b.UpdateVectsForVAOs(BoundingBox, MouseIndex, AllowHover);
  b.UpdateVectsForVAOsPreview(BoundingBox, MouseIndex);

  FBOMain.bind(FrameBufferObject::Draw);

  Shader& assetShader = ShaderManager::GetShader(ShaderManager::Assets);

  assetShader.bind();

  assetShader.apply("UIDRun", Shader::Data1i{false});

  const auto Pass = [&assetShader, this](
     VertexArrayObject& VAO,
     BufferedVertexVec<AssetVertex>& Path,
     bool BlurRun) {
    if(Path.empty()) return;

    VAO.bind();

    Path.replaceBuffer(VAO, 0);

    if(BlurRun) {
        GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
      assetShader.apply("UIDRun", Shader::Data1i{true});
      assetShader.apply("UBlurRun", Shader::Data1i{true});
    }

    if (!BlurRun)
        FBOBackgroundTexture.bind(assetShader,"UBackground","", 0);

    VAO.DrawAs(GL_POINTS);

    if (BlurRun) {
        assetShader.apply("UIDRun", Shader::Data1i{false});
        assetShader.apply("UBlurRun", Shader::Data1i{false});
    }
    if (!BlurRun)
        FBOBackgroundTexture.unbind();

    VAO.unbind();
  };

  Pass(PathVAO, b.normal.GetPath(), false);
  
  if (b.HasPreview()) {
    Pass(PathPreviewVAO, b.preview.GetPath(), false);
  }

  GLuint clearint = 0;
  //Preview
  if (PreviewBlurDirty) {
    GLCALL(glClearTexImage(FBOBlurPreviewTexture.GetId(), 0, GL_RED_INTEGER,
        GL_UNSIGNED_INT, &clearint));

    FBOBlurPreview.bind(FrameBufferObject::BindMode::Draw);

    if (!b.preview.GetPath().empty()) {
      PreviewNeedsReblur = true;
      Pass(PathPreviewVAO, b.preview.GetPath(), true);
    }

    FBOBlurPreview.unbind();
  }

  //Highlight
  if (HighlightedBlurDirty) {
    GLCALL(glClearTexImage(FBOBlurHighlightTexture.GetId(), 0, GL_RED_INTEGER,
        GL_UNSIGNED_INT, &clearint));

    FBOBlurHighlight.bind(FrameBufferObject::BindMode::Draw);

    if (!b.highlighted.GetPath().empty() && !b.HasPreview()) {
      HighlightedNeedsReblur = true;
      Pass(PathHighlightedVAO, b.highlighted.GetPath(), true);
    }

    FBOBlurHighlight.unbind();
  }

  //Marked
  if (MarkedBlurDirty) {
    GLCALL(glClearTexImage(FBOBlurMarkedTexture.GetId(), 0, GL_RED_INTEGER,
        GL_UNSIGNED_INT, &clearint));

    FBOBlurMarked.bind(FrameBufferObject::BindMode::Draw);

    if (!b.marked.GetPath().empty() && !b.HasPreview()) {
      MarkedNeedsReblur = true;
      Pass(PathMarkedVAO, b.marked.GetPath(), true);
    }

    FBOBlurHighlight.unbind();
  }

  PROFILE_SCOPE_ID_START("Index from Path", 3);

  FBOPathID.bind();

  // Recreate Index Buffer
  auto IndexRecreationPass = [](VertexArrayObject& VAO,
     BufferedVertexVec<AssetVertex>& Path) {
    if (Path.empty())
      return;

    VAO.bind();
    Path.replaceBuffer(VAO, 0);
    VAO.DrawAs(GL_POINTS);
    VAO.unbind();
  };

  assetShader.apply("UIndexRun", Shader::Data1i{true});
  GLCALL(glClear(GL_COLOR_BUFFER_BIT));
  IndexRecreationPass(PathVAO, b.normal.GetPath());
  IndexRecreationPass(PathPreviewVAO, b.preview.GetPath());
  assetShader.apply("UIndexRun", Shader::Data1i{false});

  FBOPathID.unbind();

  assetShader.unbind();

  PROFILE_SCOPE_ID_END(3);

  FBOMain.bind(FrameBufferObject::Draw);
}

void Renderer::BlurI(FrameBufferObject& FBO, const Texture& iTexture, const int iterations) {
  auto& gaussianShader = ShaderManager::GetShader(ShaderManager::Gaussian);

  gaussianShader.bind();
  Frame->HoleScreenVAO->bind();

  for(int i = 0; i < iterations; i++) {
    FBOBlurSwap.bind();
    iTexture.bind(gaussianShader, "UImage", "", 0);
    gaussianShader.apply("horizontal", Shader::Data1i{true});
    Frame->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
    iTexture.unbind();
    FBOBlurSwap.unbind();

    FBO.bind();
    FBOBlurSwapTexture.bind(gaussianShader, "UImage", "", 0);
    gaussianShader.apply("horizontal", Shader::Data1i{false});
    Frame->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
    FBOBlurSwapTexture.unbind();
    FBO.unbind();
  }

  Frame->HoleScreenVAO->unbind();
  gaussianShader.unbind(); 
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

  ShaderManager::applyGlobal("UTime", Shader::Data1f{std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0f});

  // PreviewBlurDirty = true;
  // HighlightedBlurDirty = true;
  // MarkedBlurDirty = true;

  Dirty = false;
  IdMapDirty = true;
  PreviewNeedsReblur = false;
  HighlightedNeedsReblur = false;
  MarkedNeedsReblur = false;

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

  GLCALL(glClear(GL_COLOR_BUFFER_BIT));

  GLCALL(glViewport(0, 0, CanvasSize.x(), CanvasSize.y()));

  FBOBackground.bind(FrameBufferObject::Read);

  GLCALL(glBlitFramebuffer(0, 0, CanvasSize.x(), CanvasSize.y(), 0, 0,
                           CanvasSize.x(), CanvasSize.y(), GL_COLOR_BUFFER_BIT,
                           GL_NEAREST));

  FBOBackground.unbind();

  PROFILE_SCOPE_ID_END(0);
 
  RenderWires();

  auto& assetShader = ShaderManager::GetShader(ShaderManager::Assets);

  assetShader.bind();

  assetShader.apply("UIDRun", Shader::Data1i{ false });

  PROFILE_SCOPE_ID_START("Draw And Or XOR", 4);

  GLCALL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

  auto SimplePass = [](auto VertsGetter, VertexArrayObject &VAO) {
    if (!VertsGetter().empty()) {
      VAO.bind();
      VertsGetter().replaceBuffer(VAO, 0);
      VAO.DrawAs(GL_POINTS);
      VAO.unbind();
    }
  };

  auto BlurPass = [](auto VertsGetter, VertexArrayObject &VAO, FrameBufferObject& FBO) {
    if (!VertsGetter().empty()) {

      FBO.bind(FrameBufferObject::BindMode::Draw);
      
      VAO.bind();
      VertsGetter().replaceBuffer(VAO, 0);
      VAO.DrawAs(GL_POINTS);
      VAO.unbind();

      FBO.unbind();
    }
  };

  TextAtlas.bind(assetShader, "UText", "", 1);
  FBOPathIDTexture.bind(assetShader, "UPathPresent", "", 0);
  SimplePass([&b]() { return b.AssetVBO; }, AssetVAO);
  FBOPathIDTexture.unbind();
  TextAtlas.unbind();

  assetShader.apply("UIDRun", Shader::Data1i{true});
  assetShader.apply("UBlurRun", Shader::Data1i{true});

  if (!b.HighlightAssetVBO.empty() && !b.HasPreview() && HighlightedBlurDirty) {
      HighlightedNeedsReblur = true;
      BlurPass([&b]() {return b.HighlightAssetVBO;}, HighlightAssetVAO, FBOBlurHighlight);
  }

  if (!b.MarkedAssetVBO.empty() && !b.HasPreview() && MarkedBlurDirty) {
      MarkedNeedsReblur = true;
      BlurPass([&b]() {return b.MarkedAssetVBO;}, MarkedAssetVAO, FBOBlurMarked);
  }

  assetShader.apply("UIDRun", Shader::Data1i{false});
  assetShader.apply("UBlurRun", Shader::Data1i{false});
  assetShader.unbind();

  PROFILE_SCOPE_ID_END(4);

  PROFILE_SCOPE_ID_START("Blur", 10);

  if (PreviewNeedsReblur)
    BlurI(FBOBlurPreview, FBOBlurPreviewTexture, 1);
  if (HighlightedNeedsReblur)
    BlurI(FBOBlurHighlight, FBOBlurHighlightTexture, 1);
  if (MarkedNeedsReblur)
    BlurI(FBOBlurMarked, FBOBlurMarkedTexture, 1);

  PreviewBlurDirty = false;
  HighlightedBlurDirty = false;
  MarkedBlurDirty = false;

  FBOMain.bind(FrameBufferObject::Draw);

  PROFILE_SCOPE_ID_END(10);

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
      BB.Position.y() += BB.Size.y() - 1;

      PointType Pos1 = BB.Position;
      Pos1.y() -= BB.Size.y() - 1;
      PointType Pos2 = BB.Position;
      Pos2.x() += BB.Size.x() - 1;

      if (size == 0) {
        BlockMetadata Meta{};
        Blocks.append(AssetVertex::Box(Meta.Transform(), Pos1, Pos2, ColourType(0.5, 0.5, 0.5, 0.5), 0));
      } else {
        BlockMetadata Meta{};
        auto [cr, cg, cb] = GetColour(size);
        Blocks.append(AssetVertex::Box(Meta.Transform(), Pos1, Pos2, ColourType(cr, cg, cb, 0.5), 0));
      }
    }
  }

  assetShader.bind();
  SimplePass([&Blocks]() { return Blocks; }, CollisionGridVAO);
  assetShader.unbind();
#endif

  if (Frame->Blockselector && !Frame->Blockselector->GetTextVBO().empty()) {

    PROFILE_SCOPE("Text");

    assetShader.bind();

    TextAtlas.bind(assetShader, "UTexture", "", 0);

    // Overlay Text
    assetShader.apply("UZoom", Shader::Data2f{
                       0.01f * CanvasSize.x() / 2.0f,
                       0.01f * CanvasSize.y() / 2.0f});
    assetShader.apply("UOffset", Shader::Data2f{0.0,0.0});
    assetShader.apply("UZoomFactor", Shader::Data1f{0.001f});

    if (Frame->Blockselector) {
      Frame->Blockselector->Update();
      auto &Blockselector = *Frame->Blockselector;
      if (!Blockselector.GetTextVBO().empty()) {
        StaticTextVAO.bind();
        Blockselector.GetTextVBO().replaceBuffer(StaticTextVAO, 0);
        StaticTextVAO.DrawAs(GL_POINTS);
        StaticTextVAO.unbind();
      }
    }

    assetShader.apply("UZoom", Shader::Data2f{
                            CanvasSize.x() * float(Zoom),
                            CanvasSize.y() * float(Zoom)});
    assetShader.apply("UOffset", Shader::Data2f{Offset.x(),
                       Offset.y()});
    assetShader.apply("UZoomFactor", Shader::Data1f{float(Zoom)});

    assetShader.unbind();
  }

  PROFILE_SCOPE("Swap Buffers");

  title = "Zoom: " + std::to_string(Zoom) +
          " Off: " + std::to_string(Offset.x()) + ", " +
          std::to_string(Offset.y());

  bool mainFBOSwap = true;

  auto mainFBOActive = [&mainFBOSwap, this]() -> FrameBufferObject& {
      return (mainFBOSwap) ? FBOMainSwap : FBOMain;
   };

  auto mainFBOTexture = [&mainFBOSwap, this]() -> Texture& {
      return (mainFBOSwap) ? FBOMainColorTexture : FBOMainColorSwapTexture;
   };

  auto& liquidGlassShader = ShaderManager::GetShader(ShaderManager::LiquidGlass);
  if(b.HasPreview() || b.HasHighlited() || b.HasHighlitedPath() || b.HasAnythingMarked()) {
    liquidGlassShader.bind();
    Frame->HoleScreenVAO->bind();
    FBOMain.unbind();
    FBOMainSwap.bind();
    GLCALL(glClear(GL_COLOR_BUFFER_BIT));
    FBOMainSwap.unbind();
  }

  if(b.HasAnythingMarked()) {
    mainFBOActive().bind();
    mainFBOTexture().bind(liquidGlassShader, "UTex", "", 1);
    FBOBlurMarkedTexture.bind(liquidGlassShader, "UBluredBase", "", 0);
    liquidGlassShader.apply("UColor", Shader::Data3f{1.0, 0.0, 1.0});
    Frame->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
    FBOBlurMarkedTexture.unbind();
    mainFBOTexture().unbind();
    mainFBOActive().unbind();
    mainFBOSwap = !mainFBOSwap;
  }

  if(b.HasHighlited() || b.HasHighlitedPath()) {
    mainFBOActive().bind();
    mainFBOTexture().bind(liquidGlassShader, "UTex", "", 1);
    FBOBlurHighlightTexture.bind(liquidGlassShader, "UBluredBase", "", 0);
    liquidGlassShader.apply("UColor", Shader::Data3f{1.0, 1.0, 0.0});
    Frame->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
    FBOBlurHighlightTexture.unbind();
    mainFBOTexture().unbind();
    mainFBOActive().unbind();
    mainFBOSwap = !mainFBOSwap;
  }

  if(b.HasPreview()) {
    mainFBOActive().bind();
    mainFBOTexture().bind(liquidGlassShader, "UTex", "", 1);
    FBOBlurPreviewTexture.bind(liquidGlassShader, "UBluredBase", "", 0);
    liquidGlassShader.apply("UColor", Shader::Data3f{0.0, 1.0, 0.0});
    Frame->HoleScreenVAO->DrawAs(GL_TRIANGLE_STRIP);
    FBOBlurPreviewTexture.unbind();
    mainFBOTexture().unbind();
    mainFBOActive().unbind();
    mainFBOSwap = !mainFBOSwap;
  }

  mainFBOSwap = !mainFBOSwap;
  if(b.HasPreview() || b.HasHighlited() || b.HasHighlitedPath() || b.HasAnythingMarked()) {
    Frame->HoleScreenVAO->unbind();
    liquidGlassShader.unbind();
    mainFBOActive().bind();
  }

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
  if (!b.BasePositionVBO.empty()) {

    PROFILE_SCOPE("BasePotitionOfBlocks");

    assetShader.bind();

    BlockBasePositionVAO.bind();
    b.BasePositionVBO.replaceBuffer(BlockBasePositionVAO, 0);
    BlockBasePositionVAO.DrawAs(GL_POINTS);
    BlockBasePositionVAO.unbind();

    assetShader.unbind();
  }
#endif

#ifdef ShowBoundingBoxes
  if(!b.BBVBO.empty()){
    assetShader.bind();

    BBVAO.bind();
    b.BBVBO.replaceBuffer(BBVAO, 0);
    BBVAO.DrawAs(GL_POINTS);
    BBVAO.unbind();
    
    assetShader.unbind();
  }
#endif
  
  mainFBOActive().unbind();
  
  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  mainFBOActive().bind(FrameBufferObject::Read);

  GLCALL(glBlitFramebuffer(0, 0, CanvasSize.x(), CanvasSize.y(), 0, 0,
                           CanvasSize.x(), CanvasSize.y(), GL_COLOR_BUFFER_BIT,
                           GL_NEAREST));

  mainFBOActive().unbind();

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
      FBOPathIDTexture(1, 1, nullptr,
                   []() {
                     Texture::Descriptor desc;
                     desc.Format = GL_RED_INTEGER;
                     desc.Internal_Format = GL_R32UI;
                     desc.Depth_Stencil_Texture_Mode = GL_STENCIL_INDEX;
                     desc.Type = GL_UNSIGNED_INT;
                     return desc;
                   }()),
      FBOPathID({&FBOPathIDTexture}, {GL_NONE, GL_COLOR_ATTACHMENT1}),
      FBOMainColorTexture(1, 1),
      FBOMain({&FBOMainColorTexture},
              {GL_COLOR_ATTACHMENT0}),
      FBOMainColorSwapTexture(1, 1),
      FBOMainSwap({&FBOMainColorSwapTexture}, {GL_COLOR_ATTACHMENT0}),
      FBOIDTexture(1, 1, nullptr,
                   []() {
                     Texture::Descriptor desc;
                     desc.Format = GL_RED_INTEGER;
                     desc.Internal_Format = GL_R32UI;
                     desc.Depth_Stencil_Texture_Mode = GL_STENCIL_INDEX;
                     desc.Type = GL_UNSIGNED_INT;
                     return desc;
                   }()),
      FBOID({&FBOIDTexture}, {GL_NONE, GL_COLOR_ATTACHMENT1}),
      FBOBlurHighlightTexture(CreateBlurTexture()),
      FBOBlurHighlight({&FBOBlurHighlightTexture}, {GL_NONE, GL_COLOR_ATTACHMENT1}),
      FBOBlurPreviewTexture(CreateBlurTexture()),
      FBOBlurPreview({&FBOBlurPreviewTexture}, {GL_NONE, GL_COLOR_ATTACHMENT1}),
      FBOBlurMarkedTexture(CreateBlurTexture()),
      FBOBlurMarked({&FBOBlurMarkedTexture}, {GL_NONE, GL_COLOR_ATTACHMENT1}),
      FBOBlurSwapTexture(CreateBlurTexture()),
      FBOBlurSwap({&FBOBlurSwapTexture}, {GL_NONE, GL_COLOR_ATTACHMENT1}),
      PathVAO(CreateVAO<AssetVertex>()),
      PathPreviewVAO(CreateVAO<AssetVertex>()),
      PathHighlightedVAO(CreateVAO<AssetVertex>()),
      PathMarkedVAO(CreateVAO<AssetVertex>()),
#ifdef RenderCollisionGrid
      CollisionGridVAO(CreateVAO<AssetVertex>()),
#endif
      AssetVAO(CreateVAO<AssetVertex>()),
      PinVAO(CreateVAO<AssetVertex>()),
      RoundPinVAO(CreateVAO<AssetVertex>()),
      HighlightAssetVAO(CreateVAO<AssetVertex>()),
      MarkedAssetVAO(CreateVAO<AssetVertex>()),
      AreaSelectVAO(CreateVAO<AssetVertex>()),
#ifdef ShowBoundingBoxes
      BBVAO(CreateVAO<AssetVertex>()),
#endif
#ifdef ShowBasePositionOfBlocks
      BlockBasePositionVAO(CreateVAO<AssetVertex>()),
#endif
      StaticTextVAO(CreateVAO<AssetVertex>()),
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

  FBOPathIDTexture.Resize(CanvasSize.x(), CanvasSize.y());

  FBOMainColorTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOMainColorSwapTexture.Resize(CanvasSize.x(), CanvasSize.y());

  FBOIDTexture.Resize(CanvasSize.x(), CanvasSize.y());

  FBOBlurHighlightTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOBlurPreviewTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOBlurMarkedTexture.Resize(CanvasSize.x(), CanvasSize.y());
  FBOBlurSwapTexture.Resize(CanvasSize.x(), CanvasSize.y());

  UpdateViewProjectionMatrix();
  Dirty = true;
  Render();
}

BufferedVertexVec<AssetVertex> &Renderer::GetAreaSelectVerts() {
  return AreaSelectVerts;
}

void Renderer::UpdateMouseIndex(const PointType &newMouseIndex) {

  if (!Frame->Canvas->BindContext()) {
    wxMessageBox("Context should be bindable by now!", "Error", wxICON_ERROR);
  }

  Shader &BackgroundShader =
      ShaderManager::GetShader(ShaderManager::Background);

  ShaderManager::applyGlobal("UMousePos", Shader::Data2f{float(newMouseIndex.x()),
    float(newMouseIndex.y())});

  BackgroundShader.bind();
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

  FBOID.bind();

  GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT1));

  auto Size = Frame->Canvas->GetSize();

  GLuint i;
  GLCALL(glReadPixels(Mouse.x(), Size.y - Mouse.y(), 1, 1, GL_RED_INTEGER,
                      GL_UNSIGNED_INT, &i));
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
  double TargetZoom = 0.01;

  Zoom = TargetZoom;

  const auto &ContainedExteriorOpt =
      Frame->BlockManager->GetCompressedBlockData(cbdi);
  if (!ContainedExteriorOpt) {
    assert(false && "you messed up");
  }
  const auto &ContainedExterior = ContainedExteriorOpt.value();
  auto BlockSize = ContainedExterior.blockExteriorData.Size;

  if (BlockSize == PointType{-1, -1}) {
    BlockSize = PointType{0, 0};
  }

  auto rectVertical =
      MyRectF::FromCorners(Eigen::Vector2f{-1.5, 1.5},
                           Eigen::Vector2f{BlockSize.x(), -BlockSize.y()} +
                               Eigen::Vector2f{1.5, -1.5});

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

  ColourType NoColor = {0.0, 0.0, 0.0, 1.0};

  BlockMetadata Meta;
  Meta.Pos = {};
  Meta.Rotation = MyDirection::Up;

  PointType Pos1 = {};
  Pos1.y() -= BlockSize.y();
  PointType Pos2 = {};
  Pos2.x() += BlockSize.x();

  BufferedVertexVec<AssetVertex> VBO;

	static auto GetBI = [this](const std::string_view name) {
		return Frame->BlockManager->GetBlockIndex(BlockIdentifiyer::ParsePredefined(name)); 
	};

  using namespace PredefinedNames;

	static const auto AND =          GetBI(And);
	static const auto OR =           GetBI(Or);
	static const auto XOR =          GetBI(XOr);
	static const auto MUX =          GetBI(Mux);
	static const auto DRIVER =       GetBI(Driver);
	// static const auto TOGGLEBUTTON = GetBI(ToggleButton);
	// static const auto BUTTON =       GetBI(Button);
	static const auto CONSTANT =     GetBI(Constant);
	static const auto PROBE =        GetBI(Probe);

  for (const auto& Pin : ContainedExterior.blockExteriorData.InputPin) {
      if (Pin.Round) continue;
      BlockMetadata PinMeta;
      PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
      VBO.append(AssetVertex::Pin(AssetVertex::ID::InputPin, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor, 1));
  }
  for (const auto& Pin : ContainedExterior.blockExteriorData.OutputPin) {
      if (Pin.Round) continue;
      BlockMetadata PinMeta;
      PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
      VBO.append(AssetVertex::Pin(AssetVertex::ID::OutputPin, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor, 1));
  }

  if (cbdi == AND) VBO.append(AssetVertex::Gate(AssetVertex::ID::And, Meta.Transform(), Meta.Pos, 1));
  else if (cbdi == OR) VBO.append(AssetVertex::Gate(AssetVertex::ID::Or, Meta.Transform(), Meta.Pos, 1));
  else if (cbdi == XOR) VBO.append(AssetVertex::Gate(AssetVertex::ID::Xor, Meta.Transform(), Meta.Pos, 1));
  else if (cbdi == MUX) VBO.append(AssetVertex::Mux(Meta.Transform(), 1, Meta.Pos, NoColor, 1));
  else if (cbdi == DRIVER) VBO.append(AssetVertex::Driver(Meta.Transform(), 1, Meta.Pos, NoColor, 1));
  else if (cbdi == CONSTANT);// dont do anything
  else if (cbdi == PROBE);// dont do anything
  else VBO.append(AssetVertex::Box(Meta.Transform(), Pos1, Pos2, NoColor, 1));

  for (const auto& Pin : ContainedExterior.blockExteriorData.InputPin) {
      if (!Pin.Round) continue;
      BlockMetadata PinMeta;
      PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
      VBO.append(AssetVertex::Pin(AssetVertex::ID::InputRoundPin, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor, 1));
  }
  for (const auto& Pin : ContainedExterior.blockExteriorData.OutputPin) {
      if (!Pin.Round) continue;
      BlockMetadata PinMeta;
      PinMeta.Rotation = VisualBlockInterior::GetPinRotation(Meta, Pin);
      VBO.append(AssetVertex::Pin(AssetVertex::ID::OutputRoundPin, PinMeta.Transform(), VisualBlockInterior::GetPinPosition(BlockSize, Meta, Pin, 1), NoColor, 1));
  }

  auto& assetShader = ShaderManager::GetShader(ShaderManager::Assets);

  assetShader.bind();
  assetShader.apply("UIDRun", Shader::Data1i{ true });
  AssetVAO.bind();
  VBO.replaceBuffer(AssetVAO, 0);
  AssetVAO.DrawAs(GL_POINTS);
  AssetVAO.unbind();
  assetShader.unbind();

  FBOID.unbind();

  FBOID.bind();

  // GLCALL(glDrawBuffers(DrawBuffer1.size(), DrawBuffer1.data()));

  GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT1));

  std::unique_ptr<std::vector<GLuint>> vec =
      std::make_unique<std::vector<GLuint>>();
  vec->resize(ViewportSize.x() * ViewportSize.y());
  GLCALL(glReadPixels(0, 0, ViewportSize.x(), ViewportSize.y(), GL_RED_INTEGER,
                      GL_UNSIGNED_INT, vec->data()));

  // GLCALL(glDrawBuffers(DrawBuffer0.size(), DrawBuffer0.data()));
  FBOID.unbind();

  MyRectF rect = MyRectF::FromCorners({0, 0}, {1, 1});
  BlockBoundingBoxes[cbdi] = {rect, rect, rect, rect};

  std::unique_lock ul_Funcs(BBUpdateFuncsMutex);

  BBUpdateFuncs.push([vec = vec.release(), ViewportSize, TargetZoom, BlockSize,
                      this, cbdi]() {
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

    if (cbdi == MUX || cbdi == DRIVER) {
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

  double TargetZoom = DefaultZoom;
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
