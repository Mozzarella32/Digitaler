#include "MyFrame.hpp"

#include "MyApp.hpp"
#include "PngManager.hpp"

#include "IOHandler.hpp"

#include "Renderer.hpp"

#include "Block.hpp"

#include "ShaderManager.hpp"

#include "DataResourceManager.hpp"

#include "BlockSelector.hpp"

MyFrame::MyFrame(MyApp *App)
    : wxFrame(nullptr, wxID_ANY, "MyFrame", wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE),
      // #ifdef HotShaderReload
      App(App),
      // #endif
      LoopTimer(this, [this]() { Loop(); }) {
  PROFILE_FUNKTION;
  Bind(wxEVT_CLOSE_WINDOW, [App, this](wxCloseEvent &evt) {
    evt.Skip();
    LoopTimer.Stop();
    // Destroy();
  });

  wxIcon Icon;
  Icon.CopyFromBitmap(PngManager::GetPng(PngManager::Icon));
  SetIcon(Icon);

  auto AddMenu = [Frame = this](wxMenu *Parent, Menus Id,
                                const wxString &TitleAndShortcut,
                                void (MyFrame::*fn)(wxCommandEvent &),
                                wxBitmap Bitmap = wxNullBitmap,
                                wxItemKind kind = wxITEM_NORMAL) {
    auto *item = new wxMenuItem(Parent, (int)Id, TitleAndShortcut, "", kind);
    item->SetBitmap(Bitmap);
    Parent->Append(item);
    Frame->CallAfter(
        [Frame, fn, Id]() { Frame->Bind(wxEVT_MENU, fn, Frame, (int)Id); });
    return item;
  };

  wxMenuBar *MenuBar = new wxMenuBar();

  wxMenu *FileMenu = new wxMenu();

  AddMenu(FileMenu, Menus::New, "&New\tCtrl+N", &MyFrame::OnNewMenu);
  FileMenu->AppendSeparator();
  AddMenu(FileMenu, Menus::Open, "&Open\tCtrl+O", &MyFrame::OnOpenMenu);
  AddMenu(FileMenu, Menus::Save, "&Save\tCtrl+S", &MyFrame::OnSaveMenu);
  FileMenu->AppendSeparator();
  AddMenu(FileMenu, Menus::Import, "&Import\tCtrl+I", &MyFrame::OnImportMenu);
  AddMenu(FileMenu, Menus::Export, "&Export\tCtrl+E", &MyFrame::OnExportMenu);
  FileMenu->AppendSeparator();
  AddMenu(FileMenu, Menus::Quit, "&Quit\tAlt+F4", &MyFrame::OnQuitMenu);

  MenuBar->Append(FileMenu, "&File");

  wxMenu *EditMenu = new wxMenu();
  AddMenu(EditMenu, Menus::Copy, "&Copy\tCtrl+C", &MyFrame::OnCopyMenu,
          PngManager::GetPng(PngManager::Copy).Scale(12, 12));
  AddMenu(EditMenu, Menus::Cut, "&Cut\tCtrl+X", &MyFrame::OnCutMenu,
          PngManager::GetPng(PngManager::Cut).Scale(12, 12));
  AddMenu(EditMenu, Menus::Paste, "&Paste\tCtrl+V", &MyFrame::OnPasteMenu,
          PngManager::GetPng(PngManager::Paste).Scale(12, 12));
  EditMenu->AppendSeparator();

  MenuBar->Append(EditMenu, "&Edit");

  wxMenu *SimulateMenu = new wxMenu();
  /*ToggelSimulationMenu =*/AddMenu(SimulateMenu, Menus::ToggelSimulataion,
                                    "&Start\tSPACE",
                                    &MyFrame::OnStartStopButton);
  AddMenu(SimulateMenu, Menus::SingleStep, "&Step\ts",
          &MyFrame::OnSingleStepButton,
          PngManager::GetPng(PngManager::SingleStep).Scale(12, 12));

  MenuBar->Append(SimulateMenu, "&Simulate");

  wxMenu *HelpMenu = new wxMenu();

  HelpMenu->Append(new wxMenuItem(HelpMenu, wxID_HELP, "&About"));
  CallAfter([this]() {
    Bind(
        wxEVT_MENU, [this](wxCommandEvent &evt) { OnHelpMenu(evt); },
        wxID_HELP);
  });

  MenuBar->Append(HelpMenu, "&Help");

  SetMenuBar(MenuBar);

  SetBackgroundColour(wxColour(50, 50, 50));
  SetForegroundColour("White");
  auto *Sizer = new wxBoxSizer(wxVERTICAL);

  auto MenuBarPanel = new wxPanel(this);
  MenuBarPanel->SetBackgroundColour(wxColour(70, 70, 70));
  MenuBarPanel->SetDoubleBuffered(true);
  auto *MenuBarPanelSizer = new wxBoxSizer(wxHORIZONTAL);

  auto AddButton = [Frame = this](wxWindow *Parent, wxSizer *Sizer,
                                  const wxImage &Bitmap,
                                  void (MyFrame::*Func)(wxCommandEvent &)) {
    auto *Button =
        new wxButton(Parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
                     wxDefaultSize, wxBU_EXACTFIT | wxBORDER_NONE);
    Button->SetBitmap(Bitmap.Scale(24, 24));
    Frame->CallAfter(
        [Frame, Func, Button]() { Button->Bind(wxEVT_BUTTON, Func, Frame); });
    Button->SetBackgroundColour(wxColour(70, 70, 70));
    Button->SetForegroundColour("White");
    Button->DisableFocusFromKeyboard();
    Sizer->Add(Button, 0, wxALL, 5);
    Button->Disable();
    Frame->CallAfter([Button]() { Button->Enable(); });
    return Button;
  };

  auto AddTrenner = [](wxWindow *Parent, wxSizer *Sizer) {
    auto *Trenner = new wxPanel(Parent, wxID_ANY, wxDefaultPosition, {2, -1});
    Trenner->SetBackgroundColour(wxColour(120, 120, 120));
    Sizer->Add(Trenner, 0, wxEXPAND | wxALL, 5);
  };

  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::Home), &MyFrame::OnHomeButton);
  // AddTrenner(MenuBarPanel, MenuBarPanelSizer);
  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::SelectAll), &MyFrame::OnSelectAll);
  // AddTrenner(MenuBarPanel, MenuBarPanelSizer);
  // CopyButton = AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::Copy), &MyFrame::OnCopyMenu); CutButton =
  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::Cut), &MyFrame::OnCutMenu); PasteButton =
  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::Paste), &MyFrame::OnPasteMenu);
  // AddTrenner(MenuBarPanel, MenuBarPanelSizer);
  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::ClearWhite), &MyFrame::OnClearButtonWhite);
  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::ClearBlack), &MyFrame::OnClearButtonBlack);
  // AddTrenner(MenuBarPanel, MenuBarPanelSizer);
  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::MirrowV), &MyFrame::OnMirrowVMenu);
  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::MirrowH), &MyFrame::OnMirrowHMenu);
  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::DiagonalPrimary),
  // &MyFrame::OnMirrowPrimaryDiagonalMenu); AddButton(MenuBarPanel,
  // MenuBarPanelSizer, PngManager::GetPng(PngManager::DiagonalSecondary),
  // &MyFrame::OnMirrowSecondaryDiagonalMenu); AddTrenner(MenuBarPanel,
  // MenuBarPanelSizer); AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::RotateNeunzigMinus),
  // &MyFrame::OnRotateNeunzigMinusMenu); AddButton(MenuBarPanel,
  // MenuBarPanelSizer, PngManager::GetPng(PngManager::RotateNeunzigPlus),
  // &MyFrame::OnRotateNeunzigPlusMenu); AddButton(MenuBarPanel,
  // MenuBarPanelSizer, PngManager::GetPng(PngManager::RotateHalfway),
  // &MyFrame::OnRotateHalfwayMenu); AddTrenner(MenuBarPanel,
  // MenuBarPanelSizer); AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::ChangeRules),
  // &MyFrame::OnChangeRulesButton); AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::ChangeSize), &MyFrame::OnChangeSizeButton);
  // AddTrenner(MenuBarPanel, MenuBarPanelSizer);
  // StartStopButton = AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::Start), &MyFrame::OnStartStopButton);
  // AddButton(MenuBarPanel, MenuBarPanelSizer,
  // PngManager::GetPng(PngManager::SingleStep), &MyFrame::OnSingleStepButton);
  // AddTrenner(MenuBarPanel, MenuBarPanelSizer);

  // Slider
  // auto* SliderText = new wxStaticText(MenuBarPanel, wxID_ANY, "Speed");
  // SliderText->SetBackgroundColour(wxColour(70, 70, 70));
  // SliderText->SetForegroundColour("White");
  // MenuBarPanelSizer->Add(SliderText, 0, wxUP | wxDOWN |
  // wxALIGN_CENTER_VERTICAL, 5); SpeedSlider = new wxSlider(MenuBarPanel,
  // wxID_ANY, 100, 0, 100); SpeedSlider->SetValue(50);
  // SpeedSlider->SetBackgroundColour(wxColour(70, 70, 70));
  // SpeedSlider->SetForegroundColour("White");
  // SpeedSlider->Disable();

  // CallAfter([=]() {
  // SpeedSlider->Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt)
  // {OnSpeedSlider(); evt.Skip(); });
  // });

  // MenuBarPanelSizer->Add(SpeedSlider, 1, wxALIGN_CENTER_VERTICAL);

  /*wxTreeCtrl* Tree = new wxTreeCtrl(MenuBarPanel, wxID_ANY, wxDefaultPosition,
  wxDefaultSize,  wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_SINGLE |
  wxTR_FULL_ROW_HIGHLIGHT | wxTR_NO_LINES);
  Tree->SetBackgroundColour(wxColour(70, 70, 70));
  Tree->SetForegroundColour("White");
  Tree->SetDoubleBuffered(true);
  Tree->AddRoot("Root");

  auto Child0 = Tree->AppendItem(Tree->GetRootItem(), "Child0");
  auto Child1 = Tree->AppendItem(Tree->GetRootItem(), "Child1");

  auto Child00 = Tree->AppendItem(Child0, "Child00");
  auto Child01 = Tree->AppendItem(Child0, "Child01");

  auto Child10 = Tree->AppendItem(Child1, "Child10");
  auto Child11 = Tree->AppendItem(Child1, "Child11");

  MenuBarPanelSizer->Add(Tree, 1, wxEXPAND);*/

  // auto Tree = new wxTreeListCtrl(MenuBarPanel, wxID_ANY, wxDefaultPosition,
  // wxDefaultSize, wxTL_DEFAULT_STYLE | wxTL_CHECKBOX);

  // auto Col_Desc1 = Tree->AppendColumn("Desc1", wxCOL_RESIZABLE |
  // wxCOL_WIDTH_AUTOSIZE); auto Col_Desc2 = Tree->AppendColumn("Desc2",
  // wxCOL_RESIZABLE | wxCOL_WIDTH_AUTOSIZE);

  // auto AppendChild = [Tree, Col_Desc1, Col_Desc2](wxTreeListItem Parent,
  // const wxString& Title, const wxString& Desc1, const wxString& Desc2) {
  // auto
  // Item = Tree->AppendItem(Parent, Title); 	Tree->SetItemText(Item,
  // Col_Desc1, Desc1); 	Tree->SetItemText(Item, Col_Desc2, Desc2);
  // return Item;
  //	};

  // auto Child0 = AppendChild(Tree->GetRootItem(), "Child0", "HI", "Zwei");
  // auto Child1 = AppendChild(Tree->GetRootItem(), "Child1", "Ne", "Drei");

  // auto Child00 = AppendChild(Child0, "Child00", "Ich", "F�nf");

  // auto Child000 = AppendChild(Child00, "Child000", "Ich", "F�nf");
  // auto Child001 = AppendChild(Child00, "Child001", "Ich", "F�nf");

  // auto Child01 = AppendChild(Child0, "Child01", "Wei�", "Sieben");

  // auto Child010 = AppendChild(Child01, "Child010", "Ich", "F�nf");
  // auto Child011 = AppendChild(Child01, "Child011", "Ich", "F�nf");

  // auto Child10 = AppendChild(Child1, "Child10", "Nicht", "Dreizen");

  // auto Child100 = AppendChild(Child10, "Child100", "Ich", "F�nf");
  // auto Child101 = AppendChild(Child10, "Child101", "Ich", "F�nf");

  // auto Child11 = AppendChild(Child1, "Child11", "weiter", "Neun");

  // auto Child110 = AppendChild(Child11, "Child110", "Ich", "F�nf");
  // auto Child111 = AppendChild(Child11, "Child111", "Ich", "F�nf");

  ////Tree->Bind(wxEVT_TREELIST_ITEM_EXPANDING,
  ///[Tree,AppendChild](wxTreeListEvent& evt) { /	evt.Skip(); /	auto
  /// item = evt.GetItem(); /	auto hasChild =
  /// Tree->GetFirstChild(item).IsOk(); /	if (!hasChild) { /
  /// AppendChild(item, "Child", "HI", "What"); /	} /
  /////Tree->Expand(evt.GetItem()); /	});

  // Tree->Bind(wxEVT_TREELIST_ITEM_CHECKED, [this, AppendChild,
  // Tree](wxTreeListEvent& evt) { 	evt.Skip(); 	wxCheckBoxState Check =
  // Tree->GetCheckedState(evt.GetItem());

  //	/*if (Check == wxCHK_CHECKED) {
  //		Check = wxCHK_UNCHECKED;
  //	}
  //	else if (Check == wxCHK_UNCHECKED) {
  //		Check = wxCHK_CHECKED;
  //	}*/

  //	//auto child = AppendChild(evt.GetItem(), "Child", "HI", "What");
  //	//auto child = Tree->GetFirstChild(evt.GetItem());

  //	Tree->CheckItemRecursively(evt.GetItem(), Check);

  //
  //	//Tree->Expand(evt.GetItem());
  //	Tree->Refresh();
  //	Layout();
  //	Refresh();
  //	});
  ///*CallAfter([=]() {
  //	Tree->SendSizeEvent();
  //	});*/

  // MenuBarPanelSizer->Add(Tree, 1, wxEXPAND);

  MenuBarPanel->SetSizerAndFit(MenuBarPanelSizer);
  Sizer->Add(MenuBarPanel, 0, wxEXPAND | wxALL, 0);

  Canvas = new wxGLCanvas(this);

  Canvas->Bind(wxEVT_SIZE, [this](wxSizeEvent &evt) {
    evt.Skip();
    if (!IO)
      return;
    IO->OnCanvasSizeChange();
  });
  Canvas->Bind(wxEVT_MOUSEWHEEL, [this](wxMouseEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    const double scale = GetContentScaleFactor();
    IO->OnMouseWheel({scale * evt.GetPosition().x, scale * evt.GetPosition().y},
                     evt.GetWheelRotation());
  });
  /*Canvas->Bind(wxEVT_GESTURE_ZOOM, [this](wxZoomGestureEvent& evt) {
          evt.Skip();
          IO->OnMouseWheel(evt.GetPosition(), evt.GetZoomFactor());
          });*/
  Canvas->Bind(wxEVT_MOTION, [this](wxMouseEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    const double scale = GetContentScaleFactor();
    IO->OnMouseMove({scale * evt.GetPosition().x, scale * evt.GetPosition().y});
  });
  Canvas->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    const double scale = GetContentScaleFactor();
    IO->OnMouseDown({scale * evt.GetPosition().x, scale * evt.GetPosition().y});
  });
  Canvas->Bind(wxEVT_LEFT_DCLICK, [this](wxMouseEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    const double scale = GetContentScaleFactor();
    IO->OnDClick({scale * evt.GetPosition().x, scale * evt.GetPosition().y});
  });
  Canvas->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    const double scale = GetContentScaleFactor();
    IO->OnMouseUp({scale * evt.GetPosition().x, scale * evt.GetPosition().y});
  });
  Canvas->Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    const double scale = GetContentScaleFactor();
    IO->OnRightMouseDown(
        {scale * evt.GetPosition().x, scale * evt.GetPosition().y});
  });
  Canvas->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    IO->StoppDragg();
  });
  App->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    IO->OnKeyDown(evt);
  });
  App->Bind(wxEVT_KEY_UP, [this](wxKeyEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    IO->OnKeyUp(evt);
  });
  App->Bind(wxEVT_CHAR_HOOK, [this](wxKeyEvent &evt) {
    evt.Skip();
    if (!Initilized)
      return;
    IO->OnChar(evt);
  });

  Sizer->Add(Canvas, 1, wxEXPAND);

  SetSizerAndFit(Sizer);
  Layout();

  // CenterOnScreen(wxBOTH);

  LoopTimer.SetSamples(60 * 10);
  LoopTimer.SetFPS(60);

  LoopTimer.Start();

  // SetSize({ 1000,1000 });

  Show(true);

  Maximize();

  PROFILE_SESSION_START("Hi",
                        std::filesystem::path(wxGetHomeDir().ToStdString()) /
                            "Profiled.json");

  InitilizeDescriptor = "Waiting for Opengl initilisation";

  SetTitle(std::string("Initilizing: ").append(InitilizeDescriptor));

  CallAfter([this]() {
    PROFILE_SCOPE("Initilizing");

    this->App->ContextBinder->BindContext(this->App->GlContext.get());

    /*	InitilizeDescriptor = "Loaing Shaders";
            SetTitle(std::string("Initilizing: ").append(InitilizeDescriptor));

            ShaderManager::Initilise();*/

    IO = std::make_unique<IOHandler>(this);
    {
      PROFILE_SCOPE("Creating Renderer");
      renderer = std::make_unique<Renderer>(this->App, this);
    }

    IO->OnCanvasSizeChange();

    InitilizeDescriptor = "Loaing Block Data";
    SetTitle(std::string("Initilizing: ").append(InitilizeDescriptor));

    // CurrentBlock = std::make_unique<VisualBlockInterior>(0);
    BlockManager = std::make_unique<DataResourceManager>(renderer.get());

    Blockselector =
        std::make_unique<BlockSelector>(BlockManager.get(), renderer.get());

    BlockManager->Blockselector = Blockselector.get();

    BlockManager->SetCurrent(
        BlockManager->GetBlockIndex(
            BlockIdentifiyer::ParsePredefined("Testing:MainBlock")),
        0.01f, {0, 0}, true);

    InitilizeDescriptor =
        "Resizing Textures, Creating BoundingBoxes and Initial Render";
    SetTitle(std::string("Initilizing: ").append(InitilizeDescriptor));

    Initilized = true;
    IO->SetState(IOHandler::State::Normal);
    IO->OnCanvasSizeChange();
    SetTitle("Ready");
  });
}

