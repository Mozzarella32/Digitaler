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
	auto error = gluErrorString(id);
	if (error == nullptr) {
		o << " as Error: " <<  "gluErrorString returned nullptr";
	}
	else {
		o << " as Error: " << error;
	}

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

/*1
;
		cxtAttrs.CoreProfile().OGLVersion(4, 5).EndList();
*/


bool MyApp::OnInit() {
	PROFILE_FUNKTION;
#ifdef _WIN32
	SetProcessDPIAware();
#endif
	//wxSystemOptions::SetOption(_T("msw.remap"), 0);
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

	{
		PROFILE_SCOPE("Request Context");
		cxtAttrs.CoreProfile().OGLVersion(4, 5).EndList();
		//Init Context - for wxGlCanvas
	// 	Initlisier = new GLFrameIndependentInitiliser(cxtAttrs,
	// 	[this](){
	// 		GLFWwindow* window;

	// 		if(!glfwInit()){
	// 				std::cout << "Faild to initilize glfw\n";
	// 				return;
	// 		}

	// 		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	// 		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	// 		window = glfwCreateWindow(1, 1, "GLFW context init", NULL, NULL);
	// 		if (!window)
	// 		{
	// 			glfwTerminate();
	// 			exit(EXIT_FAILURE);
	// 		}

	// 		glfwMakeContextCurrent(window);

	// 		if (!gladLoadGL(glfwGetProcAddress)) {
	// 				std::cout << "GLAD could not be initilised\n";
	// 				return;
	// 		}	
			
	// 		glfwDestroyWindow(window);
 
	// 		glfwTerminate();
	// 	},
	// 	[this]() {OnOGLInit(); });
	}
	Initlisier = new GLFrameIndependentInitiliser(cxtAttrs,
	  [this](){
			GLenum glewInitResult = glewInit();
			if (glewInitResult != GLEW_OK) {
					const GLubyte* errorString = glewGetErrorString(glewInitResult);
					std::cout << "GLEW-Fehler: " << errorString << std::endl;
					wxASSERT_MSG(false, wxString::Format("GLEW-Fehler: %s", errorString));
			}},
		[this]() {OnOGLInit(); });
	return true;
}

void MyApp::OnOGLInit() {
	PROFILE_FUNKTION;
	{
		PROFILE_SCOPE("Retrive Context");
		GlContext.reset(Initlisier->RetriveContextAndClose());
	}

	{
		PROFILE_SCOPE("Create Context Binder");
		//Create ContextBinder
		ContextBinder = new GLContextBinder();
		ContextBinder->BindContext(GlContext.get());
	}

	{
		PROFILE_SCOPE("Create HoleScreenVAO");
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
	}

	{
		PROFILE_SCOPE("Setting up Opengl");
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
	}
	{
		PROFILE_SCOPE("Creating Frame");
		new MyFrame(this);
	}
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
