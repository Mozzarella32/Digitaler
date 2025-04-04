#include "Include.hpp"
#include "pch.hpp"

class MyApp;
class IOHandler;
class Renderer;
class VisualBlockInterior;
class DataResourceManager;

class MyFrame : public wxFrame {
	GameLoopTimer LoopTimer;

#ifdef HotShaderReload
	MyApp* App;
#endif

public:
	bool Initilized = false;
	std::unique_ptr<IOHandler> IO;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<DataResourceManager> BlockManager;
	wxGLCanvas* Canvas;
	//std::unique_ptr<Board> board;

private:
	//void PositionButtons(wxGLCanvas* Canvas, std::vector<wxButton*> Buttons);

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

	void OnQuitMenu(wxCommandEvent& evt) {};

	void OnNewMenu(wxCommandEvent& evt) {};

	void OnSaveMenu(wxCommandEvent& evt) {};
	void OnOpenMenu(wxCommandEvent& evt) {};

	void OnImportMenu(wxCommandEvent& evt) {};
	void OnExportMenu(wxCommandEvent& evt) {};

	void OnHelpMenu(wxCommandEvent& evt) {};

	void OnCopyMenu(wxCommandEvent& evt) {};
	void OnCutMenu(wxCommandEvent& evt) {};
	void OnPasteMenu(wxCommandEvent& evt) {};

	//void PostMoveUpdates(bool render);

	//void OnEscape();
	//void OnEnter();

	wxSlider* SpeedSlider;
	wxButton* StartStopButton;

	void OnSingleStepButton(wxCommandEvent& evt) {};
	void OnStartStopButton(wxCommandEvent& evt) {};
	void OnHomeButton(wxCommandEvent& evt) {};
	void OnSpeedSlider() {};


public:
	MyFrame(MyApp* App);

	~MyFrame();
private:
	void Loop();

	void OnMouseMove(wxMouseEvent& evt) {};
	void OnMouseDown(wxMouseEvent& evt) {};
	void OnMouseUp(wxMouseEvent& evt) {};
	void OnLeaveWindow(wxMouseEvent& evt) {};

	void OnKeyDown(wxKeyEvent& evt) {};
	void OnKeyUp(wxKeyEvent& evt) {};

	void OnChar(wxKeyEvent& evt);
};
