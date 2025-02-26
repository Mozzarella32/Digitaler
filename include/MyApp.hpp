#pragma once
#include "pch.hpp"

#include "Include.hpp"

class MyApp : public wxApp {
public:

public:
	GLContextBinder* ContextBinder;
	std::unique_ptr<wxGLContext> GlContext;

	std::unique_ptr<VertexArrayObject> HoleScreenVAO;

public:
	bool OnInit() override;

	//bool OnExceptionInMainLoop() override;
};