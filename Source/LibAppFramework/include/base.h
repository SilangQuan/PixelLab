#pragma once

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment( lib, "..\\..\\Lib\\x64\\Debug\\SkyEngine.lib" )
#else
#pragma comment( lib, "..\\..\\Lib\\x64\\Release\\SkyEngine.lib" )
#endif

#else

#ifdef _DEBUG
#pragma comment( lib, "..\\..\\Lib\\Win32\\Debug\\SkyEngine.lib" )
#else
#pragma comment( lib, "..\\..\\Lib\\Win32\\Release\\SkyEngine.lib" )
#endif

#endif
//

#if _MSC_VER > 1000
#pragma warning( disable : 4786 ) // shut up compiler warning
#endif


// Basic variable type-definitions --------------------------------------------

#include <string>

#include "../../SkyEngine/include/SkyEngine.h"
#include "Input.h"
#include "InputDefinition.h"
#include "Time.h"