MyFrame::~MyFrame() {
  PROFILE_SESSION_END;
  LoopTimer.Stop();
}

void MyFrame::Loop() {
  PROFILE_FUNKTION;

  if (!Initilized) {
    SetTitle(std::string("Initilizing: ").append(InitilizeDescriptor));
    return;
  }

  auto start = std::chrono::high_resolution_clock::now();

  /*auto MousePosition = wxGetMousePosition();
  IO->Keyboarddata.MousePosition = { MousePosition.x,MousePosition.y };
  */
  // for (int i = 0; i < 1000; i++) {
#ifdef Testing
  CurrentBlock = std::make_unique<VisualBlockInterior>(renderer->FrameCount);
#endif
  renderer->FrameCount++;
  //}
  IO->DoLoop();
#ifdef Testing
  renderer->Dirty = true;
#endif
#ifdef HotShaderReload
  Canvas->SetCurrent(*App->GlContext.get());
  ShaderManager::GetShader(ShaderManager::Background); // To Reload
  if (ShaderManager::IsDirty) {
    renderer->Dirty = true;
  }
#endif
  // renderer->Dirty = true;
  SetTitle(renderer->title + " " + IO->GetStateString());
  // SetTitle("HI");

  // SetTitle(std::to_string(IO->Zoom));

  // std::stringstream s;
  // s << IO->GetStateString() + " " << IO->Keyboarddata.MousePosition.x() << ",
  // " << IO->Keyboarddata.MousePosition.y(); SetTitle(IO->GetStateString());

  std::chrono::microseconds dur =
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::high_resolution_clock::now() - start);

  // static std::vector<std::chrono::microseconds> history;
  // static constexpr size_t maxHistorySize = 60; // Memory size
  // static float bestBoxSize = 100;
  // static int64_t bestDuration = std::numeric_limits<int64_t>::max();
  // static float step = 0.01; // Adjustment step
  // static float boxFloat = bestBoxSize;
  // static constexpr int minBoxSize = 1;
  // static constexpr int maxBoxSize = 100;

  // if (renderer->FrameCount % 400 == 0) {
  //	boxFloat = rand() % 100;
  // }

  //// Update the history
  // if (history.size() >= maxHistorySize) {
  //	history.erase(history.begin());
  // }
  // history.push_back(dur);

  //// Calculate average duration
  // int64_t avgDuration = std::accumulate(history.begin(), history.end(),
  // std::chrono::microseconds(0)).count() / history.size();

  //// Adjust BoxSize
  // if (avgDuration < bestDuration) {
  //	bestDuration = avgDuration;
  //	bestBoxSize = boxFloat;
  //	boxFloat += step; // Keep moving in the same direction
  // }
  // else {
  //	step = -step; // Reverse direction
  //	boxFloat += step;
  // }

  //// Clamp BoxSize to valid range
  // if (boxFloat < minBoxSize) boxFloat = minBoxSize;
  // if (boxFloat > maxBoxSize) boxFloat = maxBoxSize;

  // VisualBlockInterior::BoxSize = boxFloat;

  auto Info = LoopTimer.GetInfo();

  std::stringstream s;

#ifdef UseCollisionGrid
  s << "BoxSize: " << std::setprecision(2) << std::setw(10)
    << VisualBlockInterior::BoxSize;
#endif
  // s << " FPS:" << std::setw(10) << 1000.0 / dur.count() << " Frametime: " <<
  // std::setw(10) << dur.count() / 1000.0 << "ms" << " FPS: " << std::setw(10)
  // << std::setprecision(4) << Info.FPS
  //<< " Sleep: " << std::setw(10) <<
  // std::chrono::duration_cast<std::chrono::microseconds>(Info.Sleep).count() /
  // 1000.0 << "ms";

  s << IO->GetStateString();
  // SetTitle(s.str());
}

// void MyFrame::OnClose(wxCloseEvent &evt) {

// }
