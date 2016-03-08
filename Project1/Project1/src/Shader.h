#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h> // get OpenGL header

using namespace std;
class Shader {
public:
	// The program ID;
	GLint Program;
	bool ok = true;
	// Constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;
		// activate exceptions
		vShaderFile.exceptions(ifstream::badbit);
		fShaderFile.exceptions(ifstream::badbit);
		try
		{
			//Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			stringstream vShaderStream, fShaderStream;
			// read file text into stream
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (const ifstream::failure e)
		{
			cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
			cout << vertexPath << endl;
			ok = false;
			//should quit program?
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		// compile vertex shader
		GLuint vertexShader, fragmentShader;
		GLint success;
		GLchar infoLog[512];
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		// Check for compile time errors	
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			ok = false;
			//should quit program?
		}

		// compile fragment shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		// Check for compile time errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			cout << fragmentPath << endl;
			ok = false;
			//should quit program?
		}

		// Link shaders
		this->Program = glCreateProgram();
		// link 1
		glAttachShader(this->Program, vertexShader);
		glAttachShader(this->Program, fragmentShader);
		glLinkProgram(this->Program);
		// Check for linking errors
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::PROGRAM::LINK::FAILED\n" << infoLog << std::endl;
			ok = false;
		}
		// Delete shaders since they are linked and no longer needed
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	// Use the program
	void Shader::Use() {
		glUseProgram(this->Program);
	}
};

#endif