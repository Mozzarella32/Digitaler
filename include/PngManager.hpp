#pragma once

#include "pch.h"

#include "Include.h"

#include"Art/Png_X_List.h"

//Singilton
class PngManager {
private:
	PngManager();
private:
	static PngManager& GetInstance();

public:
#define X(Name) Name,
	enum Pngs {
		XList_Png_Images
	};
#undef X

private:

	std::unordered_map<Pngs, wxImage> Map;

public:

	static const wxImage& GetPng(const Pngs& xpm);
};