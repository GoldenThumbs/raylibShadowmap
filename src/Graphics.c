#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include <rlgl.h>
#include <raymath.h>

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

void InitTransform(Transform3D *transform, Vector3 position, Vector3 rotation, float scale)
{
	transform->position = position;
	transform->rotation = QuaternionFromEuler(rotation.x*DEG2RAD, rotation.y*DEG2RAD, rotation.z*DEG2RAD);
	transform->scale = (Vector3){ scale, scale, scale };
	CalcTransform(transform);
}

void CalcTransform(Transform3D *transform)
{
	Matrix position = MatrixTranslate(transform->position.x, transform->position.y, transform->position.z);
	Matrix rotation = QuaternionToMatrix(transform->rotation);
	Matrix scale = MatrixScale(transform->scale.x, transform->scale.y, transform->scale.z);
	transform->world = MatrixMultiply(MatrixMultiply(scale, rotation), position);
}

void InitCamera(CCamera *camera, Vector3 position, Vector3 rotation, float fov, float near, float far, double aspect, bool ortho)
{
	camera->transform.position = position;
	camera->transform.rotation = QuaternionFromEuler(rotation.x*DEG2RAD, rotation.y*DEG2RAD, rotation.z*DEG2RAD);
	camera->transform.scale = (Vector3){ 1, 1, 1 };
	camera->fov = fov;
	camera->nearClip = near;
	camera->farClip = far;
	camera->aspect = aspect;
	camera->orthographic = ortho;
	CalcCamera(camera);
}

void CalcCamera(CCamera *camera)
{
	CalcTransform(&camera->transform);
	camera->view = MatrixInvert(camera->transform.world);
	if (camera->orthographic)
	{
		double top = camera->fov/2.0;
		double right = top*camera->aspect;
		camera->projection = MatrixOrtho(-right, right, -top, top, camera->nearClip, camera->farClip);
	}else
		camera->projection = MatrixPerspective(camera->fov*DEG2RAD, camera->aspect, camera->nearClip, camera->farClip);
}

void CameraBegin(CCamera camera)
{
	rlDrawRenderBatchActive();

	rlMatrixMode(RL_PROJECTION);
	rlPushMatrix();
	rlLoadIdentity();

	rlMultMatrixf(MatrixToFloat(camera.projection));

	rlMatrixMode(RL_MODELVIEW);
	rlLoadIdentity();

	rlMultMatrixf(MatrixToFloat(camera.view));

	rlEnableDepthTest();
}

void CameraEnd()
{
	EndMode3D();
}

ShadowMap LoadShadowMap(int width, int height)
{
    ShadowMap _shadowMap;

	_shadowMap.id = rlLoadFramebuffer(width, height);
    _shadowMap.width = width;
    _shadowMap.height = height;

	rlEnableFramebuffer(_shadowMap.id);
	
	_shadowMap.depth.id = rlLoadTextureDepth(width, height, false);
    _shadowMap.depth.width = width;
    _shadowMap.depth.height = height;
    _shadowMap.depth.format = 19;
    _shadowMap.depth.mipmaps = 1;

    rlTextureParameters(_shadowMap.depth.id, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_CLAMP);
    rlTextureParameters(_shadowMap.depth.id, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_CLAMP);
    rlFramebufferAttach(_shadowMap.id, _shadowMap.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

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
    rlDrawRenderBatchActive();
	rlEnableFramebuffer(shadowMap.id);

	rlClearScreenBuffers();

	// Set viewport to framebuffer size
	rlViewport(0, 0, shadowMap.width, shadowMap.height);

	rlMatrixMode(RL_PROJECTION);        // Switch to projection matrix
	rlLoadIdentity();                   // Reset current matrix (projection)

	// Set orthographic projection to current framebuffer size
	// NOTE: Configured top-left corner as (0, 0)
	rlOrtho(0, shadowMap.width, shadowMap.height, 0, 0, 1);

	rlMatrixMode(RL_MODELVIEW);         // Switch back to modelview matrix
	rlLoadIdentity();                   // Reset current matrix (modelview)

	rlClearColor(255, 255, 255, 255);
	rlDisableColorBlend();
}

void ShadowMapEnd()
{
    rlEnableColorBlend();
	rlDisableTexture();

	rlDrawRenderBatchActive();
	rlDisableFramebuffer();

	rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	rlMatrixMode(RL_PROJECTION);
	rlLoadIdentity();
	
	rlOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, 0, 1);
	
	rlMatrixMode(RL_MODELVIEW);
	rlLoadIdentity();
}
