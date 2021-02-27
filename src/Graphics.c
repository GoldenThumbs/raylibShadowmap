#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include <rlgl.h>
#include <external/glad.h>

#include "Graphics.h"

int GRAPHICS_init(Graphics *graphics)
{
    if (!graphics->windowTitle)
        graphics->windowTitle = "No Name";
    if (!graphics->width)
        graphics->width = 800;
    if (!graphics->height)
        graphics->height = 450;
    
    InitWindow(
        graphics->width,
        graphics->height,
        graphics->windowTitle
    );
    graphics->window = GetWindowHandle();

    if (!graphics->window)
    {
        TraceLog(LOG_ERROR, "FAILED TO CREATE WINDOW");
        return 1;
    }

    return 0;
}

void GRAPHICS_free(Graphics *graphics)
{
    CloseWindow();
}

ShadowMap LoadShadowMap(int width, int height)
{
    ShadowMap _shadowMap;

    _shadowMap.depth.id = 0;
    _shadowMap.depth.width = width;
    _shadowMap.depth.height = height;
    _shadowMap.depth.format = 19;
    _shadowMap.depth.mipmaps = 0;

    _shadowMap.id = rlLoadFramebuffer(width, height);
    _shadowMap.width = width;
    _shadowMap.height = height;

    //glGenTextures(1, &_shadowMap.depth.id);
    //glBindTexture(GL_TEXTURE_2D, _shadowMap.depth.id);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    _shadowMap.depth.id = rlLoadTextureDepth(width, height, false);
    rlTextureParameters(_shadowMap.depth.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_LINEAR);
    rlTextureParameters(_shadowMap.depth.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_LINEAR);
    glBindTexture(GL_TEXTURE_2D, _shadowMap.depth.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);
    rlFramebufferAttach(_shadowMap.id, _shadowMap.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D);

    rlDisableFramebuffer();

    return _shadowMap;
}

void UnloadShadowMap(ShadowMap shadowMap)
{
    rlUnloadFramebuffer(shadowMap.id);
    rlUnloadTexture(shadowMap.depth.id);
}

void ShadowMapBegin(ShadowMap shadowMap)
{
    rlglDraw();
    rlEnableFramebuffer(shadowMap.id);

    rlClearScreenBuffers();

    rlViewport(0, 0, shadowMap.width, shadowMap.height);

    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();

    rlOrtho(0, shadowMap.width, shadowMap.height, 0, 0.0f, 1.0f);

    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();

    ClearBackground(WHITE);
}

void ShadowMapEnd()
{
    rlglDraw();
        
    rlDisableFramebuffer();
    
    rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());
    
    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();
    
    rlOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, 0, 1);
    
    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();
}
