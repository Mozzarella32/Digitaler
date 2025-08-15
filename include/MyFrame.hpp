#pragma once

#include "pch.hpp"

#include "Include.hpp"

class MyApp;
class IOHandler;
class Renderer;
class VisualBlockInterior;
class DataResourceManager;
class BlockSelector;

class MyFrame : public FrameWithGlContext {
  GameLoopTimer LoopTimer;

  // #ifdef HotShaderReload
  MyApp *App;
  // #endif

public:
  bool Initilized = false;
  std::unique_ptr<IOHandler> IO;
  std::unique_ptr<Renderer> renderer;
  std::unique_ptr<DataResourceManager> BlockManager;

  std::unique_ptr<VertexArrayObject> HoleScreenVAO;

  // GUI
  std::unique_ptr<BlockSelector> Blockselector;

  wxGLCanvasWithFrameContext *Canvas;
  // std::unique_ptr<Board> board;

  std::string InitilizeDescriptor = "none";

private:
  // void PositionButtons(wxGLCanvas* Canvas, std::vector<wxButton*> Buttons);

  enum class Menus {
    Quit = wxID_HIGHEST,
    Open,
    Save,
    Import,
    Export,
    New,
    SingleStep,
    ToggelSimulataion,
    Copy,
    Cut,
    Paste,
    GoHome
  };

  void OnQuitMenu([[maybe_unused]] wxCommandEvent &evt) {};

  void OnNewMenu([[maybe_unused]] wxCommandEvent &evt) {};

  void OnSaveMenu([[maybe_unused]] wxCommandEvent &evt) {};
  void OnOpenMenu([[maybe_unused]] wxCommandEvent &evt) {};

  void OnImportMenu([[maybe_unused]] wxCommandEvent &evt) {};
  void OnExportMenu([[maybe_unused]] wxCommandEvent &evt) {};

  void OnHelpMenu([[maybe_unused]] wxCommandEvent &evt) {};

  void OnCopyMenu([[maybe_unused]] wxCommandEvent &evt) {};
  void OnCutMenu([[maybe_unused]] wxCommandEvent &evt) {};
  void OnPasteMenu([[maybe_unused]] wxCommandEvent &evt) {};

  // void PostMoveUpdates(bool render);

  // void OnEscape();
  // void OnEnter();

  wxSlider *SpeedSlider;
  wxButton *StartStopButton;

  void OnSingleStepButton([[maybe_unused]] wxCommandEvent &evt) {};
  void OnStartStopButton([[maybe_unused]] wxCommandEvent &evt) {};
  void OnHomeButton([[maybe_unused]] wxCommandEvent &evt) {};
  void OnSpeedSlider() {};

public:
  MyFrame(MyApp *App);

  void OnGLInit() override;

  ~MyFrame();

private:
  void Loop();

  void OnMouseMove([[maybe_unused]] wxMouseEvent &evt) {};
  void OnMouseDown([[maybe_unused]] wxMouseEvent &evt) {};
  void OnMouseUp([[maybe_unused]] wxMouseEvent &evt) {};
  void OnLeaveWindow([[maybe_unused]] wxMouseEvent &evt) {};

  void OnKeyDown([[maybe_unused]] wxKeyEvent &evt) {};
  void OnKeyUp([[maybe_unused]] wxKeyEvent &evt) {};

  void OnChar([[maybe_unused]] wxKeyEvent &evt);
};
