#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <string>
#include <vector>

#include "SoundBuffer.h"
#include "Source.h"

class SoundManager
{
public:
    static void init();

    static void cleanUp();
    static void destroy();

    static void play2D(const SoundBuffer& buffer, bool loop);
    static void play2D(const std::string& path, bool loop);

    static void playFromSource(const SoundBuffer& buffer, const Source& source, bool loop);

private:
    static ALCdevice* m_device;
    static ALCcontext* m_context;

    // For on-the-fly sound playback
    static Source m_source2D;
    static SoundBuffer m_buffer2D;
};