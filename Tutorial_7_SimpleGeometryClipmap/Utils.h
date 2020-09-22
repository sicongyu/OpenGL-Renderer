#pragma once
#include "Predefine.h"

GLint GetVarLoc(GLuint program, std::string varName);

void SetUniformInt(GLuint Program, std::string varName, GLint val);

void SetUniformFloat(GLuint Program, std::string varName, GLfloat val);

void SetUniformVec2f(GLuint Program, std::string varName, GLfloat val0, GLfloat val1);

void SetUniformVec2f(GLuint Program, std::string varName, vec2 val);

void SetUniformVec3f(GLuint Program, std::string varName, GLfloat val0, GLfloat val1, GLfloat val2);

void SetUniformVec3f(GLuint Program, std::string varName, vec3 val);

void SetUniformMat3f(GLuint Program, std::string varName, mat3 val);

void SetUniformMat4f(GLuint Program, std::string varName, mat4 val);

void SetUniformVec4f(GLuint Program, std::string varName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

void SetUniformVec4f(GLuint Program, std::string varName, vec4 val);