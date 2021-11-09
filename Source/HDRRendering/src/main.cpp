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
	creationFlags.title = "HDR Rendering";
	creationFlags.isWindowed = true;
	creationFlags.renderPath = RenderingPath::Forward;
	qDebug() << "Begin App";
	App triangleApp;

	if (!triangleApp.Initialize(creationFlags))
	{
		return 1;
	}

	qDebug() << "Initialized";

	return triangleApp.Run();
	
}
