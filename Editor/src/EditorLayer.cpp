#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <core/Application.h>
#include <renderer/RenderCommand.h>
#include <scene/Components.h>
#include <renderer/Renderer2D.h>
#include <renderer/Renderer.h>
#include <renderer/Assets.h>

EditorLayer::EditorLayer()
{

}

void EditorLayer::onAttach()
{
    Assets::add<Texture2D>("texture", Texture2D::create("Editor/assets/texture.png"));
    Assets::add<Texture2D>("texture_1", Texture2D::create("Editor/assets/texture_1.png"));
    Assets::add<Texture2D>("texture_2", Texture2D::create("Editor/assets/texture_2.png"));

    m_scene = createShared<Scene>();

    auto entity1 = m_scene->getRegistry().create();
    m_scene->getRegistry().emplace<TagComponent>(entity1, "Entity 1");

    auto entity2 = m_scene->getRegistry().create();
    m_scene->getRegistry().emplace<TagComponent>(entity2, "Entity 2");
    m_scene->getRegistry().emplace<TransformComponent>(entity2, math::vec3(100, 100, 0), math::vec3(0), math::vec3(2));
    m_scene->getRegistry().emplace<CameraComponent>(entity2);

    m_sceneHeirarchy.setContext(m_scene);
    m_framebuffer = Framebuffer::create(1280, 720);
    m_viewportSize = math::vec2(1280, 720);
    m_camera.setPosition(math::vec2(0, 0));

    m_font = TrueTypeFont::create("Editor/assets/minecraftia.ttf", 48);
}

void EditorLayer::onUpdate(float dt)
{
    m_framebuffer->resize(m_viewportSize.x, m_viewportSize.y);

    m_camera.update(dt);

    m_framebuffer->bind();
    RenderCommand::setClearColor(math::vec4(0, 0, 0, 1));
    RenderCommand::clear(RenderCommand::defaultClearBits());

    Renderer2D::beginScene(m_camera);
    Renderer2D::renderSprite(Assets::get<Texture2D>("texture"), math::vec2(0, 0), math::vec2(100, 100));
    Renderer2D::renderSprite(Assets::get<Texture2D>("texture_1"), math::vec2(200, 0), math::vec2(200, 200));
    
    Renderer2D::renderSprite(Assets::get<Texture2D>("texture_2"), math::vec2(0, 200), math::vec2(200, 200));
    Renderer2D::renderQuad(math::vec2(0, 0), math::vec2(200, 200), math::vec4(1, 0, 0, 1));
    Renderer2D::endScene();

    Renderer2D::renderText("Hello, hello, hello, hello", m_font, math::vec2(0, 0), math::vec4(0, 0, 1, 1));

    m_scene->onUpdate();

    m_framebuffer->unbind();
}

void EditorLayer::onImGuiRender()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    bool dockspaceOpen = true;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("Window", &dockspaceOpen, windowFlags);
    ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.f, 0.f), ImGuiDockNodeFlags_None);

    ImGui::SetNextWindowSize(ImVec2{1280, 720});
    ImGui::Begin("Viewport");

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

    ImGui::Image(reinterpret_cast<void*>(m_framebuffer->getColorAttachment()), ImVec2{m_viewportSize.x, m_viewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
    ImGui::End();

    m_sceneHeirarchy.onImGuiRender();
    
    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorLayer::onDetach()
{
    
}

void EditorLayer::onViewportResize(WindowResizeEvent& event)
{
    m_scene->onViewportResize(event.getWidth(), event.getHeight());
}

void EditorLayer::onEvent(Event& event)
{
    m_camera.onEvent(event);
}