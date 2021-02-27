#include <raylib.h>
#include "EngineCore.h"

int ENGINE_init(Engine *engine,  struct EngineOptions *options)
{
    if (options)
    {
        engine->graphics.width = options->width;
        engine->graphics.height = options->height;
        engine->graphics.windowTitle = options->title;
    }
    GRAPHICS_init(&engine->graphics);
    engine->quit = 0;
    return 0;
}

int ENGINE_free(Engine *engine)
{
    GRAPHICS_free(&engine->graphics);
    return 0;
}