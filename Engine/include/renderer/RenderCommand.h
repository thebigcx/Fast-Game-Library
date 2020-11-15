#pragma once

#include <renderer/RendererAPI.h>

class RenderCommand
{
public:
    static void init()
    {
        m_api = RendererAPI::create();
        m_api->init();
    }

    static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        m_api->setViewport(x, y, width, height);
    }

    static void setClearColor(float r, float g, float b, float a)
    {
        m_api->setClearColor(r, g, b, a);
    }

    static void setDepthTesting(bool enabled)
    {
        m_api->setDepthTesting(enabled);
    }

    static void clear(uint32_t buffer)
    {
        m_api->clear(buffer);
    }

    static void setBlend(bool enabled)
    {
        m_api->setBlend(enabled);
    }

    static void setBlendFunction(BlendFunction src, BlendFunction dst)
    {
        m_api->setBlendFunction(src, dst);
    }

    static void renderIndexed(Shared<VertexArray> array)
    {
        m_api->renderIndexed(array);
    }

private:
    static Unique<RendererAPI> m_api;
};