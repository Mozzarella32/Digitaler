#include "pch.hpp"
#include "ShaderManager.hpp"

#define DEFINE_MAKRO_UTILITIES
#include "Makros.hpp"

#include "../resources/shaders/Shader_Includes.h"

bool ShaderManager::IsDirty = false;

#define X(Vert, Frag) \
Map.emplace(Frag,\
std::make_unique<Shader>(\
ErrorHandler,\
std::string(CONCAT(Vert, _vert)),\
std::string(CONCAT(Frag, _frag)))\
);

#ifdef HotShaderReload
ShaderManager::~ShaderManager() {
	Running = false;
	Worker.join();
}
#endif

ShaderManager::ShaderManager()
/*:
Map{
	XList_Shaders_Combined
}*/
{
	XList_Shaders_Combined
#ifdef HotShaderReload
	Worker = std::thread(&ShaderManager::Work, this);
#endif
}

ShaderManager& ShaderManager::GetInstance() {
	static ShaderManager This;
	return This;
}



#define X(Vert,Frag) \
UpdateShader(Frag,std::string("../Digitaler/Shaders/Source/")+STRINGIFY(Vert)+".vert",std::string("../Digitaler/Shaders/Source/")+STRINGIFY(Frag)+".frag");

#ifdef HotShaderReload
void ShaderManager::Work() {
	while (Running) {
		XList_Shaders_Combined
		Sleep(16);
	}
}

void ShaderManager::UpdateShader(const Shaders& shader, const std::filesystem::path& Vert, const std::filesystem::path& Frag) {
	auto VertTime = std::filesystem::last_write_time(Vert);
	auto FragTime = std::filesystem::last_write_time(Frag);

	bool Update = false;

	if (!LastUpdate.contains(shader)) {
		Update = true;
	}
	else {
		const auto& [LastVertTime, LastFragTime] = LastUpdate.at(shader);

		if (LastVertTime != VertTime) {
			Update = true;
		}
		if (LastFragTime != FragTime) {
			Update = true;
		}
	}

	if (!Update) return;
	LastUpdate[shader] = std::make_tuple(VertTime, FragTime);
	std::unique_lock ul(QueueMutex);
	Queue.emplace_back(shader, Vert, Frag);
}
#endif

const std::unique_ptr<Shader>& ShaderManager::GetShader(const Shaders& shader) {
	auto& This = GetInstance();
#ifdef HotShaderReload
	std::unique_lock ul(This.QueueMutex);
	if (This.Queue.empty() && This.Map.contains(shader)) return This.Map.at(shader);
	while (!This.Queue.empty()) {
		const auto& [shader, Vert, Frag] = This.Queue.back();
		This.Map[shader] = std::make_unique<Shader>(ErrorHandler, Vert, Frag);
		This.Queue.pop_back();
	}
	This.IsDirty = true;
	return This.Map.at(shader);
#else
	return This.Map.at(shader);
#endif
	
	
}

void ShaderManager::Initilise() {
	GetInstance();
}
