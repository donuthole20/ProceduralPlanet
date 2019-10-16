#include "Input.h"
#include "InputCode.h"

void Input::registerKeyInputCallback(InputListener* listener)
{
	keyInputCallbacks.push_back(listener);
}
void Input::registerMousePositionInputCallback(InputListener* listener)
{
	mousePositionCallbacks.push_back(listener);
}



void Input::unRegisterCallback(unsigned int ID)
{
	//TODO: change vector callbacks to another data type that can remove and delete
}



void Input::processKeyInput(int key, int action)
{

	for (int i = 0; i < keyInputCallbacks.size(); i++)
	{
		keyInputCallbacks[i]->handleKeyInput(key, action);
	}
}

void Input::processCursorPosition(float xpos, float ypos)
{
	for (int i = 0; i < mousePositionCallbacks.size(); i++)
	{
		mousePositionCallbacks[i]->handleMousePositionInput(xpos, ypos);
	}
}

