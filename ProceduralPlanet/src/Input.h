#pragma once
#include <vector>
#include <functional>

class InputListener
{
public:
	virtual void HandleKeyInput(int key, int action) = 0;
	virtual void HandleMousePositionInput(float xChange, float yChange) = 0;
	virtual void HandleKeyInputUpdate(float deltaTime) =0;
};



class Input
{
private:
	std::vector<InputListener*> keyInputCallbacks;
	std::vector<InputListener*> mousePositionCallbacks;
	int keyPressedCount;
public:
	void RegisterKeyInputCallback(InputListener* listener);
	void RegisterMousePositionInputCallback(InputListener* listener);
	void UnRegisterCallback(unsigned int ID);
	void ProcessKeyInput(int key, int action);
	void ProcessCursorPosition(float xpos, float ypos);
	void UpdateKeyInput(float deltaTime);
};

