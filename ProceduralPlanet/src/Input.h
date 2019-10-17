#pragma once
#include <vector>
#include <functional>

class InputListener
{
public:
	virtual void handleKeyInput(int key, int action) = 0;
	virtual void handleMousePositionInput(float xChange, float yChange) = 0;
	virtual void handleKeyInputUpdate(float deltaTime) =0;
};



class Input
{
private:
	std::vector<InputListener*> keyInputCallbacks;
	std::vector<InputListener*> mousePositionCallbacks;
	int keyPressedCount;
public:
	void registerKeyInputCallback(InputListener* listener);
	void registerMousePositionInputCallback(InputListener* listener);
	void unRegisterCallback(unsigned int ID);
	void processKeyInput(int key, int action);
	void processCursorPosition(float xpos, float ypos);
	void updateKeyInput(float deltaTime);
};

