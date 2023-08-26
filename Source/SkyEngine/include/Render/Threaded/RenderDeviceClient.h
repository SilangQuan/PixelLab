#pragma once
#include "Render/RenderDevice.h"

class RenderDeviceWorker;
class RingBuffer;

class RenderDeviceClient 
{
public:
        RenderDeviceClient(GraphicsAPI api, RenderThreadMode threadmode, SdlWindow* mainwindow);
        ~RenderDeviceClient();
        RingBuffer* GetCommandQueue() const { return m_CommandQueue; }
        RenderDeviceWorker* GetGfxDeviceWorker() const { return m_DeviceWorker; }
        void SetGfxDeviceWorker(RenderDeviceWorker* worker) { m_DeviceWorker = worker; }
        void SetRealDevice(RenderDevice* realDevice) { m_RealDevice = realDevice; };

		virtual void SetClearColor(float r, float g, float b, float alpha);
		virtual void DrawTriangle(std::vector<Vector3>& vertices);
		virtual void SetViewPort(int x, int y, int width, int height);

		virtual void BeginFrame();
		virtual void Present();
	
private:
    bool                m_Threaded;
	bool                m_PresentPending;

    RenderDeviceWorker* m_DeviceWorker;
    RenderDevice* m_RealDevice;
    RingBuffer* m_CommandQueue;
};