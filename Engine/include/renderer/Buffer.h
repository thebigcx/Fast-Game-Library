#pragma once

#include <cstdlib>
#include <string>
#include <vector>

#include <renderer/shader/Shader.h>

struct BufferElement
{
    BufferElement(Shader::DataType type, const std::string& name)
        : type(type), name(name)
    {
        size = dataTypeSize();
    }

    int componentCount() const
    {
        switch (type)
        {
            using Type = Shader::DataType;

            case Type::Float:
            case Type::Bool:
            case Type::Int:
            case Type::Double:
            case Type::Uint:    return 1;

            case Type::Vec2:
            case Type::iVec2:
            case Type::uVec2:
            case Type::dVec2:
            case Type::bVec2:   return 2;

            case Type::Vec3:
            case Type::iVec3:
            case Type::uVec3:
            case Type::dVec3:
            case Type::bVec3:    return 3;

            case Type::Vec4:
            case Type::iVec4:
            case Type::uVec4:
            case Type::dVec4:
            case Type::bVec4:    return 4;

            case Type::Mat3:
            case Type::dMat3:    return 9;

            case Type::Mat4:
            case Type::dMat4:    return 16;
            
            case Type::Color:   return 4;

            default:                        return 0;
        }
    }

    size_t dataTypeSize()
    {
        size_t size = 0;
        
        if (getOpenGLType() == GL_FLOAT)
            size = sizeof(float);
        else if (getOpenGLType() == GL_INT)
            size = sizeof(int);

        return componentCount() * size;
    }

    GLenum getOpenGLType() const
    {
        switch (type)
        {
            using Type = Shader::DataType;

            case Type::Float:
            case Type::Vec2:
            case Type::Vec3:
            case Type::Vec4:
            case Type::Mat3:
            case Type::Mat4:
            case Type::Color:   return GL_FLOAT;

            case Type::Bool:
            case Type::bVec2:
            case Type::bVec3:
            case Type::bVec4:
            case Type::Int:
            case Type::iVec2:
            case Type::iVec3:
            case Type::iVec4:   return GL_INT;

            case Type::Uint:
            case Type::uVec2:
            case Type::uVec3:
            case Type::uVec4:   return GL_UNSIGNED_INT;

            case Type::Double:
            case Type::dVec2:
            case Type::dVec3:
            case Type::dVec4:
            case Type::dMat3:
            case Type::dMat4:   return GL_DOUBLE;

            default:            return GL_FLOAT;
        }
    }

    Shader::DataType type;
    std::string name;
    size_t offset;
    size_t size;
};



class BufferLayout
{
public:
    BufferLayout()
    {

    }

    BufferLayout(const std::initializer_list<BufferElement>& elements)
        : m_elements(elements)
    {
        setOffsets();
    }

    void setOffsets()
    {
        size_t offset = 0;
        m_stride = 0;
        for (auto& element : m_elements)
        {
            element.offset = offset;
            offset += element.size;
            m_stride += element.size;
        }
    }

    int size() const
    {
        return m_elements.size();
    }

    const BufferElement& operator[](int index) const
    {
        return m_elements[index];
    }

    size_t getStride() const
    {
        return m_stride;
    }

private:
    std::vector<BufferElement> m_elements;

    size_t m_stride;
};



class VertexBuffer
{
public:
    VertexBuffer();
    ~VertexBuffer();

    void update(const void* data, size_t size);

    void bind() const;

    void create(size_t size);

    void setLayout(const BufferLayout& layout);
    const BufferLayout& getLayout() const;

private:
    unsigned int m_id;

    BufferLayout m_layout;
};

class IndexBuffer
{
public:
    IndexBuffer();
    ~IndexBuffer();

    void update(const unsigned int* data, unsigned int size);

    void create(unsigned int size);

    void bind() const;

    inline unsigned int getCount() const
    {
        return m_count;
    }

    inline GLenum getIndexType() const
    {
        return m_indexType;
    }

private:
    unsigned int m_id = 0;

    unsigned int m_count = 0;

    const GLenum m_indexType = GL_UNSIGNED_INT;
};