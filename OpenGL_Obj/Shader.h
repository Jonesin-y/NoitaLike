#pragma once
#include<string>
#include<glad/glad.h>
class Shader
{
public:
	Shader(const std::string& vs_file, const std::string& fs_file, const std::string& name);
	std::string LoadFromFile(const std::string& filePath);
	unsigned int CompileShader(GLenum type,const std::string& src);
	unsigned int CreateShader(const std::string& vs_src, const std::string& fs_src);
	void SetUniform1i(const std::string& name,int value);
	void SetUniformMat4f(const std::string& name, unsigned int count, unsigned char transpose, const float* value);

	void Bind();
	void UnBind();
private:
	unsigned int m_RendererID;
	std::string m_Name;
	
};

