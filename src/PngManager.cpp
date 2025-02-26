#include "pch.hpp"
#include "PngManager.hpp"

#define DEFINE_MAKRO_UTILITIES
#include "Makros.hpp"

#include "../resources/art/Png_Includes.h"


PngManager::PngManager()
	:
#define X(Name) {Name,\
[](){\
	wxMemoryInputStream stream(CONCAT(Source_,CONCAT(Name, _png)), CONCAT(Source_,CONCAT(Name, _png_len)));\
	return wxImage(stream, wxBITMAP_TYPE_PNG);\
}()},
	Map{
	XList_Png_Images
}
#undef X
{
#define X(Name) Map.at(Name);
	XList_Png_Images
#undef X
}

PngManager& PngManager::GetInstance() {
	static PngManager This;
	return This;
}

const wxImage& PngManager::GetPng(const Pngs& xpm) {
	const auto& This = GetInstance();
	return This.Map.at(xpm);
}
