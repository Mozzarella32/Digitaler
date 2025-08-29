#pragma once

#include "pch.hpp"

#include "Include.hpp"
#include "../resources/shaders/Shader_X_List.hpp"
#include <condition_variable>
#include <functional>
#include <memory>

//Singilton
class ShaderManager {
private:
	ShaderManager();

#ifdef HotShaderReload
	~ShaderManager();
#endif
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

	std::mutex WaitCVMutex;
	std::condition_variable WaitCV;
	std::vector<std::tuple<Shaders, std::filesystem::path, std::filesystem::path>> Queue;

	bool Running = true;

	std::unordered_map<Shaders, std::pair<std::filesystem::file_time_type, std::filesystem::file_time_type>> LastUpdate;

	void Work();

	void UpdateShader(const Shaders& shader,const std::filesystem::path& Vert, const std::filesystem::path& Frag);
public:
	static bool IsDirty;

#endif
	std::array<Shader,ShadersSize> shaders;


private:
	std::map<std::string, std::vector<std::reference_wrapper<Shader>>> shadersWithUniform;

	static std::unique_ptr<Shader> PlacholderShader;
public:
	static void applyGlobal(const std::string& uniform, const Shader::UniformData& data);

	static Shader& GetShader(const Shaders& shader);

	static void Initilise();

	static void Stop();

	static const std::unique_ptr<Shader>& GetPlacholderShader();
};
