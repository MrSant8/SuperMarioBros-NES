#pragma once
#include "Globals.h"
#include "Scene.h"
#include "FadeTransition.h"


class Game
{
public:
    Game();
    ~Game();

    AppStatus Initialise(float scale);
    AppStatus Update();
    void Render();
    void Cleanup();

private:
    AppStatus BeginPlay();
    void FinishPlay();

    AppStatus LoadResources();
    void UnloadResources();

    GameState state;
    Scene *scene;
    const Texture2D *img_menu, *img_menu2, *img_gameOver, *img_win, *img_world;

    //To work with original game units and then scale the result
    RenderTexture2D target;
    Rectangle src, dst;

    //To make fade out/in transitions effects
    FadeTransition fade_transition;

    //Music
    Music GroundMusic;


    //Wold variables
    int time;
    float timeCounter;
    float worldTimer = 0.0f;
    bool worldTimerStarted = false;

    //Intro variables
    float introTimer = 0.0f;
    bool introTimerStarted = false;
};