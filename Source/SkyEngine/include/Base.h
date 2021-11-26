#pragma once
#include <float.h>
#pragma warning( disable: 4996 ) // Disable warning about _controlfp being deprecated.

#ifdef _WIN64


#pragma comment( lib, "..\\3rdParty\\lib\\assimp\\lib64\\assimp-vc142-mt.lib" )
#pragma comment( lib, "..\\3rdParty\\lib\\GLEW\\x64\\glew32.lib" )
#pragma comment( lib, "..\\3rdParty\\lib\\SDL\\x64\\SDL2.lib" )
#pragma comment( lib, "..\\3rdParty\\lib\\SDL\\x64\\SDL2main.lib" )
#pragma comment( lib, "..\\3rdParty\\lib\\SDL\\x64\\SDL2_image.lib" )
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#else


#pragma comment( lib, "..\\3rdParty\\lib\\assimp\\lib32\\assimp-vc142-mt.lib" )
#pragma comment( lib, "..\\3rdParty\\lib\\GLEW\\Win32\\glew32.lib" )
#pragma comment( lib, "..\\3rdParty\\lib\\SDL\\x86\\SDL2.lib" )
#pragma comment( lib, "..\\3rdParty\\lib\\SDL\\x86\\SDL2main.lib" )
#pragma comment( lib, "..\\3rdParty\\lib\\SDL\\x86\\SDL2_image.lib" )
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#endif




#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"

#include <string>
#include "Math/Mathf.h"
#include "Log/Debug.h"
#include "Log/Log.h"
#include "Core/ResourceManager.h"
#include "Components/Transform.h"
#include <map>
#include <set>
#include <optional>
#include <vector>


#include <nlohmann/json.hpp>
using json = nlohmann::json;


// Basic macro definitions ----------------------------------------------------

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define SAFE_DELETE( p ) { if( p ) { delete p; p = 0; }  }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] p; p = 0; }  }
#define SAFE_RELEASE( p ) { if( p ) { ( p )->Release(); p = 0; }  }

#define ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Size of a static C-style array. Don't use on pointers!



inline void fpuTruncate() { _controlfp(_RC_DOWN + _PC_24, _MCW_RC | _MCW_PC); }	///< Sets FPU to truncation-mode and single precision.
inline void fpuReset() { _controlfp(_CW_DEFAULT, _MCW_RC | _MCW_PC); }		///< Resets FPU to default.

typedef signed char         int8;		///< 8-bit signed integer
typedef signed short        int16;		///< 16-bit signed integer
typedef signed int          int32;		///< 32-bit signed integer
typedef unsigned char       uint8;		///< 8-bit unsigned integer
typedef unsigned short      uint16;		///< 16-bit unsigned integer
typedef unsigned int        uint32;		///< 32-bit unsigned integer

typedef float				float32;	///< 32-bit float
typedef double				float64;	///< 64-bit float
typedef unsigned char		byte;		///< byte



enum result
{
	ok = 0,				///< No errors were encountered, function returned successfully.
	unknown,				///< An unknown error has occured.
	invalidparameters,	///< One or more parameters were invalid.
	outofmemory,			///< Allocation of memory within the function failed.

	invalidformat,		///< A format is invalid for a specific task.
	invalidstate,			///< An invalid state has been found and the function cannot continue.
	shadercompileerror
};

enum RenderingPath
{
	Forward,
	Deferred,
	ForwardPlus,
};

enum GraphicsAPI
{
	OpenGL,
	Vulkan,
	DX12
};

enum DepthType {
	NoDepth,
	Depth16,
	Depth24S8,
	Depth32
};
enum ColorType {
	RGB565,
	RGB888,
	RGBA8888,
	RGB16F,
	RGBA16F,
	R11G11B10,
};

enum EModelType
{
	OBJ,
	FBX,
	GLTF
};

enum EFillMode
{
	FM_Point,
	FM_Wireframe,
	FM_Solid,

	EFillMode_Num,
};

enum ECullMode
{
	CM_None,
	CM_Front,
	CM_Back,

	ECullMode_Num,
};

enum EZTestMode
{
	TM_ALWAYS,
	TM_NEVER,
	TM_LESS,
	TM_EQUAL,
	TM_LEQUAL,
	TM_GREATER,
	TM_NOTEQUAL,
	TM_GEQUAL,
	EZTestMode_Num,
};


enum EZWriteMode
{
	WM_ON,
	WM_OFF,
	EZWriteMode_Num,
};

enum ETextureVariableType
{
	TV_2D,
	TV_3D,
	TV_CUBE
};

struct ViewInfo
{
public:
	Matrix4x4 View;
	Matrix4x4 Projection;
	Matrix4x4 InvProject;
	Matrix4x4 ViewPorject;
	Vector4 ScreenSizeAndInv;
};

class Base
{
protected:
	inline Base() : refCount(1) {}	///< The constructor initializes the reference count to 1.
	virtual inline ~Base() {}

public:
	inline void AddRef() { ++refCount; }	///< AddRef() increases the reference count.
	inline void Release() { if (--refCount == 0) delete this; }	///< Release() decreases the reference count and calls the destructor when it is 0.

private:
	Base(const Base &) {}								///< Private copy-operator to avoid object copying.
	Base &operator =(const Base &) { return *this; }	///< Private assignment-operator to avoid object copying. Returns a value to avoid compiler warnings.

private:
	uint32 refCount;
};


static void PushGroupMarker(const char* Name)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, strlen(Name), Name);
}

static void PopGroupMarker()
{
	glPopDebugGroup();
}
