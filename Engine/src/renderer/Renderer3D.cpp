#include <renderer/Renderer3D.h>
#include <core/Application.h>
#include <maths/matrix/matrix_transform.h>
#include <renderer/RenderCommand.h>
#include <renderer/shader/ShaderFactory.h>
#include <renderer/MeshFactory.h>

Renderer3DData Renderer3D::data;

void Renderer3D::init()
{
    math::ivec2 windowSize = Application::get().getWindow().getSize();
    
    data.modelShader = ShaderFactory::lightingShader();

    data.matrixData = UniformBuffer::create(sizeof(math::mat4) * 2, 0);

    data.lightingData = UniformBuffer::create(sizeof(DirectionalLight)
                                            + sizeof(PointLight) * 64
                                            + sizeof(SpotLight) * 64
                                            + sizeof(uint32_t)
                                            + sizeof(uint32_t)
                                            + sizeof(math::vec3)
                                            + sizeof(float), 1);

    std::array<std::string, 6> skyboxFaces = {
        "Sandbox/assets/skybox/right.jpg",
        "Sandbox/assets/skybox/left.jpg",
        "Sandbox/assets/skybox/top.jpg",
        "Sandbox/assets/skybox/bottom.jpg",
        "Sandbox/assets/skybox/front.jpg",
        "Sandbox/assets/skybox/back.jpg"
    };

    data.environment = Skybox::create(skyboxFaces);
    data.skyboxMesh = MeshFactory::skyboxMesh();
    data.skyboxShader = Shader::createFromFile("Engine/src/renderer/shader/default/skybox.glsl");
}

void Renderer3D::shutdown()
{
    
}

void Renderer3D::beginScene(PerspectiveCamera& camera)
{
    if (data.sceneStarted)
    {
        Logger::getCoreLogger()->error("beginScene() must be called before endScene()!");
    }

    data.sceneStarted = true;
    data.camera = &camera;

    data.matrixData->setData(math::buffer(data.camera->getProjectionMatrix()), sizeof(math::mat4), 0);
    data.matrixData->setData(math::buffer(data.camera->getViewMatrix()), sizeof(math::mat4), sizeof(math::mat4));

    data.modelShader->bind();
    data.modelShader->setFloat3("cameraPos", data.camera->getPosition());
    //math::vec3 camPos = data.camera->getPosition();
    //data.lightingData->setData(&camPos.x, 16, 48);
}

void Renderer3D::endScene()
{
    data.sceneStarted = false;

    glDepthFunc(GL_LEQUAL);
    data.skyboxShader->bind();
    data.skyboxMesh->vertexArray->bind();
    data.environment->getCubemap()->bind();
    RenderCommand::renderIndexed(data.skyboxMesh->vertexArray);
    glDepthFunc(GL_LESS);
}

void Renderer3D::submit(const Shared<Mesh>& mesh, const math::mat4& transform)
{
    if (!data.sceneStarted)
    {
        Logger::getCoreLogger()->error("beginScene() must be called before executing draw calls!");
    }

    RenderCommand::setDepthTesting(true);

    mesh->material->bind();
    mesh->material->getShader()->setMatrix4("transform", transform);
    mesh->material->getShader()->setFloat("material.shininess", mesh->material->shininess);

    mesh->vertexArray->bind();

    RenderCommand::renderIndexed(mesh->vertexArray);
}

void Renderer3D::submit(const Shared<Model>& model, const math::mat4& transform)
{
    if (!data.sceneStarted)
    {
        Logger::getCoreLogger()->error("beginScene() must be called before executing draw calls!");
    }

    RenderCommand::setDepthTesting(true);

    for (auto& mesh : model->meshes)
    {
        mesh->material->bind();
        mesh->material->getShader()->setMatrix4("transform", transform);
        mesh->material->getShader()->setFloat("material.shininess", mesh->material->shininess);

        mesh->vertexArray->bind();

        RenderCommand::renderIndexed(mesh->vertexArray);
    }
}

