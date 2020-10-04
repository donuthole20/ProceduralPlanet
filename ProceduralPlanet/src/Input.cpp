#include "Input.h"
#include "InputCode.h"

void Input::RegisterKeyInputCallback(InputListener* listener)
{
	keyInputCallbacks.push_back(listener);
}
void Input::RegisterMousePositionInputCallback(InputListener* listener)
{
	mousePositionCallbacks.push_back(listener);
}



void Input::UnRegisterCallback(uint32_t ID)
{
	//TODO: change vector callbacks to another data type that can remove and delete
}



void Input::ProcessKeyInput(int key, int action)
{
	if (action == PRESS)
	{
		keyPressedCount++;
	}
	else if(action == RELEASE)
	{
		keyPressedCount--;
	}

	for (int i = 0; i < keyInputCallbacks.size(); i++)
	{
		keyInputCallbacks[i]->HandleKeyInput(key, action);
	}
}

void Input::ProcessCursorPosition(float xpos, float ypos)
{
	for (int i = 0; i < mousePositionCallbacks.size(); i++)
	{
		mousePositionCallbacks[i]->HandleMousePositionInput(xpos, ypos);
	}
}

void Input::UpdateKeyInput(float deltaTime)
{
	if (keyPressedCount < 1)
	{
		//return;
	}

	for (size_t i = 0; i < keyInputCallbacks.size(); i++)
	{
		keyInputCallbacks[i]->HandleKeyInputUpdate(deltaTime);
	}
}

