#pragma once

#include "pch.hpp"

#include "Include.hpp"
#include "../resources/shaders/Shader_X_List.hpp"

//Singilton
class ShaderManager {
private:
	ShaderManager();
	~ShaderManager();
private:
	static ShaderManager& GetInstance();

	static auto inline ErrorHandler = [](std::string Error, std::string Msg) {
		wxMessageBox(Msg, Error, wxOK);
		};

public:
#define X(Name) Name,
	enum Shaders {
		XList_Shaders_Names
		ShadersSize
	};
#undef X

private:

#ifdef HotShaderReload
	std::thread Worker;

	std::mutex QueueMutex;
	std::vector<std::tuple<Shaders, std::filesystem::path, std::filesystem::path>> Queue;

	bool Running = true;

	std::unordered_map<Shaders, std::pair<std::filesystem::file_time_type, std::filesystem::file_time_type>> LastUpdate;

	void Work();

	void UpdateShader(const Shaders& shader,const std::filesystem::path& Vert, const std::filesystem::path& Frag);
public:
	static bool IsDirty;

#endif
	std::unordered_map<Shaders, std::unique_ptr<Shader>> Map;


private:
	std::map<std::string, std::vector<std::pair<Shaders, GLenum>>> Uniforms;
public:

	static const std::vector<std::pair<Shaders, GLenum>>& GetShadersWithUniform(const std::string& uniform);

	static const std::unique_ptr<Shader>& GetShader(const Shaders& shader);

	static void Initilise();
};
