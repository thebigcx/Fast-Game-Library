#pragma once

namespace Engine
{

class IRenderable2D
{
public:
    virtual ~IRenderable2D() = default;
    virtual void render() = 0;
};

}