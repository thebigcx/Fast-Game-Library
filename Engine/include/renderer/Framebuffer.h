#pragma once

#include <memory>

#include <GL/glew.h>

#include <core/Core.h>
#include <maths/math.h>
#include <renderer/Texture2D.h>

namespace Engine
{

enum class ColorBuffer
{
    None = 0,
    FrontLeft = 1 << 0,
    FrontRight = 1 << 1,
    BackLeft = 1 << 2,
    BackRight = 1 << 3
};

class Framebuffer
{
public:
    virtual ~Framebuffer() = default;

    static Shared<Framebuffer> create(uint32_t width, uint32_t height);
    static Shared<Framebuffer> create(const Shared<Texture2D>& texture, GLenum attachment); // TODO: make platform independent

    virtual void resize(uint32_t width, uint32_t height) = 0;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual math::vec2 getSize() const = 0;

    virtual uint32_t getColorAttachment() const = 0;
    virtual uint32_t getDepthAttachment() const = 0;

    virtual void drawBuffer(uint32_t buffer) const = 0;
    virtual void readBuffer(uint32_t buffer) const = 0;

    virtual bool operator==(const Framebuffer& buffer) const = 0;
    virtual bool operator!=(const Framebuffer& buffer) const = 0;
};

}