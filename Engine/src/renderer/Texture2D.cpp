#include <renderer/Texture2D.h>

#include <util/Image.h>
#include <platform/GL/GLTexture2D.h>

#include <iostream>
#include <thread>

namespace Engine
{

Shared<Texture2D> Texture2D::s_whiteTexture = nullptr;

Shared<Texture2D> Texture2D::create(const std::string& file, bool isSRGB, bool clamp, bool linear)
{
    return createShared<GLTexture2D>(file, isSRGB, clamp, linear);
}

Shared<Texture2D> Texture2D::create(int width, int height, GLenum dataFormat, bool clamp, bool linear)
{
    return createShared<GLTexture2D>(width, height, dataFormat, clamp, linear);
}

Shared<Texture2D> Texture2D::createWhiteTexture()
{
    if (!s_whiteTexture)
    {
        s_whiteTexture = createShared<GLTexture2D>(1, 1);
        uint32_t white = 0xffffffff;
        s_whiteTexture->setData(0, 0, 1, 1, &white);
    }

    return s_whiteTexture;
}

Shared<Texture2D> Texture2D::asyncCreate(const std::string& file, bool isSRGB)
{
    Shared<Image> img;
    std::thread thr([&]
    {
        img = ImageLoader::loadOpenGLImage(file);
    });
    

    Shared<Texture2D> texture = createShared<GLTexture2D>(img->width, img->height);
    texture->setData(0, 0, img->width, img->height, img->data);
}

}