#pragma once
#include "Render/RenderDevice.h"
#include <thread>
#include "RingBuffer.h"
#include "PlatformSemaphore.h"

enum WaitType
{
	WaitType_Common,
	WaitType_OnwerShip,
	WaitType_Present,
	WaitType_CreateShader,
	WaitType_CreateVBO,
	WaitType_CreateTexture,

	WaitType_Max
};

enum GfxCommandType
{
	kGfxCmd_Unused = 10000,

	kGfxCmd_Clear,
	kGfxCmd_UseGPUProgram,
	kGfxCmd_CreateGPUProgram,
	kGfxCmd_DeleteGPUProgram,
	kGfxCmd_CreateTexture2D,
	kGfxCmd_DeleteTexture2D,
	kGfxCmd_UseTexture2D,
	kGfxCmd_SetClearColor,
	kGfxCmd_DrawTriangle,
	kGfxCmd_SetViewport,
	kGfxCmd_BeginFrame,
	kGfxCmd_Present,
	kGfxCmd_AcqiureThreadOwnerShip,
	kGfxCmd_ReleaseThreadOwnership,
	kGfxCmd_CreateVBO,
	kGfxCmd_UpdateVBO,
	kGfxCmd_DeleteVBO,
	kGfxCmd_DrawVBO,
	kGfxCmd_SetGPUProgramAsInt,
	kGfxCmd_SetGPUProgramAsFloat,
	kGfxCmd_SetGPUProgramAsMat4,
	kGfxCmd_SetGPUProgramAsIntArray,
	kGfxCmd_SetGPUProgramAsFloatArray,
	kGfxCmd_SetGPUProgramAsMat4Array,
	kGfxCmd_InitThreadGPUProgramParam,

	kGfxCmd_Count
};


class RenderDeviceWorker
{

private:
	std::thread _thread;
	bool _quit;
	Semaphore _waitSem[WaitType_Max];

	RingBuffer* _commandBuffer;
	bool  _threaded;
	bool _isInPresenting;
	bool _returnResImmediately;	//是否立即返回资源创建
	RenderDevice* _realDevice;
	void RunOneThreadCommand();


	static void* _Run(void* data)
	{
		qDebug() << "[render] _Run(void* data)";
		RenderDeviceWorker* self = (RenderDeviceWorker*)data;
		self->_RunCommand();
		qDebug() << "[render] _Run(void* data) end";
		return 0;
	}
	virtual void _RunCommand()
	{
		qDebug() << "[render] __RunCommand()";
		_threaded = true;
		_realDevice->AcqiureThreadOwnerShip();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		Signal();
		while (!_quit) {
			//usleep(10);
			if (_threaded) {
				RunOneThreadCommand();
			}
		}
		qDebug() << "[render] __RunCommand() end";
	}

public:
	/*
	RenderDeviceWorker(RenderDevice* context, bool returnResImmediately)
		:_returnResImmediately(returnResImmediately)
		, _threaded(false)
		, _quit(false)
		, _isInPresenting(false)
	{
		qDebug() << "create [render] ThreadESDevice";

		_commandBuffer = new RingBuffer(BUFFER_SIZE);
	}*/

	RenderDeviceWorker(RingBuffer* ringbuffer)
		: _threaded(false)
		, _quit(false)
		, _isInPresenting(false)
	{
		qDebug() << "create [render] RenderDeviceWorker";
		_commandBuffer = ringbuffer;
	}
	~RenderDeviceWorker()
	{
		SAFE_DELETE(_commandBuffer);
	}


	void Startup()
	{
		//m_WorkerThread->SetTempAllocatorSize(256 * 1024);
		//m_Device->ReleaseThreadOwnership();
		//_thread = new thread(&RenderDeviceWorker::Run);
		//_thread->join();
		


		/* 
		//_thread->SetName("UnityGfxDeviceWorker");
		m_WorkerThread->SetTempAllocatorSize(256 * 1024);
		//m_Device->ReleaseThreadOwnership();

#if !(PLATFORM_WINRT || UNITY_APPLE)
		// this has to be done after the thread is run, otherwise the change is ignored
		m_WorkerThread->SetPriority(GetDeviceWorkerPriority());
#endif*/
	}

	RenderDevice* GetRealDevice() { return _realDevice; }
	void    SetRealDevice(RenderDevice* device) { _realDevice = device; }



	/*virtual bool CreateWindow1(const std::string& title, int width, int height, int flags)
	{
		return _realDevice->CreateWindow(title, width, height, flags);
	}*/
	virtual void Cleanup()
	{
		_quit = true;
		_thread.join();
		delete _realDevice;
	}
	virtual int GetScreenWidth() { return _realDevice->GetScreenWidth(); }
	virtual int GetScreenHeigt() { return _realDevice->GetScreenHeigt(); }

	//virtual GPUProgramParam* GetGPUProgramParam(GPUProgram* program, const std::string& name);

	//virtual void InitThreadGPUProgramParam(ThreadedGPUProgram* program, ThreadedGPUProgramParam* param, const std::string& name) = 0;
	bool IsCreateResInBlockMode()const
	{
		return _returnResImmediately;
	}
	void WaitForSignal(WaitType waitType = WaitType_Common) {
		_waitSem[waitType].WaitForSignal();
	}
	void Signal(WaitType waitType = WaitType_Common) {
		_waitSem[waitType].Signal();
	}
	void WaitForPresent()
	{
		WaitForSignal(WaitType_Present);
	}
	void SignalPresent()
	{
		Signal(WaitType_Present);
		_isInPresenting = false;
	}
	void WaitForOwnerShip()
	{
		WaitForSignal(WaitType_OnwerShip);
	}
	void SignalOnwerShip()
	{
		Signal(WaitType_OnwerShip);
	}

	void Run()
	{
		qDebug()<<"[render] ThreadESDevice::Run()";
		_thread = std::thread(&RenderDeviceWorker::_Run, (void*)this);
		//_realDevice->AcqiureThreadOwnerShip();
		this->WaitForSignal();
	}
};