void Renderer3D::setLights(const LightSetup& setup)
{
    data.modelShader->bind();

    size_t counter = 0;

    //data.lightingData->bind();

    //void* buffer = data.lightingData->getBufferPtr();

    data.modelShader->setFloat("skyLight", setup.getSkyLight());
    //float skylight = setup.getSkyLight();
    //data.lightingData->setData(&skylight, 4, 48);
    //memcpy();
    
    DirectionalLight dirLight = setup.getDirectionalLight();
    data.modelShader->setFloat3("dirLight.direction", dirLight.direction);
    data.modelShader->setFloat3("dirLight.color", dirLight.color);
    data.modelShader->setFloat("dirLight.intensity", dirLight.intensity);
    data.modelShader->setFloat("dirLight.specular", dirLight.specular);

    //data.lightingData->setData(&(setup.getDirectionalLight().direction.x), 48, 0);

    //data.lightingData->unmap();

    auto& pointLights = setup.getPointLights();
    for (unsigned int i = 0; i < 64; i++)
    {
        if (i >= pointLights.size())
        {
            counter += sizeof(PointLight);
            continue;
        }
        std::string index = std::to_string(i);

        auto& light = pointLights[i];

        data.modelShader->setFloat3("pointLights[" + index + "].position",  light.position);
        data.modelShader->setFloat3("pointLights[" + index + "].color",   light.color);
        data.modelShader->setFloat("pointLights[" + index + "].intensity",  light.intensity);
        data.modelShader->setFloat("pointLights[" + index + "].specular",  light.specular);
        data.modelShader->setFloat("pointLights[" + index + "].attenuation", light.attenuation);
        /*data.lightingData->setData(&(light.position.x), sizeof(PointLight), counter);
        counter += sizeof(PointLight);*/
    }

    auto& spotLights = setup.getSpotLights();
    for (unsigned int i = 0; i < 64; i++)
    {
        if (i >= spotLights.size())
        {
            counter += sizeof(SpotLight);
            continue;
        }
        std::string index = std::to_string(i);

        auto& light = spotLights[i];

        data.modelShader->setFloat3("spotLights[" + index + "].position",  light.position);
        data.modelShader->setFloat3("spotLights[" + index + "].direction",  light.direction);
        data.modelShader->setFloat3("spotLights[" + index + "].color",   light.color);
        data.modelShader->setFloat("spotLights[" + index + "].intensity",   light.intensity);
        data.modelShader->setFloat("spotLights[" + index + "].specular",  light.specular);
        data.modelShader->setFloat("spotLights[" + index + "].attenuation", light.attenuation);
        data.modelShader->setFloat("spotLights[" + index + "].cutoff", light.cutoff);
        data.modelShader->setFloat("spotLights[" + index + "].outerCutoff", light.outerCutoff);
        /*data.lightingData->setData(&(light.position.x), sizeof(SpotLight), counter);
        counter += sizeof(SpotLight);*/
    }
    counter += sizeof(PointLight) * 64 + sizeof(SpotLight) * 64;

    Renderer3D::data.modelShader->setInt("numPointLights", pointLights.size());
    Renderer3D::data.modelShader->setInt("numSpotLights", spotLights.size());
    
    /*int num = pointLights.size();
    data.lightingData->setData(&num, sizeof(uint32_t), counter);
    counter += sizeof(uint32_t);

    num = spotLights.size();
    data.lightingData->setData(&num, sizeof(uint32_t), counter);
    counter += sizeof(uint32_t);

    math::vec3 camPos = math::vec3(1, 1, 1);
    data.lightingData->setData(&camPos, sizeof(math::vec3), counter);
    counter += sizeof(math::vec3); // Camera pos set later

    float skylight = setup.getSkyLight();
    data.lightingData->setData(&skylight, sizeof(float), counter);*/
}

void Renderer3D::setEnvironment(const Shared<Skybox>& environment)
{
    data.environment = environment;
}