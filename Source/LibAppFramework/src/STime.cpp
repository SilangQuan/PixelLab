#include "../include/STime.h"
#include "../../3rdParty/include/SDL/SDL.h"
float STime::deltaTime = 0;
float STime::time = 0;
float STime::fps = 0;

float inv1000 = 1.0f / 1000;
STime::STime()
{
	frameCountInOneSecond = 0;
}

STime::~STime()
{
	
}

void STime::Start()
{
	startMark = SDL_GetTicks() * inv1000;
	lastFpsMark = startMark;
	lastUpdateMark = SDL_GetTicks()* inv1000;
}

void STime::Stop()
{
	
}

void STime::Restart()
{
	
}

void STime::Update()
{
	lastUpdateMark = time;
	time = SDL_GetTicks() * inv1000 - startMark;
	deltaTime = time - lastUpdateMark;
	frameCountInOneSecond++;
	if (time - lastFpsMark > 1)
	{
		fps = frameCountInOneSecond;
		frameCountInOneSecond = 0;
		lastFpsMark = time;
	}
	//qDebug() <<"deltaTime" << deltaTime;
	//fps = 1.0 / deltaTime;
}