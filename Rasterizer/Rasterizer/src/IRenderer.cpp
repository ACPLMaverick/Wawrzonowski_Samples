#include "IRenderer.h"



IRenderer::IRenderer(SystemSettings* settings) :
	_bufferColor(settings->GetDisplayWidth()/* * 0.5f*/, settings->GetDisplayHeight()/* * 0.5f*/),
	_bufferDepth(settings->GetDisplayWidth()/* * 0.5f*/, settings->GetDisplayHeight()/* * 0.5f*/)
{
}


IRenderer::~IRenderer()
{
}
