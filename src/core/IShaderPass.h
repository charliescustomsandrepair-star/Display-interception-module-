#pragma once
#include <GL/glew.h>

class IShaderPass {
public:
    virtual ~IShaderPass() = default;
    virtual bool Init(int width, int height) = 0;
    virtual GLuint Execute(GLuint inputTex) = 0;
    virtual void Resize(int width, int height) = 0;
    virtual const char* GetName() const = 0;
};
