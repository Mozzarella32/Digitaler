#include "pch.hpp"
#include "MyApp.hpp"
#include "MyFrame.hpp"
#include "ShaderManager.hpp"

wxIMPLEMENT_APP(MyApp);

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	// Beep(200, 1000);
	// DebugBreak();
	std::ofstream o("Error.log", std::ios::app);
	o << "[OpenGL ErrorCallback] ";
	o << "Source: ";
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		o << "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		o << "Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		o << "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		o << "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		o << "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		o << "Other";
		break;
	default:
		o << "Unknown";
		break;
	}
	o << " Type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		o << "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		o << "Deprecated Behavior";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		o << "Undefined Behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		o << "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		o << "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		o << "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		o << "Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		o << "Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		o << "Other";
		break;
	default:
		o << "Unknown";
		break;
	}
	o << " ID: " << id;
	o << " as Error: " << gluErrorString(id);

	o << " Severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		o << "High";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		o << "Medium";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		o << "Low";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		o << "Notification";
		break;
	default:
		o << "Unknown";
		break;
	}
	o << "\n";

	o << "Message: " << message;
	o << std::endl;
}

bool MyApp::OnInit() {
#ifdef _WIN32
	SetProcessDPIAware();
#endif
	//wxSystemOptions::SetOption(_T("msw.remap"), 0);
	srand((unsigned int)time(NULL));

	wxFileName exePath(wxStandardPaths::Get().GetExecutablePath());
	wxFileName exeDir = exePath.GetPath();
	wxSetWorkingDirectory(exeDir.GetFullPath());

	wxInitAllImageHandlers();

	//Init Context - for wxGlCanvas
	auto* Initlisier = new GLEWFrameIndependentInitiliser();

	GlContext.reset(Initlisier->RetriveContextAndClose());

	//Create ContextBinder
	ContextBinder = new GLContextBinder();
	ContextBinder->BindContext(GlContext.get());

	ShaderManager::Initilise();

	//Create Static VAO
	HoleScreenVAO = std::make_unique<VertexArrayObject>(
		std::vector<VertexBufferObjectDescriptor>{
			{
				GLenum(GL_STATIC_DRAW), CoordVertex()
			}
	});

	HoleScreenVAO->ReplaceVertexBuffer(
		std::vector<CoordVertex>{
			{ -1.0, -1.0 },
			{ -1.0,1.0 },
			{ 1.0,-1.0 },
			{ 1.0,1.0 },
	}, 0);
	
	
	//Init Opengl
#ifdef _DEBUG
	{
		std::ofstream o("Error.log");
	}

	GLCALL(glEnable(GL_DEBUG_OUTPUT));
	GLCALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
	GLCALL(glDebugMessageCallback(MessageCallback, 0));
#endif

	//wxImage I("C:\\Users\\valen\\Desktop\\Atlas-From-wxwidgets.png",wxBITMAP_TYPE_PNG);

	GLCALL(glEnable(GL_BLEND));
	GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


	GLCALL(glEnable(GL_STENCIL_TEST));
	GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
	GLCALL(glStencilMask(0x00));

	new MyFrame(this);

	return true;
}

//
//bool MyApp::OnExceptionInMainLoop() {
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
