#include <iostream>
#include "../include/App.h"

int main(int argc, char *argv[])
{
	CreationFlags creationFlags;
	int32 width = 640;
	int32 height = 360;

	//int32 width = 1280;
	//int32 height = 720;


	creationFlags.width = width;
	creationFlags.height = height;
	creationFlags.title = "Image Processing Compute";
	creationFlags.isWindowed = true;
	creationFlags.renderPass = RenderingPath::Forward;
	qDebug() << "Begin App";
	App triangleApp;

	if (!triangleApp.Initialize(creationFlags))
	{
		return 1;
	}

	qDebug() << "Initialized";

	return triangleApp.Run();
	
}
