#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>

#include <scene/Components.h>

#include <renderer/Model.h>
#include <renderer/RenderCommand.h>
#include <renderer/Renderer3D.h>
#include <renderer/MeshFactory.h>
#include <renderer/Renderer.h>
#include <renderer/Assets.h>
#include <util/io/FileSystem.h>
#include <script/Script.h>
#include <scene/SceneCamera.h>
#include <audio/AudioListener.h>
#include <audio/AudioSource.h>
#include <audio/AudioBuffer.h>

namespace Engine
{

SceneHierarchyPanel::SceneHierarchyPanel()
{
    
}

SceneHierarchyPanel::SceneHierarchyPanel(Scene* scene)
    : m_context(scene)
{
    
}

void SceneHierarchyPanel::recurseTree(GameObject& object)
{
    ImGui::PushID(&object);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
    const char* tag = object.getComponent<Tag>()->tag.c_str();
    
    bool objectExpanded = ImGui::TreeNodeEx(tag, flags);

    if (ImGui::IsItemClicked())
    {
        m_selection = &object;
    }
    
    bool deleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Create Child"))
        {
            auto child = object.createChild();
            child->createComponent<Tag>("Untitled Child");
        }

        if (ImGui::MenuItem("Delete Game Object"))
        {
            deleted = true;
        }

        ImGui::EndPopup();
    }

    if (deleted)
    {
        m_selection = nullptr;
        m_deletedGameObject = &object;
    }

