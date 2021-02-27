#include <stdio.h>

#define RL_CULL_DISTANCE_FAR 50
#include <raylib.h>
#include <raymath.h>

#include "EngineCore.h"
#include "Graphics.h"

int main()
{   
    struct EngineOptions options = { 0 };
    options.title = "Engine Test";
    options.width = 1600;
    options.height = 850;

    Engine engine = { 0 };
    ENGINE_init(&engine, &options);

    Camera3D camera = { 0 };
    camera.fovy = 65.0;
    camera.position = (Vector3){ 0, 0.5, 0 };
    camera.target = (Vector3){ 0, 0.5, -1 };
    camera.up = (Vector3){ 0, 1, 0 };
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FIRST_PERSON);

    Model groundPlane = LoadModelFromMesh(GenMeshPlane(10, 10, 1, 1));
    Model mainModel = LoadModelFromMesh(GenMeshKnot(1.0, 0.5, 32, 128));

    ShadowMap groundShadow = LoadShadowMap(2048, 2048);

    Camera3D shadowCam = { 0 };
    shadowCam.fovy = 90.0;
    shadowCam.position = (Vector3){ 2.5, 1, 1 };
    shadowCam.target = (Vector3){ 0, 0.5, 0 };
    shadowCam.up = (Vector3){ 0, 0, -1 };
    shadowCam.type = CAMERA_PERSPECTIVE;

    Shader shadowShader = LoadShader(ASSETS_PATH"shaders/shadow.vs", 0);
    shadowShader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shadowShader, "matModel");
    int matLightLoc0 = GetShaderLocation(shadowShader, "matLight");

    groundPlane.materials[0].maps[0].texture = groundShadow.depth;
    mainModel.materials[0].maps[0].texture = groundShadow.depth;

    Shader mainShader = LoadShader(ASSETS_PATH"shaders/geom.vs", ASSETS_PATH"shaders/geom.fs");
    mainShader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(mainShader, "matModel");
    mainShader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(mainShader, "viewPos");
    int lightPosLoc = GetShaderLocation(mainShader, "lightPos");
    int matLightLoc1 = GetShaderLocation(mainShader, "matLight");

    groundPlane.materials[0].shader = mainShader;
    mainModel.materials[0].shader = mainShader;

    Shader depthDisplayShader = LoadShader(0, ASSETS_PATH"shaders/depth.fs");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera);
        SetShaderValue(mainShader, mainShader.locs[LOC_VECTOR_VIEW], (float*)&camera.position, UNIFORM_VEC3);

        shadowCam.position.y += (float)sin(GetTime()) * 0.025f;

        SetShaderValue(mainShader, lightPosLoc, (float*)&shadowCam.position, UNIFORM_VEC3);

        Matrix LightMatrix = MatrixMultiply(MatrixLookAt(shadowCam.position, shadowCam.target, shadowCam.up),
                             MatrixPerspective(shadowCam.fovy*DEG2RAD, 1.0, 0.1, 50));
        SetShaderValueMatrix(shadowShader, matLightLoc0, LightMatrix);
        SetShaderValueMatrix(mainShader, matLightLoc1, LightMatrix);

        ShadowMapBegin(groundShadow);
            BeginMode3D(shadowCam);
                // ShaderMode doesn't work on 3D geometry :(
                mainModel.materials[0].shader = shadowShader;
                DrawModel(mainModel, (Vector3){ 0, 0.5, 0 }, 1.0f, RED);
            EndMode3D();
        ShadowMapEnd();

        BeginDrawing();
            ClearBackground(SKYBLUE);
            
            BeginMode3D(camera);
                mainModel.materials[0].shader = mainShader;
                DrawModel(mainModel, (Vector3){ 0, 0.5, 0 }, 1.0f, RED);
                DrawModel(groundPlane, Vector3Zero(), 1.0f, WHITE);
                DrawCube(shadowCam.position, 0.25, 0.25, 0.25, YELLOW);
            EndMode3D();

            BeginShaderMode(depthDisplayShader);
                DrawTexturePro(groundShadow.depth, (Rectangle){ 0, 0, (float)groundShadow.width, (float)groundShadow.height },
                (Rectangle){ 0, 0, 256, 256 }, Vector2Zero(), 0, WHITE);
            EndShaderMode();
            DrawText("SHADOWMAP", 10, 256, 24, YELLOW);
        EndDrawing();
    }

    UnloadModel(groundPlane);
    UnloadModel(mainModel);
    UnloadShadowMap(groundShadow);
    UnloadShader(shadowShader);
    UnloadShader(mainShader);
    UnloadShader(depthDisplayShader);

    ENGINE_free(&engine);
}