// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Renderer.h"
#include "core/Engine.h"
#include "Texture.h"
#include "Shader.h"
#include "events/EventHandler.h"
#include "MeshBuffer.h"
#include "events/EventDispatcher.h"
#include "RenderTarget.h"
#include "BlendState.h"
#include "core/Window.h"

namespace ouzel
{
    namespace graphics
    {
        Renderer::Renderer(Driver pDriver):
            driver(pDriver), clearColor(0, 0, 0, 255), activeDrawQueueFinished(false), refillDrawQueue(true)
        {

        }

        Renderer::~Renderer()
        {

        }

        void Renderer::free()
        {
            activeDrawQueue.clear();
            ready = false;
        }

        bool Renderer::init(const WindowPtr& window,
                            uint32_t newSampleCount,
                            TextureFiltering newTextureFiltering,
                            bool newVerticalSync)
        {
            size = window->getSize();
            fullscreen = window->isFullscreen();
            sampleCount = newSampleCount;
            textureFiltering = newTextureFiltering;
            verticalSync = newVerticalSync;

            ready = true;

            return true;
        }

        bool Renderer::present()
        {
            if (activeDrawQueueFinished)
            {
                drawQueue = std::move(activeDrawQueue);
                drawCallCount = static_cast<uint32_t>(drawQueue.size());
                activeDrawQueueFinished = false;
                refillDrawQueue = true;
            }

            return true;
        }

        void Renderer::setSize(const Size2& newSize)
        {
            size = newSize;
        }

        void Renderer::setFullscreen(bool newFullscreen)
        {
            fullscreen = newFullscreen;
        }

        std::vector<Size2> Renderer::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        BlendStatePtr Renderer::createBlendState()
        {
            BlendStatePtr blendState(new BlendState());
            return blendState;
        }

        TexturePtr Renderer::createTexture()
        {
            TexturePtr texture(new Texture());
            return texture;
        }

        RenderTargetPtr Renderer::createRenderTarget()
        {
            RenderTargetPtr renderTarget(new RenderTarget());
            return renderTarget;
        }

        ShaderPtr Renderer::createShader()
        {
            ShaderPtr shader(new Shader());
            return shader;
        }

        MeshBufferPtr Renderer::createMeshBuffer()
        {
            MeshBufferPtr meshBuffer(new MeshBuffer());
            return meshBuffer;
        }

        bool Renderer::addDrawCommand(const std::vector<TexturePtr>& textures,
                                      const ShaderPtr& shader,
                                      const std::vector<std::vector<float>>& pixelShaderConstants,
                                      const std::vector<std::vector<float>>& vertexShaderConstants,
                                      const BlendStatePtr& blendState,
                                      const MeshBufferPtr& meshBuffer,
                                      uint32_t indexCount,
                                      DrawMode drawMode,
                                      uint32_t startIndex,
                                      const RenderTargetPtr& renderTarget,
                                      bool scissorTestEnabled,
                                      const Rectangle& scissorTest)
        {
            activeDrawQueue.push_back({
                textures,
                shader,
                pixelShaderConstants,
                vertexShaderConstants,
                blendState,
                meshBuffer,
                (indexCount > 0) ? indexCount : meshBuffer->getIndexCount(),
                drawMode,
                startIndex,
                renderTarget,
                scissorTestEnabled,
                scissorTest
            });

            return true;
        }

        void Renderer::flushDrawCommands()
        {
            refillDrawQueue = false;
            activeDrawQueueFinished = true;
        }

        Vector2 Renderer::viewToScreenLocation(const Vector2& position)
        {
            float x = 2.0f * position.x / size.width - 1.0f;
            float y = 2.0f * (size.height - position.y) / size.height - 1.0f;

            return Vector2(x, y);
        }

        Vector2 Renderer::viewToScreenRelativeLocation(const Vector2& position)
        {
            float x = 2.0f * position.x / size.width;
            float y = -2.0f * position.y / size.height;

            return Vector2(x, y);
        }

        Vector2 Renderer::screenToViewLocation(const Vector2& position)
        {
            float x = (position.x + 1.0f) / 2.0f * size.width;
            float y = size.height - (position.y + 1.0f) / 2.0f * size.height;

            return Vector2(x, y);
        }

        bool Renderer::saveScreenshot(const std::string& filename)
        {
            std::lock_guard<std::mutex> lock(screenshotMutex);

            screenshotQueue.push(filename);

            return true;
        }

        void Renderer::scheduleUpdate(const ResourcePtr& resource)
        {
            std::lock_guard<std::mutex> lock(updateMutex);

            auto i = updateSet.insert(resource);

            if (i.second)
            {
                updateQueue.push(resource);
            }
        }
    } // namespace graphics
} // namespace ouzel