    if (objectExpanded)
    {
        for (auto object : object.getChildren())
        {
            recurseTree(*object);
        }
        
        
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void SceneHierarchyPanel::onImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_CollapsingHeader;
    bool gameObjectsOpen = ImGui::CollapsingHeader("Game Objects");

    if (ImGui::BeginPopupContextWindow(0, 1, false))
    {
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Empty Game Object"))
            {
                auto object = m_context->createGameObject("New Game Object");
                object->createComponent<Transform>();
                m_selection = object;
            }

            if (ImGui::MenuItem("Camera"))
            {
                auto object = m_context->createGameObject("New Camera");
                object->createComponent<Transform>();

                auto listener = object->createComponent<AudioListener>();
                listener->setPrimary(true);

                auto camera = object->createComponent<SceneCamera>();
                camera->primary = true;
                camera->setProjectionType(Camera::ProjectionType::Perspective);

                m_selection = object;
            }

            if (ImGui::MenuItem("Sprite"))
            {
                auto object = m_context->createGameObject("New Sprite");
                object->createComponent<Transform>();
                object->createComponent<SpriteRendererComponent>();
                m_selection = object;
            }

            if (ImGui::BeginMenu("Mesh"))
            {
                if (ImGui::MenuItem("Import"))
                {
                    FileDialog::open("meshload");
                }

                if (ImGui::MenuItem("Empty Mesh"))
                {
                    auto object = m_context->createGameObject("New Mesh");
                    object->createComponent<Transform>();
                    object->createComponent<Mesh>();
                    object->createComponent<MeshRendererComponent>();
                    m_selection = object;
                }
                if (ImGui::MenuItem("Cube"))
                {
                    auto object = m_context->createGameObject("Cube");
                    object->createComponent<Transform>();
                    auto mesh = object->createComponent<MeshComponent>();
                    mesh->mesh = MeshFactory::cubeMesh(1.f);
                    auto render = object->createComponent<MeshRendererComponent>();
                    m_selection = object;
                }
                if (ImGui::MenuItem("Sphere"))
                {
                    auto object = m_context->createGameObject("Sphere");
                    object->createComponent<Transform>();
                    auto mesh = object->createComponent<MeshComponent>();
                    mesh->mesh = MeshFactory::sphereMesh(1.f, 20, 20);
                    auto render = object->createComponent<MeshRendererComponent>();
                    m_selection = object;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Audio"))
            {
                if (ImGui::MenuItem("Audio Source"))
                {
                    auto object = m_context->createGameObject("Audio Source");
                    object->createComponent<Transform>();
                    object->createComponent<AudioSource>();
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Directional Light"))
            {
                auto object = m_context->createGameObject("Directional Light");
                object->createComponent<Transform>();
                object->createComponent<DirectionalLight>();
                m_selection = object;
            }

            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    
    if (FileDialog::selectFile("meshload", "Choose mesh...", ".obj", ".fbx", ".blend", ".3ds", ".gltf"))
    {
        if (!FileDialog::display())
        {
            if (FileDialog::madeSelection())
            {
                Reference<Mesh> mesh = Mesh::load(FileDialog::getSelection(), 0);

                auto object = m_context->createGameObject("Imported Mesh");
                object->createComponent<Transform>();
                auto meshRender = object->createComponent<MeshRendererComponent>();
                meshRender->material = mesh->material;

                auto meshComp = object->createComponent<MeshComponent>();
                meshComp->mesh = mesh;

                m_selection = object;
            }
        }
    }
    

    if (gameObjectsOpen)
    {   
        m_deletedGameObject = nullptr;
        auto gameObjects = m_context->getRootGameObject().getChildren();
        for (auto gameObject : gameObjects)
        {
            recurseTree(*gameObject);
        }

        if (m_deletedGameObject)
        {
            if (m_deletedGameObject->getParent() == nullptr)
                m_context->getRootGameObject().removeChild(m_deletedGameObject);
            else
                m_deletedGameObject->getParent()->removeChild(m_deletedGameObject);
        }
    }

    ImGui::End();

    ImGui::Begin("Inspector");

    if (m_selection != nullptr)
    {
        drawProperties(*m_selection);
    }

    ImGui::End();
}

#define ADD_COMPONENT(type, str) if (!object.hasComponent<type>())\
                                {\
                                    if (ImGui::Button(str)) {\
                                        object.createComponent<type>();\
                                        ImGui::CloseCurrentPopup();\
                                    }\
                                }

void SceneHierarchyPanel::drawProperties(GameObject& object)
{
    char buf[128];
    strcpy(buf, object.getComponent<Tag>()->tag.c_str());
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    ImGui::InputText("Name", buf, 128, flags);
    object.getComponent<Tag>()->tag = std::string(buf);

    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent"))
    {
        ADD_COMPONENT(Transform, "Transform");
        ADD_COMPONENT(SpriteRendererComponent, "Sprite Renderer");
        ADD_COMPONENT(SceneCamera, "Camera");
        ADD_COMPONENT(MeshComponent, "Mesh");
        ADD_COMPONENT(SkyLight, "Sky Light");
        ADD_COMPONENT(DirectionalLight, "Directional Light");
        ADD_COMPONENT(PointLight, "Point Light");
        ADD_COMPONENT(MeshRendererComponent, "Mesh Renderer");
        ADD_COMPONENT(ScriptInstance, "Script");
        ADD_COMPONENT(AudioListener, "Audio Listener");
        ADD_COMPONENT(AudioSource, "Audio Source");

        ImGui::EndPopup();
    }

    drawComponent<Transform>("Transform", object, [](auto* component)
    {
        ImGui::Columns(2);

        ImGui::PushID("Transform");

        ImGui::Text("Translation");
        ImGui::NextColumn();
        math::vec3 translation = component->getTranslation();
        ImGui::DragFloat3("##T", &translation.x);
        component->setTranslation(translation);
        ImGui::NextColumn();

        ImGui::Text("Rotation");
        ImGui::NextColumn();
        math::vec3 rotation = component->getRotation();
        ImGui::DragFloat3("##R", &rotation.x);
        component->setRotation(rotation);
        ImGui::NextColumn();

        ImGui::Text("Scale");
        ImGui::NextColumn();
        math::vec3 scale = component->getScale();
        ImGui::DragFloat3("##S", &scale.x);
        component->setScale(scale);
        ImGui::NextColumn();

        ImGui::PopID();

        ImGui::Columns(1);
    });

    drawComponent<SceneCamera>("Camera", object, [](auto* camera)
    {
        ImGui::Columns(2);

        ImGui::Text("Primary");
        ImGui::NextColumn();
        ImGui::Checkbox("##Primary", &camera->primary);
        ImGui::NextColumn();

        ImGui::Text("Projection");
        ImGui::NextColumn();

        const char* projectionTypes[] = { "Perspective", "Orthographic" };
        const char* currentProjectionType = projectionTypes[(int)camera->getProjectionType()];
        if (ImGui::BeginCombo("##Projection", currentProjectionType))
        {
            for (unsigned int i = 0; i < 2; i++)
            {
                bool isSelected = currentProjectionType == projectionTypes[i];
                if (ImGui::Selectable(projectionTypes[i], isSelected))
                {
                    currentProjectionType = projectionTypes[i];
                    camera->setProjectionType((Camera::ProjectionType)i);
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        ImGui::NextColumn();

        if (camera->getProjectionType() == Camera::ProjectionType::Orthographic)
        {
            float size = camera->getOrthoSize();
            ImGui::Text("Size");
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Size", &size))
            {
                camera->setOrthoSize(size);
            }
            ImGui::NextColumn();

            float near = camera->getOrthoNear();
            ImGui::Text("Near");
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Near", &near))
            {
                camera->setOrthoNear(near);
            }
            ImGui::NextColumn();

            float far = camera->getOrthoFar();
            ImGui::Text("Far");
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Far", &far))
            {
                camera->setOrthoFar(far);
            }
            ImGui::NextColumn();
        }
        else if (camera->getProjectionType() == Camera::ProjectionType::Perspective)
        {
            float fov = math::degrees(camera->getPerspectiveFov());
            ImGui::Text("FOV");
            ImGui::NextColumn();
            if (ImGui::SliderFloat("##FOV", &fov, 0, 180));
            {
                camera->setPerspectiveFov(math::radians(fov));
            }
            ImGui::NextColumn();

            float near = camera->getPerspectiveNear();
            ImGui::Text("Near");
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Near", &near))
            {
                camera->setPerspectiveNear(near);
            }
            ImGui::NextColumn();

            float far = camera->getPerspectiveFar();
            ImGui::Text("Far");
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Far", &far))
            {
                camera->setPerspectiveFar(far);
            }
            ImGui::NextColumn();
        }

        ImGui::Text("HDR");
        ImGui::NextColumn();
        bool hdr = false;
        ImGui::Checkbox("##HDR", &hdr);
        ImGui::NextColumn();

        ImGui::Columns(1);
    });

    drawComponent<SpriteRendererComponent>("Sprite Renderer", object, [&](auto* component)
    {
        ImGui::Columns(2);
        
        ImGui::Text("Color");
        ImGui::NextColumn();
        if (ImGui::ColorButton("##colpicker", { component->color.r, component->color.g, component->color.b, component->color.a }))
        {
            ImGui::OpenPopup("colorPickerEnabled");
        }
        if (ImGui::BeginPopup("colorPickerEnabled"))
        {
            ImGui::ColorPicker4("##Color", &(component->color.x));
            ImGui::EndPopup();
        }

        ImGui::NextColumn();

        if (component->texture == nullptr)
        {
            component->texture = Texture2D::createWhiteTexture();
        }
        
        ImGui::Text("Texture");
        ImGui::NextColumn();

        std::string currentTexture = component->texture != nullptr ? component->texture->name : "<empty_texture>";
        
        if (ImGui::BeginCombo("##Texture", currentTexture.c_str()))
        {
            for (auto& texture : Assets::getCache<Texture2D>())
            {
                ImGui::PushID(texture.second.get());

                bool isSelected = component->texture ? component->texture->getId() == texture.second->getId() : false;
                if (ImGui::Selectable(texture.second->name.c_str(), isSelected))
                {
                    component->texture = texture.second;
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }

                ImGui::PopID();
            }

            ImGui::EndCombo();
        }

        ImGui::NextColumn();

        ImGui::Text("Using Texture Region");
        ImGui::NextColumn();
        ImGui::Checkbox("##usingTextureRegion", &component->usingTexRect);
        ImGui::NextColumn();

        if (component->usingTexRect)
        {
            ImGui::Text("Position");
            ImGui::NextColumn();
            ImGui::DragFloat2("##Position", &component->textureRect.x);
            ImGui::NextColumn();

            ImGui::Text("Size");
            ImGui::NextColumn();
            ImGui::DragFloat2("##Size", &component->textureRect.w);
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
    });

    drawComponent<MeshComponent>("Mesh", object, [&object](auto* component)
    {
        ImGui::Columns(2);

        char buf[128];
        if (!component->mesh)
        {
            strcpy(buf, "<empty_mesh>");
        }
        else
        {
            strcpy(buf, component->mesh->path.c_str());
        }

        ImGui::Text("Filepath");
        ImGui::NextColumn();

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;
        ImGui::InputText("", buf, 128, flags);

        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            FileDialog::open(component);
        }

        ImGui::NextColumn();

        /*ImGui::Text("ID");
        ImGui::NextColumn();

        memset(buf, 0, 128);
        strcpy(buf, mesh != nullptr ? std::to_string(component.mesh->id).c_str() : 0);
        flags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (ImGui::InputText("##ID", buf, 128, flags))
        {
            int meshID = std::stoi(std::string(buf));
            component.mesh->id = meshID;
            component.mesh = Mesh::load(component.filePath, component.meshID);
        }*/

        if (FileDialog::selectFile(component, "Choose mesh...", ".obj", ".fbx", ".blend", ".3ds", ".gltf"))
        {
            if (!FileDialog::display())
            {
                if (FileDialog::madeSelection())
                {
                    component->mesh = Mesh::load(FileDialog::getSelection(), 0);
                    component->mesh->path = FileDialog::getSelection();
                }
            }
        }

        ImGui::NextColumn();

        ImGui::Columns(1);
    });

    drawComponent<SkyLight>("Sky Light", object, [](auto* component)
    {
        ImGui::Columns(2);
        ImGui::PushID("SkyLight");

        ImGui::Text("Radiance");
        ImGui::NextColumn();
        ImGui::ColorEdit3("##Radiance", &component->radiance.r);
        ImGui::NextColumn();

        ImGui::Text("Intensity");
        ImGui::NextColumn();
        ImGui::DragFloat("##Intensity", &component->intensity, 0.01f, 0.f);
        ImGui::PopID();
        ImGui::Columns(1);
    });

    drawComponent<DirectionalLight>("Directional Light", object, [](auto* component)
    {
        ImGui::Columns(2);
        ImGui::PushID("Directionallight");

        ImGui::Text("Radiance");
        ImGui::NextColumn();
        ImGui::ColorEdit3("##Radiance", &component->radiance.r);
        ImGui::NextColumn();

        ImGui::Text("Intensity");
        ImGui::NextColumn();
        ImGui::DragFloat("##Intensity", &component->intensity, 0.01f, 0.f);
        ImGui::PopID();
        ImGui::Columns(1);
    });

    drawComponent<PointLight>("Point Light", object, [](auto* component)
    {
        ImGui::Columns(2);
        ImGui::PushID("PointLight");

        ImGui::Text("Radiance");
        ImGui::NextColumn();
        ImGui::ColorEdit3("##Radiance", &component->radiance.r);
        ImGui::NextColumn();

        ImGui::Text("Intensity");
        ImGui::NextColumn();
        ImGui::DragFloat("##Intensity", &component->intensity, 0.01f, 0.f);
        ImGui::PopID();
        ImGui::Columns(1);
    });

    drawComponent<MeshRendererComponent>("Mesh Renderer", object, [](auto* component)
    {
        ImGui::Columns(2);

        ImGui::Text("Material");
        ImGui::NextColumn();

        // Material select

        std::string name = component->material != nullptr ? component->material->name : "<empty_material>";

        if (ImGui::BeginCombo("##materialSelect", name.c_str()))
        {
            for (auto& material : Assets::getCache<Material>().getInternalList())
            {
                ImGui::PushID(material.second.get());

                bool isSelected = material.second == component->material;
                if (ImGui::Selectable(material.second->name.c_str(), isSelected))
                {
                    component->material = material.second;
                    isSelected = true;
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }

                ImGui::PopID();
            }

            ImGui::EndCombo();
        }

        ImGui::NextColumn();

        ImGui::Text("Cast Shadows");
        ImGui::NextColumn();

        ImGui::Checkbox("##castShadows", &component->castShadows);
        ImGui::NextColumn();

        ImGui::Columns(1);
    });

    drawComponent<ScriptInstance>("C# Script", object, [](auto* component)
    {
        ImGui::Columns(2);

        ImGui::Text("Filepath");
        ImGui::NextColumn();

        /*char buf[128];
        strcpy(buf, component.filepath.c_str());
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;
        ImGui::InputText("##csharpScriptPath", buf, 128, flags);
        ImGui::SameLine();

        if (ImGui::Button("...##csharpScriptPathSelect"))
        {
            FileDialog::open("csharpScriptPathSelect");
        }

        if (FileDialog::selectFile("csharpScriptPathSelect", "Choose script...", ".cs"))
        {
            if (!FileDialog::display())
            {
                if (FileDialog::madeSelection())
                {
                    component.filepath = FileDialog::getSelection();
                }
            }
        }*/

        ImGui::NextColumn();
        ImGui::Columns(1);
    });

    drawComponent<AudioListener>("Audio Listener", object, [](auto* component)
    {
        ImGui::Columns(2);

        ImGui::Text("Primary");
        ImGui::NextColumn();
        bool primary = component->isPrimary();
        ImGui::Checkbox("##audioListenerPrimary", &primary);
        component->setPrimary(primary);

        ImGui::NextColumn();

        ImGui::Columns(1);
    });

    drawComponent<AudioSource>("Audio Source", object, [](auto* component)
    {
        ImGui::PushID("AudioSourceComponent");
        ImGui::Columns(2);

        ImGui::Text("Audio File");
        ImGui::NextColumn();
        
        if (ImGui::Button("..."))
        {
            FileDialog::open("selectAudioFile");
        }

        if (FileDialog::selectFile("selectAudioFile", "Choose audio file", ".mp3", ".wav"))
        {
            if (!FileDialog::display())
            {
                if (FileDialog::madeSelection())
                {
                    component->setBuffer(FileDialog::getSelection());
                }
            }
        }

        ImGui::NextColumn();

        ImGui::Text("Play on start");
        ImGui::NextColumn();
        ImGui::Checkbox("##playOnStart", &component->playOnStart);
        ImGui::NextColumn();

        ImGui::Text("Loop");
        ImGui::NextColumn();
        bool looped = component->isLooped();
        ImGui::Checkbox("##looped", &looped);
        component->setLooped(looped);
        ImGui::NextColumn();

        ImGui::Text("Volume");
        ImGui::NextColumn();
        float gain = component->getGain();
        ImGui::SliderFloat("##gain", &gain, 0, 2);
        component->setGain(gain);
        ImGui::NextColumn();

        ImGui::Text("Pitch");
        ImGui::NextColumn();
        float pitch = component->getPitch();
        ImGui::SliderFloat("##pitch", &pitch, 0, 2);
        component->setPitch(pitch);
        ImGui::NextColumn();

        ImGui::Columns(1);
        ImGui::PopID();
    });
}

template<typename T, typename F>
void SceneHierarchyPanel::drawComponent(const std::string& name, GameObject& object, const F& func)
{
    if (!object.hasComponent<T>())
    {
        return;
    }

    auto component = object.getComponent<T>();

    ImGui::Separator();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
    bool opened = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());

    bool deleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Remove Component"))
        {
            deleted = true;
        }

        ImGui::EndPopup();
    }

    if (opened)
    {
        func(component);

        ImGui::TreePop();
    }

    if (deleted)
    {
        object.removeComponent<T>();
    }
}

}