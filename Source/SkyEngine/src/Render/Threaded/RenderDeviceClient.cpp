#include "Render/Threaded/RenderDeviceClient.h"
#include "Render/Threaded/RingBuffer.h"
#include "Render/Threaded/RenderDeviceWorker.h"

RenderDeviceClient::RenderDeviceClient(GraphicsAPI api, RenderThreadMode threadmode, SdlWindow* mainwindow)
{
    m_Threaded = threadmode == RenderThreadMode::ThreadModeThreaded ? true : false;
    m_RealDevice = CreateClientDevice(api, threadmode, mainwindow);

    if (m_Threaded)
    {
        const unsigned int BUFFER_SIZE = 1024 * 1024;
        m_CommandQueue = new RingBuffer(BUFFER_SIZE);
        
        m_DeviceWorker = new RenderDeviceWorker(m_CommandQueue);

       // GfxThreadableDevice* realDevice = static_cast<GfxThreadableDevice*>(CreateRealGfxDevice(renderer));
        
        m_DeviceWorker->SetRealDevice(m_RealDevice);
        //device->SetRealDevice(realDevice);
        //SetRealGfxDevice(realDevice);
        m_DeviceWorker->Run();

    }
}

RenderDeviceClient::~RenderDeviceClient()
{


}

void RenderDeviceClient::BeginFrame()
{
    if (m_Threaded)
    {
        m_DeviceWorker->WaitForPresent();
        m_PresentPending = false;
        // Worker thread should check GetNeedsBeginFrame()
        m_CommandQueue->WriteValueType<GfxCommandType>(kGfxCmd_BeginFrame);
    }
    else
        m_RealDevice->BeginFrame();

}

void RenderDeviceClient::Present()
{
    if (!m_Threaded)
    {
        m_RealDevice->Present();
        m_PresentPending = false;
    }
    else
    {
        m_PresentPending = true;
        m_CommandQueue->WriteValueType<GfxCommandType>(kGfxCmd_Present);
        m_CommandQueue->WriteSubmitData();
    }
}

void RenderDeviceClient::SetClearColor(float r, float g, float b, float alpha)
{
    if (!m_Threaded)
    {
        m_RealDevice->Present();
        m_PresentPending = false;
    }
    else
    {
        m_PresentPending = true;
        m_CommandQueue->WriteValueType<GfxCommandType>(kGfxCmd_Clear);
        m_CommandQueue->WriteValueType<Color>(Color(r, g, b, alpha));
        m_CommandQueue->WriteSubmitData();
    }
}

void RenderDeviceClient::DrawTriangle(std::vector<Vector3>& vertices)
{
    if (!m_Threaded)
    {
        m_RealDevice->DrawTriangle(vertices);
        return;
    }
    m_CommandQueue->WriteValueType<>(kGfxCmd_DrawTriangle);
    m_CommandQueue->WriteSubmitData();
}

void RenderDeviceClient::SetViewPort(int x, int y, int width, int height)
{
    if (m_Threaded)
    {
        m_CommandQueue->WriteValueType<GfxCommandType>(kGfxCmd_SetViewport);
        m_CommandQueue->WriteValueType<Vector4>(Vector4(x, y, width, height));
        m_CommandQueue->WriteSubmitData();

    }
    else
        m_RealDevice->SetViewPort(x, y, width, height);
}
