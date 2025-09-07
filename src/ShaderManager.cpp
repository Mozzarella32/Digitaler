#include "pch.hpp"
#include <chrono>
#include <concepts>
#include <functional>
#include <memory>
#include <utility>
#include "ShaderManager.hpp"

#define DEFINE_MAKRO_UTILITIES
#include "Makros.hpp"

#include "../resources/shaders/Shader_Includes.hpp"

#ifdef HotShaderReload
bool ShaderManager::IsDirty = false;
bool ShaderManager::allowReload = false;
#endif

#ifdef HotShaderReload
ShaderManager::~ShaderManager() {
	ShaderManager::Stop();
}
#endif

ShaderManager::ShaderManager()
    : shaders{
#define X(Vert, Frag) \
    ([]() { \
        const char* vert_ptr = CONCAT(Vert, _vert); \
        size_t vert_len = CONCAT(Vert, _vert_len); \
        auto vert_src = std::string(vert_ptr, vert_len); \
        const char* frag_ptr = CONCAT(Frag, _frag); \
        size_t frag_len = CONCAT(Frag, _frag_len); \
        auto frag_src = std::string(frag_ptr, frag_len); \
        return Shader(ErrorHandler, { \
            Shader::ShaderInfo{Shader::ShaderType::Vertex, Shader::ShaderSource{vert_src}}, \
            Shader::ShaderInfo{Shader::ShaderType::Fragment, Shader::ShaderSource{frag_src}} \
        }); \
    })(),

XList_Shaders_VertFrag

#undef X

#define X(Vert, Geom, Frag) \
    ([]() { \
        const char* vert_ptr = CONCAT(Vert, _vert); \
        size_t vert_len = CONCAT(Vert, _vert_len); \
        auto vert_src = std::string(vert_ptr, vert_len); \
        const char* geom_ptr = CONCAT(Geom, _geom); \
        size_t geom_len = CONCAT(Geom, _geom_len); \
        auto geom_src = std::string(geom_ptr, geom_len); \
        const char* frag_ptr = CONCAT(Frag, _frag); \
        size_t frag_len = CONCAT(Frag, _frag_len); \
        auto frag_src = std::string(frag_ptr, frag_len); \
        return Shader(ErrorHandler, { \
            Shader::ShaderInfo{Shader::ShaderType::Vertex, Shader::ShaderSource{vert_src}}, \
            Shader::ShaderInfo{Shader::ShaderType::Geometry, Shader::ShaderSource{geom_src}}, \
            Shader::ShaderInfo{Shader::ShaderType::Fragment, Shader::ShaderSource{frag_src}} \
        }); \
    })(),

XList_Shaders_VertGeomFrag

#undef X

      }
{
#ifdef HotShaderReload
    Worker = std::thread(&ShaderManager::Work, this);
#endif
}

void ShaderManager::Stop() {
	auto& This = GetInstance();
	if (This.Running) {
		This.Running = false;
		This.WaitCV.notify_all();
		This.Worker.join();
	}
}

ShaderManager& ShaderManager::GetInstance() {
	static ShaderManager This;
	return This;
}

#ifdef HotShaderReload
void ShaderManager::Work() {
    while (Running) {

#define X(Vert, Frag) \
        { \
            std::ostringstream vert_ss; \
            vert_ss << "../resources/shaders/Source/" << STRINGIFY(Vert) << ".vert"; \
            std::string vert_path = vert_ss.str(); \
            \
            std::ostringstream frag_ss; \
            frag_ss << "../resources/shaders/Source/" << STRINGIFY(Frag) << ".frag"; \
            std::string frag_path = frag_ss.str(); \
            \
            UpdateShader(Frag, vert_path, std::nullopt, frag_path); \
        }
        XList_Shaders_VertFrag
#undef X

#define X(Vert, Geom, Frag) \
        { \
            std::ostringstream vert_ss; \
            vert_ss << "../resources/shaders/Source/" << STRINGIFY(Vert) << ".vert"; \
            std::string vert_path = vert_ss.str(); \
            \
            std::ostringstream geom_ss; \
            geom_ss << "../resources/shaders/Source/" << STRINGIFY(Geom) << ".geom"; \
            std::string geom_path = geom_ss.str(); \
            \
            std::ostringstream frag_ss; \
            frag_ss << "../resources/shaders/Source/" << STRINGIFY(Frag) << ".frag"; \
            std::string frag_path = frag_ss.str(); \
            \
            UpdateShader(Frag, vert_path, geom_path, frag_path); \
        }
        XList_Shaders_VertGeomFrag
#undef X

        std::unique_lock ulSleep(WaitCVMutex);
        WaitCV.wait_for(ulSleep, std::chrono::milliseconds(16));
    }
}

