#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>


enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

void WriteShader(std::vector<std::stringstream*>* shadersStream, std::vector<std::string>* filenames, ShaderType type)
{
	
	for (uint32_t i = 0; i < shadersStream->size(); i++)
	{
		//vertexShaders[i]
		std::ofstream writer;
		//"SerializedRes/shaders/"
		std::string path = (*filenames)[i].substr(0, (*filenames)[i].find("."));

		std::string postFix;
		switch (type)
		{
		case ShaderType::VERTEX:
			path = "res/serialized/shaders/" + path + "_vertex.bin";
			break;
		case ShaderType::FRAGMENT:
			path = "res/serialized/shaders/" + path + "_fragment.bin";
			break;
		}

		std::cout << "Writing Shader: " << path << "\n";

		writer.open(path, std::ofstream::binary | std::ofstream::out);
		if (!writer.is_open())
		{
			std::cout << "Cannot open shader: " << path << "\n";
			break;
		}
		size_t shaderSize = (*shadersStream)[i]->seekg(0, std::stringstream::end).tellg();
		//writer.write((char*)&shaderSize, sizeof(shaderSize));
		writer.write((*shadersStream)[i]->str().c_str(), shaderSize);
		writer.close();
	}
	for (uint32_t i = 0; i < shadersStream->size(); i++)
	{
		delete (*shadersStream)[i];
	}
}

int main(void)
{
	std::vector<std::string> unserializedShaderFiles;
	std::vector<std::string> vertexShadersFileNames;
	std::vector<std::string> fragmentShadersFileNames;
	std::vector<std::stringstream*> vertexShaders;
	std::vector<std::stringstream*> fragmentShaders;


	const char* unserializedShaderDir = "res/unserialized/shaders";
	for (auto& shader : std::filesystem::directory_iterator(unserializedShaderDir))
	{
		std::cout <<"Reading shader:" << shader.path() << '\n';
		std::ifstream stream(shader.path());
		if (stream.fail())
		{
			std::cout << "Cannot open shader: " << shader.path() <<"\n";
			break;
		}

		std::string line;
		ShaderType parseMode = ShaderType::NONE;
		while (std::getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
				{
					parseMode = ShaderType::VERTEX;
					vertexShaders.push_back(new std::stringstream());
					vertexShadersFileNames.emplace_back(shader.path().filename().string());
				}
				else if (line.find("fragment") != std::string::npos)
				{
					parseMode = ShaderType::FRAGMENT;
					fragmentShaders.push_back(new std::stringstream());
					fragmentShadersFileNames.emplace_back(shader.path().filename().string());
				}
			}
			else
			{
				switch(parseMode)
				{
				case ShaderType::VERTEX:
					std::cout << "[Vertex]Streaming line: " + line << "\n";
					(*vertexShaders.back()) <<  line << "\n";;
					break;
				case ShaderType::FRAGMENT:
					std::cout << "[Fragment]Streaming line: " + line << "\n";
					(*fragmentShaders.back()) << line << "\n";;
					break;
				}
			}
		}
		stream.close();
	}

	std::filesystem::create_directories("res/serialized/shaders");
	
	//Serialize Fragment
	WriteShader(&vertexShaders, &vertexShadersFileNames, ShaderType::VERTEX);
	WriteShader(&fragmentShaders, &fragmentShadersFileNames, ShaderType::FRAGMENT);

	/*std::cout << "Exit?";
	std::cin.get();*/

	/*std::filesystem::create_directories("SerializedRes/Test/");
	const char* tesy = "tesy";
	std::ofstream writer;
	writer.open("SerializedRes/Test/Tesy.bin", std::ios::binary | std::ios::out);
	if (!writer.is_open())
	{
		std::cout << "Error Writing!\n";
	}

	writer.write(tesy, sizeof(tesy));
	writer.close();
	std::cout << "Finished Writing... Read?";
	std::cin.get();*/

	/*	char tesyResult[sizeof(tesy)];

		if (!reader.is_open())
		{
			std::cout << "Error Reading!\n";
		}
		reader.read(tesyResult,sizeof(tesy));
		std::cout <<"Result is: "<< tesyResult << "\n";*/

	/*const char* serializedShaderDir = "res/serialized/shaders";
	for (auto& shader : std::filesystem::directory_iterator(serializedShaderDir))
	{
		if (!shader.exists())
		{
			std::cout << "Cannot open shader:" << shader.path() << "\n";
		}
		std::ifstream reader;
		reader.open(shader.path().c_str(), std::ios::binary | std::ios::in);
		size_t filesize = shader.file_size();
		char* shaderText = new char [filesize];
		reader.read(shaderText, filesize);
		reader.close();
		delete shaderText;
	
	}*/


	return 0;
}


