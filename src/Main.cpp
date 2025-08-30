#include "pch.hpp"

#include "MyApp.hpp"
#include "MyFrame.hpp"
#include "ShaderManager.hpp"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
  PROFILE_FUNKTION;

#ifdef NDEBUG
  std::cout << "Target: RELEASE\n";
#else
  std::cout << "Target: DEBUG\n";
#endif

#ifdef _WIN32
  SetProcessDPIAware();
#endif

  srand((unsigned int)time(NULL));

  wxFileName exePath(wxStandardPaths::Get().GetExecutablePath());
  wxFileName exeDir = exePath.GetPath();
  wxSetWorkingDirectory(exeDir.GetFullPath());
 
  PROFILE_SESSION_START("Hi",
                        std::filesystem::path(wxGetHomeDir().ToStdString()) /
                            "Profiled.json");
  
  {
    PROFILE_SCOPE("Init Image Handlers");
    wxInitAllImageHandlers();
  }

  frame = new MyFrame(this);
  frame->Show();
  frame->Refresh();
  frame->Update();
  return true;
}

MyApp::~MyApp() {
  ShaderManager::Stop();
  PROFILE_SESSION_END;
}

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
