#pragma once
#include "pch.hpp"

#include "Include.hpp"

class MyFrame;

class MyApp : public wxApp{
public:
  MyFrame *frame;

  bool OnInit() override;

  virtual ~MyApp();

  // bool OnExceptionInMainLoop() override;
};
