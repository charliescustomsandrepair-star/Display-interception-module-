#pragma once
#include "IShaderPass.h"
#include "UniformReflector.h"
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

class ReflectiveShaderPass : public IShaderPass {
public:
    virtual ~ReflectiveShaderPass() {
        if (m_program) glDeleteProgram(m_program);
    }

    bool LoadShader(const std::string& fragPath, const std::string& vertPath = "shaders/fullscreen.vert") {
        std::string fragSrc = ReadFile(fragPath);
        std::string vertSrc = ReadFile(vertPath);
        
        if (fragSrc.empty() || vertSrc.empty()) {
            return false;
        }
        
        m_program = CompileProgram(vertSrc.c_str(), fragSrc.c_str());
        if (!m_program) return false;
        
        m_uniforms = UniformReflector::ParseGLSL(fragSrc);
        for (auto& u : m_uniforms) {
            u.location = glGetUniformLocation(m_program, u.name.c_str());
        }
        return true;
    }

    virtual std::vector<UniformInfo>& GetUniforms() { 
        return m_uniforms; 
    }

    void ApplyUniforms() {
        UniformReflector::ApplyUniforms(m_program, m_uniforms);
    }

    GLuint GetProgram() const { 
        return m_program; 
    }

    const char* GetName() const override { 
        return m_name.c_str(); 
    }

protected:
    GLuint m_program = 0;
    std::vector<UniformInfo> m_uniforms;
    std::string m_name = "ReflectivePass";

    static std::string ReadFile(const std::string& path) {
        std::ifstream f(path);
        if (!f.is_open()) return "";
        return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    }

    static GLuint CompileProgram(const char* vs, const char* fs) {
        GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vShader, 1, &vs, nullptr);
        glCompileShader(vShader);
        
        int success;
        glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(vShader, 512, nullptr, log);
            glDeleteShader(vShader);
            return 0;
        }

        GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fShader, 1, &fs, nullptr);
        glCompileShader(fShader);
        
        glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(fShader, 512, nullptr, log);
            glDeleteShader(vShader);
            glDeleteShader(fShader);
            return 0;
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, vShader);
        glAttachShader(program, fShader);
        glLinkProgram(program);
        
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(program, 512, nullptr, log);
            glDeleteShader(vShader);
            glDeleteShader(fShader);
            glDeleteProgram(program);
            return 0;
        }

        glDeleteShader(vShader);
        glDeleteShader(fShader);
        return program;
    }
};
