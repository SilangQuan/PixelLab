#pragma once

class STime
{
public:
	static float deltaTime;
	static float time;
	static float fps;

	STime();
	~STime();
	void Start();
	void Stop();
	void Restart();
	void Update();

private:
	float startMark;
	float lastUpdateMark;
	int frameCountInOneSecond;
	float lastFpsMark;
};