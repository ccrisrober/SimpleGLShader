#include "SimpleGLShader.h"
#include <fstream>
#include <iostream>

SimpleGLShader::SimpleGLShader(void) : mTotalShaders(0) {
	mAttrsList.clear();
	mUniformList.clear();
}

SimpleGLShader::~SimpleGLShader(void) {
	mAttrsList.clear();
	mUniformList.clear();
	for(int i = 0; i < MAX_SHADERS; i++) {
		if(mShaders[i] != 0) {
			glDetachShader(mProgram, mShaders[i]);
		}
	}
	glDeleteProgram(mProgram);
}

void SimpleGLShader::load_from_source(const std::string& source, GLenum shader_type) {
	GLuint shader = glCreateShader(shader_type);

	const char *code = source.c_str();
	glShaderSource(shader, 1, &code, (const GLint*)source.length());
}

void SimpleGLShader::load_from_file(const std::string& file, GLenum shader_type) {
	std::ifstream ifs;
	ifs.open(file.c_str(), std::ios_base::in);

	if(!ifs.good()) throw;

	std::string line, bff = "";
	while(std::getline(ifs, line)) {
		bff += line + "\r\n";
	}

	load_from_source(bff, shader_type);
}

void SimpleGLShader::create_and_link() {
	mProgram = glCreateProgram();
	for(int i = 0; i < MAX_SHADERS; i++) {
		if(mShaders[i] != 0) {
			glAttachShader(mProgram, mShaders[i]);
		}
	}

	// Link and check status
	GLint compiled_status;
	for(int i = 0; i < MAX_SHADERS; i++) {
		if(mShaders[i] != 0) {
			glGetShaderiv(mShaders[i], GL_COMPILE_STATUS, & compiled_status);
			if(compiled_status == GL_FALSE) {
				// Get errors
				GLint logLen;
				glGetShaderiv(mShaders[i], GL_INFO_LOG_LENGTH, &logLen);

				char *logStr = new char[logLen];
				glGetShaderInfoLog(mShaders[i], logLen, NULL, logStr);
				std::cerr << "Error: " << logStr << std::endl;
				delete logStr;

				glDeleteShader(mShaders[i]);
				throw;
			}
		}
	}
}

void SimpleGLShader::use() {
	glUseProgram(mProgram);
}

void SimpleGLShader::unuse() {
	glUseProgram(NULL);
}


void SimpleGLShader::add_attribute(const std::string& attr) {
	mAttrsList[attr] = glGetAttribLocation(mProgram, attr.c_str());
}

void SimpleGLShader::add_uniform(const std::string& uniform) {
	mUniformList[uniform] = glGetUniformLocation(mProgram, uniform.c_str());
}

GLuint SimpleGLShader::attribute(const std::string& attr) {
	return mAttrsList[attr];
}

GLuint SimpleGLShader::uniform(const std::string& uniform) {
	return mUniformList[uniform];
}