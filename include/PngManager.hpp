#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "../resources/art/Png_X_List.hpp"

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

	std::unordered_map<Pngs, std::function<wxImage(void)>> pngLoaders;
	std::unordered_map<Pngs, wxImage> Map;

public:

	static const wxImage& GetPng(const Pngs& xpm);
};