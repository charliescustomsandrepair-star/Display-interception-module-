#pragma once
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

enum class UniformUIType {
    Float, Vec2, Vec3, Vec4, Int, Bool, Color
};

struct UniformInfo {
    std::string name;
    UniformUIType uiType;
    GLint location = -1;
    float minVal = 0.0f;
    float maxVal = 1.0f;
    float step = 0.01f;
    std::vector<float> defaultValue;
    std::vector<float> currentValue;
};

class UniformReflector {
public:
    static std::vector<UniformInfo> ParseGLSL(const std::string& source) {
        std::vector<UniformInfo> uniforms;
        std::regex re(R"(uniform\s+(\w+)\s+(\w+)\s*;\s*(?://\s*ui:(.*))?)");
        std::sregex_iterator it(source.begin(), source.end(), re), end;
        
        for (; it != end; ++it) {
            UniformInfo info;
            info.name = (*it)[2].str();
            std::string typeStr = (*it)[1].str();
            info.uiType = TypeStringToEnum(typeStr);
            
            // Parse UI hints: "min=0 max=5 step=0.1" or "color"
            std::string hints = (*it)[3].str();
            ParseHints(hints, info);
            
            size_t compCount = ComponentCount(info.uiType);
            info.defaultValue.resize(compCount, 0.0f);
            info.currentValue = info.defaultValue;
            uniforms.push_back(info);
        }
        return uniforms;
    }

    static void ApplyUniforms(GLuint program, const std::vector<UniformInfo>& uniforms) {
        glUseProgram(program);
        for (const auto& u : uniforms) {
            if (u.location == -1) continue;
            const float* val = u.currentValue.data();
            switch (u.uiType) {
                case UniformUIType::Float: 
                    glUniform1f(u.location, val[0]); 
                    break;
                case UniformUIType::Vec2:  
                    glUniform2fv(u.location, 1, val); 
                    break;
                case UniformUIType::Vec3:  
                    glUniform3fv(u.location, 1, val); 
                    break;
                case UniformUIType::Vec4:  
                    glUniform4fv(u.location, 1, val); 
                    break;
                case UniformUIType::Int:   
                    glUniform1i(u.location, (int)val[0]); 
                    break;
                case UniformUIType::Bool:  
                    glUniform1i(u.location, val[0] > 0.5f ? 1 : 0); 
                    break;
                case UniformUIType::Color: 
                    glUniform3fv(u.location, 1, val); 
                    break;
            }
        }
    }

private:
    static UniformUIType TypeStringToEnum(const std::string& s) {
        if (s == "float") return UniformUIType::Float;
        if (s == "vec2")  return UniformUIType::Vec2;
        if (s == "vec3")  return UniformUIType::Vec3;
        if (s == "vec4")  return UniformUIType::Vec4;
        if (s == "int")   return UniformUIType::Int;
        if (s == "bool")  return UniformUIType::Bool;
        return UniformUIType::Float;
    }

    static int ComponentCount(UniformUIType t) {
        switch (t) {
            case UniformUIType::Float: 
            case UniformUIType::Int: 
            case UniformUIType::Bool: 
                return 1;
            case UniformUIType::Vec2: 
                return 2;
            case UniformUIType::Vec3: 
            case UniformUIType::Color: 
                return 3;
            case UniformUIType::Vec4: 
                return 4;
            default: 
                return 1;
        }
    }

    static void ParseHints(const std::string& hints, UniformInfo& info) {
        if (hints.find("color") != std::string::npos) {
            info.uiType = UniformUIType::Color;
            info.minVal = 0.0f;
            info.maxVal = 1.0f;
        }
        
        std::regex numRe(R"((\w+)=([\d.]+))");
        std::sregex_iterator it(hints.begin(), hints.end(), numRe), end;
        
        for (; it != end; ++it) {
            std::string key = (*it)[1].str();
            float val = std::stof((*it)[2].str());
            if (key == "min") 
                info.minVal = val;
            else if (key == "max") 
                info.maxVal = val;
            else if (key == "step") 
                info.step = val;
        }
    }
};
