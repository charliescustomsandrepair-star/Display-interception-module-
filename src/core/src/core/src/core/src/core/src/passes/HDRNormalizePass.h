#pragma once
#include "../core/ReflectiveShaderPass.h"

class HDRNormalizePass : public ReflectiveShaderPass {
public:
    bool Init(int width, int height) override;
    GLuint Execute(GLuint inputTex) override;
    void Resize(int width, int height) override;

    void SetColorSpace(int cs) { m_colorSpace = cs; }
    void SetDisplayMaxNits(float nits) { m_maxNits = nits; }
    void SetSDRReferenceWhite(float nits) { m_sdrRefWhite = nits; }

private:
    bool SetupFramebuffer();
    
    int m_width = 0, m_height = 0;
    GLuint m_fbo = 0, m_outputTex = 0;
    int m_colorSpace = 0;  // 0=SDR, 1=scRGB, 2=PQ
    float m_maxNits = 80.0f;
    float m_sdrRefWhite = 80.0f;
};
