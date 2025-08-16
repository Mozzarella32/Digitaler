#pragma once
#include "pch.hpp"

#include "Include.hpp"

class MyFrame;

class MyApp : public wxApp{
public:
  MyFrame *frame;

public:
  // GLContextBinder* ContextBinder;
  // std::unique_ptr<wxGLContext> GlContext;

  // std::unique_ptr<VertexArrayObject> HoleScreenVAO;

  // wxGLContextAttrs cxtAttrs;
  // GLFrameIndependentInitiliser* Initlisier;

public:
  bool OnInit() override;

  // void OnGLInit() override;

  // bool OnExceptionInMainLoop() override;
};
