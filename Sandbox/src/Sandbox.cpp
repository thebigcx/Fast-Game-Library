#include "Sandbox.h"

Sandbox::Sandbox()
{
    Assets::add<Texture2D>("texture", Texture2D::create("Sandbox/assets/terrain.png"));
    Assets::add<Texture2D>("grass", Texture2D::create("Sandbox/assets/grass.png"));

    

    m_soundSource = SoundSource::loadFile("Sandbox/assets/monkeys.mp3");
    SoundEngine::play(*m_soundSource, true);
    
    m_framebuffer = Framebuffer::create(1280, 720);

    m_font = TrueTypeFont::create("Sandbox/assets/minecraftia.ttf", 48);

    RenderCommand::setClearColor(math::vec4(0, 0, 0, 1));
    Renderer::setTarget(m_framebuffer);

    m_animation = Animation::create(Assets::get<Texture2D>("texture"));
    m_animation->setFrames({ 
        math::frect(32, 240, 16, 16), 
        math::frect(48, 240, 16, 16),
        math::frect(64, 240, 16, 16),
        math::frect(80, 240, 16, 16)
    });
    m_animation->setFrameInterval(100.f);

    m_particleSystem = ParticleSystem::create(Assets::get<Texture2D>("texture"));
    for (int i = 0 ; i < 10 ; i++)
    {
        Particle p;
        p.life = 1.f;
        p.velocity = math::vec2(math::random::generate(-5, 5), math::random::generate(-5, 5));
        m_particleSystem->particles.push_back(p);
    }

    m_cubeMaterial = Material::create(Renderer2D::data.textureShader);
    m_cubeMaterial->setTexture(Assets::get<Texture2D>("grass"));

    m_model = Model::loadModel("Sandbox/assets/model/backpack.obj");
    
    Application::get().setCursorEnabled(false);
}

void Sandbox::update()
{
    Timer timer;
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

    Renderer::startFrame();

    m_animation->update();

    m_particleSystem->update();
    m_particleSystem->render();

    Renderer2D::startBatch(m_camera.getViewMatrix());

    for (int x = 0; x < 100; x++)
    for (int y = 0; y < 1000; y++)
    {
        //Renderer2D::renderQuad(math::vec2(x * 11, y * 11), math::vec2(10, 10), math::vec4(1, 1, 1, 1));
    }

    Renderer2D::renderSprite(Assets::get<Texture2D>("texture"), math::vec2(100, 100), math::vec2(100, 100), m_animation->getCurrentFrame());
    
    Renderer2D::endBatch();

    Renderer2D::renderText("Hello, world!", m_font, math::vec2(500, 500), math::vec2(80, 80), math::vec4(1, 0, 0, 1));

    math::mat4 view = math::translate(math::mat4(1.f), math::vec3(0.f, 0.f, -3.f));
    math::mat4 transform(1.f);
    Renderer3D::submit(m_model, m_perspectiveCamera.getViewMatrix() * transform);

    m_perspectiveCamera.update();

    Renderer::endFrame();

    Application::get().getWindow().setTitle(std::string("Sandbox FPS: " + std::to_string((int)floor(1000.f / timer.getMillis()))));
}

void Sandbox::handleEvent(const Event& event)
{
    if (event.type() == EventType::WindowResize)
    {
        m_framebuffer->resize(event.data().window.width, event.data().window.height);
    }
}