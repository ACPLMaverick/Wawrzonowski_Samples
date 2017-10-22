#include "RendererMav.h"
#include "../Scene.h"

namespace rendererMav
{
	RendererMav::RendererMav(SystemSettings * settings) :
		IRenderer(settings)
	{
		_device.Initialize(&_bufferColor, &_bufferDepth);
	}


	RendererMav::~RendererMav()
	{
		_device.Shutdown();
	}

	void RendererMav::Draw(Scene * scene)
	{
		_bufferColor.Fill(0x00CCCCCC);
		_bufferDepth.Fill(FLT_MAX);
		scene->Draw();
	}

	Buffer<float>* RendererMav::GetDepthBuffer()
	{
		return &_bufferDepth;
	}
	GraphicsDevice * RendererMav::GetGraphicsDevice()
	{
		return &_device;
	}
}