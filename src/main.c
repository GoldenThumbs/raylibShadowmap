#include <stdio.h>

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include "EngineCore.h"
#include "Graphics.h"

int main()
{   
    struct EngineOptions options = { 0 };
    options.title = "Engine Test";
    options.width = 1600;
    options.height = 850;
	double screenAspect = (double)options.width / (double)options.height;

    Engine engine = { 0 };
    ENGINE_init(&engine, &options);
	DisableCursor();

	CCamera camera = { 0 };
	InitCamera(&camera, (Vector3){ 0.0, 0.5, 0.0 }, (Vector3){ 0.0, 0.0, 0.0 }, 90.0f, 0.1f, 100.0f, screenAspect, false);

	CCamera caster = { 0 };
	InitCamera(&caster, (Vector3){ 0, 1, 1 }, (Vector3){ -45, 0, 0 }, 90.0f, 0.01f, 10.0f, 1.0, false);

	ShadowMap shadow = LoadShadowMap(1024, 1024);

	Shader shader_shadow = LoadShader(ASSETS_PATH"shaders/shadow.vs", ASSETS_PATH"shaders/shadow.fs");

	Shader shader_model = LoadShader(ASSETS_PATH"shaders/geom.vs", ASSETS_PATH"shaders/geom.fs");
	shader_model.locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(shader_model, "shadowMap");
	shader_model.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader_model, "viewPos");
	int shader_model_matLight = GetShaderLocation(shader_model, "matLight");
	int shader_model_lightPos = GetShaderLocation(shader_model, "lightPos");
	int shader_model_lightDir = GetShaderLocation(shader_model, "lightDir");
	int shader_model_lightCutoff = GetShaderLocation(shader_model, "lightCutoff");

	Shader shader_depth = LoadShader(0, ASSETS_PATH"shaders/depth.fs");

    Model groundPlane = LoadModelFromMesh(GenMeshPlane(10, 10, 1, 1));
    Model mainModel = LoadModelFromMesh(GenMeshKnot(1.0, 0.5, 32, 128));

    SetTargetFPS(60);

	Vector2 camRot = (Vector2){ 0, 0 };
    while (!WindowShouldClose())
    {	
		// ----- UPDATE -----
		float delta = GetFrameTime();

		caster.transform.rotation = QuaternionFromEuler(((float)sin(GetTime()) * 45.0f + -45.0f) * DEG2RAD, 0, 0);

		camRot = Vector2Add(camRot, Vector2Scale(GetMouseDelta(), 0.5));
		camRot.y = Clamp(camRot.y, -90.0f, 90.0f);
		camera.transform.rotation = QuaternionFromEuler(-camRot.y*DEG2RAD, -camRot.x*DEG2RAD, 0);

		Vector3 velocity = (Vector3){ 0, 0, 0 };
		if (IsKeyDown(KEY_W))
			velocity = Vector3Add(velocity, (Vector3){ 0, 0,-1 });
		if (IsKeyDown(KEY_S))
			velocity = Vector3Add(velocity, (Vector3){ 0, 0, 1 });
		if (IsKeyDown(KEY_A))
			velocity = Vector3Add(velocity, (Vector3){-1, 0, 0 });
		if (IsKeyDown(KEY_D))
			velocity = Vector3Add(velocity, (Vector3){ 1, 0, 0 });
		velocity = Vector3Normalize(velocity);
		camera.transform.position = Vector3Add(camera.transform.position, Vector3RotateByQuaternion(Vector3Scale(velocity, delta*5.0f), QuaternionFromEuler(0, -camRot.x*DEG2RAD, 0)) );

		CalcCamera(&camera);
		CalcCamera(&caster);

		Matrix matLight = MatrixMultiply(caster.view, caster.projection);
		Vector3 casterDir = Vector3RotateByQuaternion((Vector3){ 0, 0, -1 }, caster.transform.rotation);
		// ------------------


		// ----- RENDER -----
		ShadowMapBegin(shadow);
		CameraBegin(caster);
			groundPlane.materials[0].shader = shader_shadow;
			mainModel.materials[0].shader = shader_shadow;
			DrawModel(mainModel, (Vector3){ 0, 0.5, 0 }, 1.0f, RED);
		CameraEnd();
		ShadowMapEnd();

		BeginDrawing();
		ClearBackground(DARKGRAY);
		CameraBegin(camera);
			float cutoff = cosf(DEG2RAD * caster.fov * 0.46f);
			SetShaderValue(shader_model, shader_model_lightPos, (float*)&caster.transform.position, SHADER_UNIFORM_VEC3);
			SetShaderValue(shader_model, shader_model_lightDir, (float*)&casterDir, SHADER_UNIFORM_VEC3);
			SetShaderValue(shader_model, shader_model_lightCutoff, &cutoff, SHADER_UNIFORM_FLOAT);
			SetShaderValueMatrix(shader_model, shader_model_matLight, matLight);
			SetShaderValue(shader_model, shader_model.locs[SHADER_LOC_VECTOR_VIEW], (float*)&camera.transform.position, SHADER_UNIFORM_VEC3);

			groundPlane.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = shadow.depth;
			mainModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = shadow.depth;

			groundPlane.materials[0].shader = shader_model;
			mainModel.materials[0].shader = shader_model;
			DrawModel(groundPlane, (Vector3){ 0, 0, 0 }, 1.0f, WHITE);
			DrawModel(mainModel, (Vector3){ 0, 0.5, 0 }, 1.0f, RED);

			Vector3 endPos = Vector3Scale(casterDir, 0.5f);
			endPos = Vector3Add(caster.transform.position, endPos);
			DrawCubeWires(caster.transform.position, 0.125f, 0.125f, 0.125f, YELLOW);
			DrawLine3D(caster.transform.position, endPos, LIME);
		CameraEnd();
		BeginShaderMode(shader_depth);
			DrawTextureEx(shadow.depth, (Vector2){ 0, 0 }, 0.0f, 0.25f, WHITE);
		EndShaderMode();
		EndDrawing();
		// ------------------
    }

    UnloadModel(groundPlane);
    UnloadModel(mainModel);
	UnloadShadowMap(shadow);
	UnloadShader(shader_shadow);
	UnloadShader(shader_model);
	UnloadShader(shader_depth);

    ENGINE_free(&engine);
}