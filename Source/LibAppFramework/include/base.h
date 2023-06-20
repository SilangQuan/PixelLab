#pragma once

#include "../../SkyEngine/include/SkyEngine.h"
#include "Input.h"
#include "InputDefinition.h"
#include "STime.h"

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment( lib, "..\\..\\Build\\lib\\Debug\\SkyEngine.lib" )
#else
#pragma comment( lib, "..\\..\\Build\\lib\\Release\\SkyEngine.lib" )
#endif

#else

#ifdef _DEBUG
#pragma comment( lib, "..\\..\\Build\\lib\\Debug\\SkyEngine.lib" )
#else
#pragma comment( lib, "..\\..\\Build\\lib\\Release\\SkyEngine.lib" )
#endif

#endif
//

#if _MSC_VER > 1000
#pragma warning( disable : 4786 ) // shut up compiler warning
#endif




