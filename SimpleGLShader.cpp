#include "SimpleGLShader.h"
#include <fstream>
#include <iostream>

void SimpleGLShader::load(const std::string& fileName, GLenum type) {
	unsigned int fileLen;

	// Load file
	std::ifstream file;
	file.open(fileName, std::ios::in);
	if (!file) exit(-1);

	// File length
	file.seekg(0, std::ios::end);
	fileLen = unsigned int(file.tellg());
	file.seekg(std::ios::beg);

	// Read the file
	char *source = new char[fileLen + 1];

	int i = 0;
	while (file.good()) {
		source[i] = char(file.get());
		if (!file.eof()) i++;
		else fileLen = i;
	}
	source[fileLen] = '\0';
	file.close();

	// Create and compile shader
	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar **) &source, (const GLint *) &fileLen);

	GLint status;
	glCompileShader(shader);
	glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;		
		glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog= new GLchar[infoLogLength];
		glGetShaderInfoLog (shader, infoLogLength, NULL, infoLog);
		std::cerr << "Compile log ("<< fileName << "): " << infoLog << std::endl;
		delete [] infoLog;
		return;
	}
	delete source;

	// Add to shaders in use
	mShaders.push_back(shader);
}

SimpleGLShader::SimpleGLShader(void) : mTotalShaders(0) {
	mAttrsList.clear();
	mUniformList.clear();
}

SimpleGLShader::~SimpleGLShader(void) {
	mAttrsList.clear();
	mUniformList.clear();
	/*
	GLuint size = mShaders.size();
	for(int i = 0; i < size; i++) {
		if(mShaders[i] != 0) {
			glDetachShader(mProgram, mShaders[i]);
		}
	}
	glDeleteProgram(mProgram);
	*/
}

void SimpleGLShader::destroy() {
	mAttrsList.clear();
	mUniformList.clear();
	GLuint size = mShaders.size();
	for(int i = 0; i < size; i++) {
		if(mShaders[i] != 0) {
			glDetachShader(mProgram, mShaders[i]);
		}
	}
	glDeleteProgram(mProgram);
}

void SimpleGLShader::create() {
	mProgram = glCreateProgram();
	GLuint size = mShaders.size();
	for(int i = 0; i < size; i++) {
		if(mShaders[i] != 0) {
			glAttachShader(mProgram, mShaders[i]);
		}
	}
}

void SimpleGLShader::link() {
	// link and check whether the program links fine
	GLint status;
	glLinkProgram (mProgram);
	glGetProgramiv (mProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		
		glGetProgramiv (mProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog (mProgram, infoLogLength, NULL, infoLog);
		std::cerr << "Link log: " << infoLog << std::endl;
		delete [] infoLog;
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

void SimpleGLShader::bind_attribute(const std::string& attr, GLuint index) {
	glBindAttribLocation(mProgram, index, attr.c_str());
}

GLuint SimpleGLShader::attribute(const std::string& attr) {
	return mAttrsList[attr];
}

GLuint SimpleGLShader::uniform(const std::string& uniform) {
	return mUniformList[uniform];
}