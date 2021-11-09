#include <iostream>
#include "../include/App.h"

int main(int argc, char *argv[])
{

	CreationFlags creationFlags;
	int32 width = 640;
	int32 height = 360;
	creationFlags.width = width;
	creationFlags.height = height;
	creationFlags.title = "Forward Plus";
	creationFlags.isWindowed = true;
	App triangleApp;

	if (!triangleApp.Initialize(creationFlags))
	{
		return 1;
	}

	qDebug() << "Initialized";

	return triangleApp.Run();
	
}