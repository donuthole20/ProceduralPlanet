#pragma once
#include <vector>

#include "externallibs/imgui/imgui_color_gradient.h"

struct PlanetGradient
{
	ImGradient gradient;
	ImGradientMark* draggingMark = nullptr;
	ImGradientMark* selectedMark = nullptr;
	PlanetGradient()
	{
		ImGradientMark* firstMark = gradient.getMarks().back();
		ImGradientMark* secondMark = gradient.getMarks().front();

		firstMark->color[0] = std::max(0.0f, static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.2f);
		firstMark->color[1] = std::max(0.0f, static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.2f);
		firstMark->color[2] = std::max(0.0f, static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.2f);
		firstMark->color[3] = 1.0f;

		secondMark->color[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		secondMark->color[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		secondMark->color[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		secondMark->color[3] = 1.0f;

		gradient.refreshCache();
	}
	void Randomize()
	{
		for (ImGradientMark* mark : gradient.getMarks())
		{
			mark->color[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			mark->color[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			mark->color[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			mark->color[3] = 1.0f;
		}
		gradient.refreshCache();
	}
	//TODO: Make this random
};


class PlanetTexture
{
public:
	PlanetGradient* waterGradient;
	std::vector<PlanetGradient*> biomes;

	PlanetTexture();
	~PlanetTexture();
	void Randomize();
};

