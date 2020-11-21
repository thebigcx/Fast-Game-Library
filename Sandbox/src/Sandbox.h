#pragma once

#include <Engine.h>

class Sandbox : public Layer
{
public:
    Sandbox();

    void update() override;
    void handleEvent(const Event& event) override;

private:
    Shared<SoundSource> m_soundSource;
    Shared<TrueTypeFont> m_font;
    Shared<Material> m_cubeMaterial;

    Shared<Model> m_model;

    Shared<Framebuffer> m_framebuffer;

    Shared<ParticleSystem> m_particleSystem;
    
    Shared<Animation> m_animation;
    float rot = 0;

    OrthographicCamera m_camera;
    PerspectiveCamera m_perspectiveCamera;
};