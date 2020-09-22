#include "Utils.h"

GLint GetVarLoc(GLuint program, std::string varName) {
	GLint loc = glGetUniformLocation(program, varName.c_str());
	if (loc == -1) {
		printf("Variable %s in shader not found\n", varName.c_str());
	}
	return loc;
}

void SetUniformInt(GLuint Program, std::string varName, GLint val) {
	GLint loc;
	if ((loc = GetVarLoc(Program, varName)) != -1)
		glUniform1i(loc, val);
}

void SetUniformFloat(GLuint Program, std::string varName, GLfloat val) {
	GLint loc;
	if ((loc = GetVarLoc(Program, varName)) != -1)
		glUniform1f(loc, val);
}

void SetUniformVec2f(GLuint Program, std::string varName, GLfloat val0, GLfloat val1) {
	GLint loc;
	if ((loc = GetVarLoc(Program, varName)) != -1)
		glUniform2f(loc, val0, val1);
}

void SetUniformVec2f(GLuint Program, std::string varName, vec2 val) {
	SetUniformVec2f(Program, varName, val.x, val.y);
}

void SetUniformVec3f(GLuint Program, std::string varName, GLfloat val0, GLfloat val1, GLfloat val2) {
	GLint loc;
	if ((loc = GetVarLoc(Program, varName)) != -1)
		glUniform3f(loc, val0, val1, val2);
}

void SetUniformVec3f(GLuint Program, std::string varName, vec3 val) {
	SetUniformVec3f(Program, varName, val.x, val.y, val.z);
}

void SetUniformMat3f(GLuint Program, std::string varName, mat3 val) {
	GLint loc;
	if ((loc = GetVarLoc(Program, varName)) != -1)
		glUniformMatrix3fv(loc, 1, GL_FALSE, &(val[0].x));
}

void SetUniformMat4f(GLuint Program, std::string varName, mat4 val) {
	GLint loc;
	if ((loc = GetVarLoc(Program, varName)) != -1)
		glUniformMatrix4fv(loc, 1, GL_FALSE, &(val[0].x));
}

void SetUniformVec4f(GLuint Program, std::string varName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
	GLint loc;
	if ((loc = GetVarLoc(Program, varName)) != -1)
		glUniform4f(loc, v0, v1, v2, v3);
}

void SetUniformVec4f(GLuint Program, std::string varName, vec4 val) {
	SetUniformVec4f(Program, varName, val.x, val.y, val.z, val.w);
}