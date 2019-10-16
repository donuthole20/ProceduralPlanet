#pragma once
class PyramidTest
{
private:
	unsigned int vertexBufferID;
	unsigned int indexBufferID;
	unsigned int vertexArrayObjectID;
public:
	~PyramidTest();
	PyramidTest();
	void Draw();
	void Delete();
};
