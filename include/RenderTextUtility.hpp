#pragma once

#include "pch.hpp"
//
//class TextSDFManager {
//private:
//	TextSDFManager();
//
//	void StartWorker();
//
//	void StopWorker();
//
//	~TextSDFManager();
//
//	wxFont font;
//
//	std::mutex QueuesMutex;
//
//	std::mutex CVMutex;
//	std::condition_variable cv;
//	std::thread worker;
//
//	bool Terminate = false;
//
//	std::map<std::string, std::unique_ptr<Texture>> SDFs;
//
//	std::queue<std::tuple<std::string, std::vector<uint8_t>, int, int>> ToBeMadeIntoTexture;
//	std::queue<std::pair<std::string, wxImage>> ToBeProcessed;
//	std::queue<std::string> ToBeRendered;
//
//	std::unordered_set<std::string> Queue;
//
//	void AddToQueue(const std::string& string);
//
//	static wxImage RenderTextToImage(const wxString& text, const wxFont& font, int width, int height);
//
//	void ConvertImageToShape(const wxImage& image, msdfgen::Shape& shape);
//
//	std::vector<uint8_t>  GenerateMSDFTexture(msdfgen::Shape& shape, int width, int height);
//
//	std::unique_ptr<Texture> GenerateSDFTexture(const std::vector<uint8_t>& vec, int width, int height);
//
//	void Loop();
//
//public:
//
//	std::optional<std::reference_wrapper<Texture>> GetSDF(const std::string& string);
//
//	void setFont(const wxFont& f);
//
//	void ProcesOrRenderSthAtMostNTimes(int n);
//
//	static TextSDFManager& Instance();
//};

extern bool generateAtlas(const char* fontFilename);