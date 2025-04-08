#include "pch.hpp"
#include "ShaderManager.hpp"

#define DEFINE_MAKRO_UTILITIES
#include "Makros.hpp"

#include "../resources/shaders/Shader_Includes.hpp"

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
#undef X

ShaderManager& ShaderManager::GetInstance() {
	static ShaderManager This;
	return This;
}



#define X(Vert,Frag) \
UpdateShader(Frag,std::string("../resources/shaders/Source/")+STRINGIFY(Vert)+".vert",std::string("../resources/shaders/Source/")+STRINGIFY(Frag)+".frag");

#ifdef HotShaderReload
void ShaderManager::Work() {
	while (Running) {
		XList_Shaders_Combined
#ifdef _WIN32
		Sleep(16);
#else
		sleep(16);
#endif
	}
}

void ShaderManager::UpdateShader(const Shaders& shader, const std::filesystem::path& Vert, const std::filesystem::path& Frag) {
	PROFILE_FUNKTION;
	std::filesystem::file_time_type VertTime;
	std::filesystem::file_time_type FragTime;
	try {
		 VertTime = std::filesystem::last_write_time(Vert);
		 FragTime = std::filesystem::last_write_time(Frag);
	}
	catch (...) {
		Beep(200, 200);
		return;
	}
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
	PROFILE_FUNKTION;
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

const std::vector<std::pair<ShaderManager::Shaders, GLenum>>& ShaderManager::GetShadersWithUniform(const std::string& uniform) {
	PROFILE_FUNKTION;
	auto& This = GetInstance();
	auto it = This.Uniforms.find(uniform);
	if (it != This.Uniforms.end()) {
		return it->second;
	}
	std::vector<std::pair<Shaders, GLenum>> shaders;

	for (int i = 0; i < ShadersSize; i++) {
		auto& shader = This.Map.at((Shaders)i);
		size_t Location = shader->GetLocation(uniform);
		if (Location != -1) {
			shaders.emplace_back((Shaders)i, Location);
		}
	}

	This.Uniforms[uniform] = shaders;
	return This.Uniforms.at(uniform);
}

void ShaderManager::Initilise() {
	PROFILE_FUNKTION;
	GetInstance();
}
