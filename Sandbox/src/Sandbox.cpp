#include "Sandbox.h"

Sandbox::Sandbox()
{
    Shared<Texture2D> texture = Texture2D::create("Sandbox/assets/terrain.png");
    Assets::add<Texture2D>("texture", texture);

    m_soundSource = SoundSource::loadFile("Sandbox/assets/monkeys.mp3");
    SoundEngine::play(*m_soundSource, true);
    
    m_framebuffer = Framebuffer::create(1280, 720);

    m_batch = SpriteBatch::create();

    m_font = TrueTypeFont::create("Sandbox/assets/minecraftia.ttf", 48);

    for (int i = 0; i < 20000; i++)
    {
        Sprite sprite((i % 10) * 100, (i / 10) * 100, 100, 100);

        sprite.setColor(math::vec4(1, 1, 1, 1));
        sprite.setTextureRect(FloatRect(i, i, 16.f, 16.f));

        m_sprites.push_back(sprite);
    }

    Renderer2D::setClearColor(math::vec4(0, 0, 0, 1));

    m_animation = Animation::create(Assets::get<Texture2D>("texture"));
    m_animation->setFrames({ 
        FloatRect(32, 240, 16, 16), 
        FloatRect(48, 240, 16, 16),
        FloatRect(64, 240, 16, 16),
        FloatRect(80, 240, 16, 16)
    });
    m_animation->setFrameInterval(100.f);
}

void Sandbox::update()
{
    auto dt = Time::getDelta();
    const float speed = 0.5;

    if (Input::isKeyPressed(Key::A))
        m_camera.translate(math::vec2(-speed * dt, 0));

    if (Input::isKeyPressed(Key::D))
        m_camera.translate(math::vec2(speed * dt, 0));

    if (Input::isKeyPressed(Key::W))
        m_camera.translate(math::vec2(0, speed * dt));

    if (Input::isKeyPressed(Key::S))
        m_camera.translate(math::vec2(0, -speed * dt));

    if (Input::isKeyPressed(Key::Escape))
    {
        Application::get().quit();
    }

    rot++;

    m_framebuffer->bind();
    Renderer2D::clear();

    m_sprites[0].setTextureRect(m_animation->getCurrentFrame());
    m_animation->update();

    m_batch->setTransformMatrix(m_camera.getViewMatrix());
    m_batch->start();

    for (auto& sprite : m_sprites)
    {
        sprite.setRotation(rot);
        m_batch->renderSprite(Assets::get<Texture2D>("texture"), sprite);
    }

    m_batch->flush();

    Renderer2D::renderText("Hello, world!", *m_font, math::vec2(500, 500), math::vec2(80, 80), math::vec4(1, 0, 0, 1));

    m_framebuffer->unbind();

    Renderer2D::clear();

    Renderer2D::renderFramebuffer(*m_framebuffer);

    Renderer2D::endFrame();
}

void Sandbox::handleEvent(const Event& event)
{
    if (event.type() == EventType::WindowResize)
    {
        m_framebuffer->resize(event.data().window.width, event.data().window.height);
    }
}