void ShaderManager::UpdateShader(const Shaders& shader, const std::filesystem::path& Vert, const std::optional<std::filesystem::path>& Geom, const std::filesystem::path& Frag) {
	PROFILE_FUNKTION;
	std::filesystem::file_time_type VertTime;
	std::filesystem::file_time_type GeomTime{};
	std::filesystem::file_time_type FragTime;
	try {
		 VertTime = std::filesystem::last_write_time(Vert);
		 if(Geom) {
			 GeomTime = std::filesystem::last_write_time(Geom.value());
		 }
		 FragTime = std::filesystem::last_write_time(Frag);
	}
	catch (...) {
		return;
	}
	bool Update = false;

	if (!LastUpdate.contains(shader)) {
		Update = true;
	}
	else {
		const auto& [LastVertTime, LastGeomTime, LastFragTime] = LastUpdate.at(shader);

		if (LastVertTime != VertTime) {
			Update = true;
		}
		if(Geom && LastGeomTime != GeomTime){
			Update = true;
		}
		if (LastFragTime != FragTime) {
			Update = true;
		}
	}

	if (!Update) return;
	LastUpdate[shader] = std::make_tuple(VertTime, GeomTime, FragTime);
	std::unique_lock ul(QueueMutex);
	Queue.emplace_back(shader, Vert, Geom, Frag);
}
#endif

Shader& ShaderManager::GetShader(const Shaders& shader) {
	PROFILE_FUNKTION;
	auto& This = GetInstance();
#ifdef HotShaderReload
	std::unique_lock ul(This.QueueMutex);
	if (This.Queue.empty() || !This.allowReload) return This.shaders[shader];
	while (!This.Queue.empty()) {
		const auto& [currShader, Vert, Geom, Frag] = This.Queue.back();
		std::vector<Shader::ShaderInfo> shaders = {
			Shader::ShaderInfo{Shader::ShaderType::Vertex, Vert},
			Shader::ShaderInfo{Shader::ShaderType::Fragment, Frag},
		};
		if(Geom){
			shaders.emplace_back(Shader::ShaderType::Geometry, Geom.value());
		}
		This.shaders[currShader] = Shader(ErrorHandler, shaders);
		This.Queue.pop_back();
	}
	This.IsDirty = true;
	return This.shaders[shader];
#else
	return This.shaders[shader];
#endif
}

void ShaderManager::applyGlobal(const std::string& uniform, const Shader::UniformData& data) {
	PROFILE_FUNKTION;
	auto& This = GetInstance();
	auto it = This.shadersWithUniform.find(uniform);
	if (it == This.shadersWithUniform.end()) {
		std::vector<std::reference_wrapper<Shader>> shaders;
		for(auto& shader : This.shaders){
			if(shader.uniformLocation(uniform) == -1) {
				continue;
			}
			shaders.push_back(shader);
		}
		This.shadersWithUniform[uniform] = std::move(shaders);
		it = This.shadersWithUniform.find(uniform);
	}

  for (const auto &rShader : it->second) {
  	auto& shader = rShader.get();
    shader.bind();
    shader.apply(uniform, data);
    shader.unbind();
  }
}

void ShaderManager::Initilise() {
	PROFILE_FUNKTION;
	GetInstance();
}

std::unique_ptr<Shader> ShaderManager::PlacholderShader;

Shader& ShaderManager::GetPlacholderShader() {
	PROFILE_FUNKTION;
        if (!PlacholderShader) {
          PlacholderShader = std::make_unique<Shader>(
              ErrorHandler, std::vector<Shader::ShaderInfo>{
                                Shader::ShaderInfo{Shader::ShaderType::Vertex,
                                                   std::string(R"--(
//PlacholderShader.vert

#version 330 core

//In 
layout(location = 0) in vec2 InPosition;

//Out
out vec2 TextureCoord;

void main() {
	 TextureCoord = (InPosition+1)/2;
	gl_Position = vec4(InPosition,0.0,1.0);
}
)--")},
                                Shader::ShaderInfo{Shader::ShaderType::Fragment,
                                                   std::string(R"--(
//PlacholderShader.frag

#version 330 core

// In
in vec2 TextureCoord;

// Out
out vec4 FragColor;

// Uniform
uniform vec2 USize;

void main() {
    vec2 pixelCoord = TextureCoord * USize;

    int x = int(floor(pixelCoord.x)) % 2;
    int y = int(floor(pixelCoord.y)) % 2;

    if (x == y) {
        FragColor = vec4(0.0,0.0,0.0, 1.0); 
    } else {
        FragColor = vec4(0.953, 0.286, 0.961, 1.0);
    }
}
)--")}});
        }
        return *PlacholderShader;
}
