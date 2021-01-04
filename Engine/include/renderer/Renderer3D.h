#pragma once

#include <maths/matrix/mat4x4.h>
#include <renderer/Mesh.h>
#include <renderer/Texture2D.h>
#include <renderer/Material.h>
#include <renderer/Model.h>
#include <renderer/Lights.h>
#include <renderer/Framebuffer.h>
#include <renderer/Skybox.h>

#include <util/PerspectiveCamera.h>
#include <scene/EditorCamera.h>

namespace Engine
{

struct Renderer3DData
{
    bool sceneStarted = false;
    Shared<UniformBuffer> lightingData;

    Shared<UniformBuffer> matrixData;

    Shared<Skybox> environment;
    Shared<Mesh> skyboxMesh;
    Shared<Shader> skyboxShader;
    Shared<Texture2D> shadowMap;
    Shared<Framebuffer> shadowMapFramebuffer;

    std::vector<const BaseLight*> lights;

    math::vec3 cameraPos;
};

class Renderer3D
{
public:
    static void init();
    static void shutdown();

    static void beginScene(PerspectiveCamera& camera);
    static void beginScene(EditorCamera& camera);

    static void submit(const Shared<Mesh>& mesh, const math::mat4& transform);
    static void submit(const Shared<Model>& model, const math::mat4& transform);
    static void submit(const Shared<Mesh>& mesh, const math::mat4& transform, const Shared<Material>& material);

    static void setEnvironment(const Shared<Skybox>& environment);

    static inline void addLight(const BaseLight* light) { data.lights.push_back(light); }
    static inline void clearLights() { std::vector<const BaseLight*>().swap(data.lights); }

    static void removeLight(const BaseLight* light);

    static void endScene();

    static Renderer3DData data;

private:
    static void setLightingUniforms(const Shared<Shader>& shader);
};

}