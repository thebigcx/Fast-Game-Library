#include "FGL/renderer/Renderer.h"
#include "FGL/core/Application.h"

#include <GL/glew.h>

const Window* Renderer::m_currentContext = nullptr;
RenderData Renderer::m_data;

void Renderer::init()
{
    glDisable(GL_DEPTH_TEST);

    m_data.shader.create("shaders/default.vert", "shaders/default.frag");
}

void Renderer::clear(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::display()
{
    auto& window = Application::get().getWindow();
    window.display();
}

void Renderer::renderBatch(const Batch& batch)
{
    batch.getShader()->bind();

    batch.getVertexArray().bind();

    glDrawElements(GL_TRIANGLES, batch.getIndexBuffer().getCount(), batch.getIndexBuffer().getIndexType(), 0);
}

void Renderer::renderIndexed(const VertexArray& array, Shader& shader, const glm::mat4& transform)
{
    shader.bind();
    shader.setUniform("transform", transform);
    array.bind();

    glDrawElements(GL_TRIANGLES, array.getIndexBuffer()->getCount(), array.getIndexBuffer()->getIndexType(), 0);
}

void Renderer::setContext(const Window& window)
{
    window.makeCurrentContext();
    m_currentContext = &window;
}