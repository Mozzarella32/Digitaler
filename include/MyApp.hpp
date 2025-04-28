#pragma once
#include "pch.hpp"

#include "Include.hpp"

class MyApp : public wxApp {
public:

public:
	GLContextBinder* ContextBinder;
	std::unique_ptr<wxGLContext> GlContext;

	std::unique_ptr<VertexArrayObject> HoleScreenVAO;

	wxGLContextAttrs cxtAttrs;
	GLFrameIndependentInitiliser* Initlisier;

public:
	bool OnInit() override;

	void OnOGLInit();

	//bool OnExceptionInMainLoop() override;
};
