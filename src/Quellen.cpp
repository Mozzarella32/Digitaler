#include "MyApp.hpp"
#include "MyFrame.hpp"
#include "ShaderManager.hpp"
#include "pch.hpp"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
  PROFILE_FUNKTION;
#ifdef _WIN32
  SetProcessDPIAware();
#endif

  srand((unsigned int)time(NULL));

  {
    PROFILE_SCOPE("Set WD");
    wxFileName exePath(wxStandardPaths::Get().GetExecutablePath());
    wxFileName exeDir = exePath.GetPath();
    wxSetWorkingDirectory(exeDir.GetFullPath());
  }

  {
    PROFILE_SCOPE("Init Image Handlers");
    wxInitAllImageHandlers();
  }

  // wxGLAttributes glAttrs;
  // {
  //   PROFILE_SCOPE("Request Context");

  //   glAttrs.PlatformDefaults().Defaults().EndList();
  //   cxtAttrs.CoreProfile().OGLVersion(4, 5).EndList();
  // }
  // Initlisier = new GLFrameIndependentInitiliser(
  //     glAttrs, cxtAttrs,
  //     [this]() {
  //       if (!gladLoadGL()) {
  //         wxMessageBox("Failed to initialize GLAD!", "Error", wxICON_ERROR);
  //       }
  //     },
  //     [this]() { OnOGLInit(); });
  // return true;
  MyFrame *Frame = new MyFrame(this);
  Frame->Show();
  return true;
}

// void MyApp::OnGLInit() {
//   PROFILE_FUNKTION;
//   // {
//   //   PROFILE_SCOPE("Retrive Context");
//   //   GlContext.reset(Initlisier->RetriveContextAndClose());
//   // }

//   // {
//   //   PROFILE_SCOPE("Create Context Binder");
//   //   // Create ContextBinder
//   //   ContextBinder = new GLContextBinder();
//   //   ContextBinder->BindContext(GlContext.get());
//   // }

//   Frame->OnGLInit();
// }

//
// bool MyApp::OnExceptionInMainLoop() {
//	try {
//		throw;
//	}
//	//catch (nlohmann::json_abi_v3_11_3::detail::type_error& e) {
//
//	//}
//	catch (std::exception& e)
//	{
//		//MessageBoxA(NULL, e.what(), "C++ Exception Caught", MB_OK);
//	}
//
//
//	return false;   // continue on. Return false to abort program
//}
