#include "pch.h"
#include "Shader.h"

Shader::Shader(const std::string& vs_file, const std::string& fs_file, const std::string& name)
	:m_Name(name)
{
	std::string vs_src = LoadFromFile(vs_file);
	std::string fs_src = LoadFromFile(fs_file);
	m_RendererID = CreateShader(vs_src,fs_src);
}

std::string Shader::LoadFromFile(const std::string& filePath)
{
	std::ifstream fstream(filePath);
	if (!fstream.is_open())
	{
		printf("文件打开失败!\n");
		return "";
	}
	std::string line;
	std::stringstream sstream;
	while (getline(fstream, line))
	{
		sstream << line << "\n";
	}
	std::string src = sstream.str();
	return src;
}

unsigned int Shader::CompileShader(GLenum type, const std::string& src)
{
	unsigned int id = glCreateShader(type);
	const char* c_src = src.c_str();
	glShaderSource(id,1,&c_src,NULL);
	glCompileShader(id);
	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_alloca(length);
		glGetShaderInfoLog(id, length, &length,message);
		std::cout << ((type == GL_VERTEX_SHADER) ? "顶点着色器" : "片段着色器") << "编译错误 :" <<message<< std::endl;
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vs_src, const std::string& fs_src)
{
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vs_src);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fs_src);
	unsigned int id = glCreateProgram();
	glAttachShader(id,vs);
	glAttachShader(id, fs);
	glLinkProgram(id);
	glValidateProgram(id);
	GLint result;
	glGetProgramiv(id,GL_LINK_STATUS,&result);
	if (result == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_alloca(length);
		glGetProgramInfoLog(id,length,&length,message);
		std::cout << "着色器:" << m_Name << " ,链接错误 :" << message << std::endl;
		return 0;
	}
	glDeleteShader(vs);
	glDeleteShader(fs);
	return id;
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLint location = glGetUniformLocation(m_RendererID,name.c_str());
	glProgramUniform1i(m_RendererID, location, value);

}

void Shader::SetUniformMat4f(const std::string& name, unsigned int count, unsigned char transpose, const float* value)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glProgramUniformMatrix4fv(m_RendererID, location, count, transpose, value);
}

void Shader::Bind()
{
	glUseProgram(m_RendererID);
}

void Shader::UnBind()
{
	glUseProgram(0);

}
