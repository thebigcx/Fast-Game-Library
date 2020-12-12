#include <desktop/ImGuiLayer.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <core/Application.h>
#include <renderer/RenderCommand.h>

ImGuiLayer::ImGuiLayer()
{

}

void ImGuiLayer::onAttach()
{
    ImGui::CreateContext();

    auto& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = nullptr;

    io.Fonts->AddFontFromFileTTF("Editor/assets/Roboto.ttf", 18.f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Editor/assets/Roboto.ttf", 18.f);

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    auto& window = Application::get().getWindow();

    ImGui_ImplGlfw_InitForOpenGL(window.getNative(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void ImGuiLayer::onUpdate(float dt)
{

}

void ImGuiLayer::begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::end()
{
    auto& io = ImGui::GetIO();

    Application& app = Application::get();
    io.DisplaySize = ImVec2((float)app.getWindow().getWidth(), (float)app.getWindow().getHeight());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ImGuiLayer::onDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
}

void ImGuiLayer::onEvent(Event& event)
{
    if (m_blockEvents)
    {
        auto& io = ImGui::GetIO();
        event.handled |= event.categories() & io.WantCaptureMouse;
        event.handled |= event.categories() & io.WantCaptureKeyboard;
    }
}