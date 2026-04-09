#include "HDRNormalizePass.h"
#include <GL/glew.h>

bool HDRNormalizePass::Init(int width, int height) {
    m_width = width;
    m_height = height;
    m_name = "HDR Normalize";
    
    if (!LoadShader("shaders/hdr_normalize.frag")) {
        return false;
    }
    
    return SetupFramebuffer();
}

bool HDRNormalizePass::SetupFramebuffer() {
    // Create texture
    glGenTextures(1, &m_outputTex);
    glBindTexture(GL_TEXTURE_2D, m_outputTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create framebuffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_outputTex, 0);
    
    bool success = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return success;
}

GLuint HDRNormalizePass::Execute(GLuint inputTex) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    
    glUseProgram(m_program);
    
    // Set uniforms
    glUniform1i(glGetUniformLocation(m_program, "u_inputTexture"), 0);
    glUniform1i(glGetUniformLocation(m_program, "u_colorSpace"), m_colorSpace);
    glUniform1f(glGetUniformLocation(m_program, "u_displayMaxNits"), m_maxNits);
    glUniform1f(glGetUniformLocation(m_program, "u_sdrReferenceWhite"), m_sdrRefWhite);
    
    ApplyUniforms();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTex);

    // Draw fullscreen quad
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return m_outputTex;
}

void HDRNormalizePass::Resize(int width, int height) {
    m_width = width;
    m_height = height;
    
    if (m_outputTex) glDeleteTextures(1, &m_outputTex);
    if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
    
    SetupFramebuffer();
}
