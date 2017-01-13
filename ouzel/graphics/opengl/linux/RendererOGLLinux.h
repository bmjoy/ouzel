// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/opengl/RendererOGL.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLLinux: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLLinux();
            virtual void free() override;

            virtual bool present() override;

        private:
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              TextureFilter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync,
                              bool newDepth) override;
        };
    } // namespace graphics
} // namespace ouzel