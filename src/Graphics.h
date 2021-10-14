#ifndef ENGINE_GRAPHICS_H
#define ENGINE_GRAPHICS_H

#include <raylib.h>

typedef struct Graphics
{
    char *windowTitle;
    int width;
    int height;
    void *window;
}Graphics;

int GRAPHICS_init(Graphics *graphics);
void GRAPHICS_free(Graphics *graphics);

typedef struct Transform3D
{
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;

	Matrix world;
}Transform3D;

void InitTransform(Transform3D *transform, Vector3 position, Vector3 rotation, float scale);
void CalcTransform(Transform3D *transform);

typedef struct CCamera 
{
	Transform3D transform;
	float fov;
	float nearClip;
	float farClip;
	double aspect;
	bool orthographic;
	Matrix projection;
	Matrix view;
}CCamera;

void InitCamera(CCamera *camera, Vector3 position, Vector3 rotation, float fov, float near, float far, double aspect, bool ortho);
void CalcCamera(CCamera *camera);
void CameraBegin(CCamera camera);
void CameraEnd();

typedef struct ShadowMap
{
    unsigned int id;
    int width;
    int height;
    Texture2D depth;
}ShadowMap;

ShadowMap LoadShadowMap(int width, int height);
void UnloadShadowMap(ShadowMap shadowMap);
void ShadowMapBegin(ShadowMap shadowMap);
void ShadowMapEnd();

#endif