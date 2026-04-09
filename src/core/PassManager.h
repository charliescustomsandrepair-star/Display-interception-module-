#pragma once
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include "IShaderPass.h"

struct PassEntry {
    std::string name;
    std::shared_ptr<IShaderPass> pass;
    bool enabled = true;
    float gpuTimeMs = 0.0f;
};

class PassManager {
public:
    void AddPass(const std::string& name, std::shared_ptr<IShaderPass> pass) {
        if (pass) {
            m_passes.push_back({name, pass, true, 0.0f});
        }
    }

    GLuint Run(GLuint inputTex) {
        GLuint tex = inputTex;
        m_passOutputs.resize(m_passes.size(), 0);
        
        for (size_t i = 0; i < m_passes.size(); ++i) {
            auto& p = m_passes[i];
            if (!p.enabled || !p.pass) continue;
            
            auto start = std::chrono::high_resolution_clock::now();
            tex = p.pass->Execute(tex);
            auto end = std::chrono::high_resolution_clock::now();
            p.gpuTimeMs = std::chrono::duration<float, std::milli>(end - start).count();
            
            m_passOutputs[i] = tex;
            
            if (m_debugPassIndex >= 0 && (int)i == m_debugPassIndex) {
                return tex;
            }
        }
        return tex;
    }

    void Resize(int w, int h) {
        for (auto& p : m_passes) {
            if (p.pass) p.pass->Resize(w, h);
        }
    }

    void Clear() { m_passes.clear(); }
    std::vector<PassEntry>& GetPasses() { return m_passes; }
    GLuint GetPassOutput(int idx) const {
        return (idx >= 0 && idx < (int)m_passOutputs.size()) ? m_passOutputs[idx] : 0;
    }
    void SetDebugPassIndex(int idx) { m_debugPassIndex = idx; }

private:
    std::vector<PassEntry> m_passes;
    std::vector<GLuint> m_passOutputs;
    int m_debugPassIndex = -1;
};
