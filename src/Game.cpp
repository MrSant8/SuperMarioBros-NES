#include "Game.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <stdio.h>

Game::Game()
{
    state = GameState::PLAYING;
    scene = nullptr;
    img_menu = nullptr;

    target = {};
    src = {};
    dst = {};

    GroundMusic = { 0 }; 
}
Game::~Game()
{
    if (scene != nullptr)
    {
        scene->Release();
        delete scene;
        scene = nullptr;
    }
}
AppStatus Game::Initialise(float scale)
{
    float w, h;
    w = WINDOW_WIDTH * scale;
    h = WINDOW_HEIGHT * scale;

    //Initialise window
    InitWindow((int)w, (int)h, "Mario Bros NES");

    InitAudioDevice(); // Initialize Audio System

    //Render texture initialisation, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (target.id == 0)
    {
        LOG("Error al cargar las texturas");
        return AppStatus::ERROR;
    }
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    src = { 0, 0, WINDOW_WIDTH, -WINDOW_HEIGHT };
    dst = { 0, 0, w, h };

    //Load resources
    if (LoadResources() != AppStatus::OK)
    {
        LOG("Error al cargar los resources");
        return AppStatus::ERROR;
    }

    //Initialise the fade in effect
    fade_transition.Set(GameState::INTRO, 60, dst); // modificar esto ma costao encontrarlo este es para el inicio del juego

    //Set the target frame rate for the application
    SetTargetFPS(60);
    //Disable the escape key to quit functionality
    SetExitKey(0);

    return AppStatus::OK;
}
AppStatus Game::LoadResources()
{
    ResourceManager& data = ResourceManager::Instance();

    // Primero cargamos la textura para el intro
    if (data.LoadTexture(Resource::IMG_MENU_INTRO, "Assets/Textures/Hud/menuFinalIntroNames.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_menu2 = data.GetTexture(Resource::IMG_MENU_INTRO);



    // Luego cargamos la textura para el men�
    if (data.LoadTexture(Resource::IMG_MENU, "Assets/Textures/Hud/Start game.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_menu = data.GetTexture(Resource::IMG_MENU);

   
    if (data.LoadTexture(Resource::IMG_WORLD, "Assets/Textures/Hud/World.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_world = data.GetTexture(Resource::IMG_WORLD);

   


    //Imagen game over
    if (data.LoadTexture(Resource::IMG_GAMEOVER, "Assets/Textures/Hud/Game over.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_gameOver = data.GetTexture(Resource::IMG_GAMEOVER);

    if (data.LoadTexture(Resource::WORLD_1LIVES, "Assets/Textures/Hud/World_1lives.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_world1lives = data.GetTexture(Resource::WORLD_1LIVES);


    if (data.LoadTexture(Resource::WORLD_2LIVES, "Assets/Textures/Hud/World_2lives.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_world2lives = data.GetTexture(Resource::WORLD_2LIVES);

    //Imagen wikn
    if (data.LoadTexture(Resource::IMG_WIN, "Assets/Textures/Hud/Finish game.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_win = data.GetTexture(Resource::IMG_WIN);

    // Cargar m�sica
    GroundMusic = LoadMusicStream("Assets/Audio/Music/GroundTheme.mp3");
    if (GroundMusic.stream.buffer == nullptr)
    {
        LOG("Failed to load game music");
        return AppStatus::ERROR;
    }
    GameOverMusic = LoadMusicStream("Assets/Audio/Music/Game Over.wav");
    if (GameOverMusic.stream.buffer == nullptr)
    {
        LOG("Failed to load game music");
        return AppStatus::ERROR;
    }    
    WinMusic = LoadMusicStream("Assets/Audio/Music/smb_stage_clear.wav");
    if (WinMusic.stream.buffer == nullptr)
    {
        LOG("Failed to load game music");
        return AppStatus::ERROR;
    }
    starMusic = LoadMusicStream("Assets/Audio/Music/StarPowerMusic.wav");
    if (starMusic.stream.buffer == nullptr)
    {
        LOG("Failed to load game starMusic");
        return AppStatus::ERROR;
    }

    return AppStatus::OK;
}

AppStatus Game::BeginPlay()
{
    scene = new Scene();

    if (scene == nullptr)
    {
        LOG("Failed to allocate memory for Scene");
        return AppStatus::ERROR;
    }
    if (scene->Init() != AppStatus::OK)
    {
        LOG("Failed to initialise Scene");
        return AppStatus::ERROR;
    }
    //Reestart Music
    SeekMusicStream(GroundMusic, 0.0f); 
    PlayMusicStream(GroundMusic);      

    return AppStatus::OK;
}
void Game::FinishPlay()
{
    scene->Release();
    delete scene;
    scene = nullptr;

    StopMusicStream(GroundMusic); // Stop Music
}
AppStatus Game::Update()
{
    //Check if user attempts to close the window, either by clicking the close button or by pressing Alt+F4
    if(WindowShouldClose()) return AppStatus::QUIT;

    if (fade_transition.IsActive())
    {
        GameState prev_frame = state;
        state = fade_transition.Update();

        //Begin play and finish play are delayed due to the fading transition effect
        if (prev_frame == GameState::MAIN_MENU && state == GameState::PLAYING)
        {
            if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
        }
        else if (prev_frame == GameState::WORLD && state == GameState::PLAYING)
        {
            if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
        }
        if (prev_frame == GameState::PLAYING && state == GameState::MAIN_MENU)
        {
            FinishPlay();
        }
    }
    else
    {
        switch (state)
        {
        case GameState::INTRO:
            if (!introTimerStarted) {
                introTimer = 0.0f;
                introTimerStarted = true;
            }

            introTimer += GetFrameTime();

            if (introTimer >= 5.0f) {
                introTimerStarted = false;
                fade_transition.Set(GameState::INTRO, 60, GameState::MAIN_MENU, 60, dst);

            }

            if (IsKeyPressed(KEY_TWO))
            {
                introTimerStarted = false;

                StopMusicStream(GroundMusic); //Music Stops
                StopMusicStream(GameOverMusic);
                StopMusicStream(WinMusic);
                state = GameState::WORLD;

            }
            break;
       
        case GameState::MAIN_MENU:
            if (IsKeyPressed(KEY_ESCAPE)) return AppStatus::QUIT;
            if (IsKeyPressed(KEY_SPACE))
            {
                StopMusicStream(GroundMusic); //Music Stops
                StopMusicStream(GameOverMusic);
                StopMusicStream(WinMusic);
                PlayMusicStream(GroundMusic);  //Music Plays
                SetMusicVolume(GroundMusic, 1.0f);
                //"state = GameState::PLAYING;" but not until halfway through the transition
                fade_transition.Set(GameState::MAIN_MENU, 60, GameState::WORLD, 60, dst);
            }
            break;
        case GameState::WORLD:
            if (!worldTimerStarted) {
                worldTimer = 0.0f;
                worldTimerStarted = true;
            }

            worldTimer += GetFrameTime();

            if (worldTimer >= 2.0f) {
                worldTimerStarted = false;
                fade_transition.Set(GameState::WORLD, 60, GameState::PLAYING, 60, dst);
            }
            break;

            case GameState::PLAYING:  
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    //"state = GameState::MAIN_MENU;" but not until halfway through the transition
                    fade_transition.Set(GameState::PLAYING, 60, GameState::MAIN_MENU, 60, dst);
                }
                else
                {
                    
                    //Game logic
                    scene->Update();
                    UpdateMusicStream(GroundMusic);
                    UpdateMusicStream(starMusic);
                    if (scene && scene->GetPlayer()) {
                        Player* player = scene->GetPlayer();  // accede al player desde la escena

                        if (!player->isStarMario) {
                            StopMusicStream(starMusic);
                            if (!IsMusicStreamPlaying(GroundMusic)) {
                                PlayMusicStream(GroundMusic);

                            }
                        }
                        else {
                            StopMusicStream(GroundMusic);
                            if (!IsMusicStreamPlaying(starMusic)) {
                                PlayMusicStream(starMusic);
                                UpdateMusicStream(starMusic);
                            }
                        }
                    }

                    if (scene->gameOver) 
                    {
                        state = GameState::GAMEOVER;
                    }
                    if (scene->mariolive2)
                    {
                        state = GameState::WORLD_2LIVES;
                    }
                    if (scene->mariolive1)
                    {
                        state = GameState::WORLD_1LIVES;
                    }
                    if (scene->win)
                    {
                        state = GameState::WIN;
                    }
                }
                break;

            case GameState::GAMEOVER:
                StopMusicStream(GroundMusic);
                PlayMusicStream(GameOverMusic);

                UpdateMusicStream(GameOverMusic);
                if (IsKeyPressed(KEY_SPACE))
                {
                   state = GameState::MAIN_MENU;
                }
                break;

            case GameState::WORLD_2LIVES:
                StopMusicStream(GroundMusic);
                PlayMusicStream(GameOverMusic);

                UpdateMusicStream(GameOverMusic);
                scene->mariolive2 = false;
                if (IsKeyPressed(KEY_SPACE))
                {
                    FinishPlay();
                    if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
                    scene->life = 2;
                    state = GameState::PLAYING;
                }
                break;

            case GameState::WORLD_1LIVES:
                StopMusicStream(GroundMusic);
                PlayMusicStream(GameOverMusic);

                UpdateMusicStream(GameOverMusic);
                scene->mariolive1 = false;
                if (IsKeyPressed(KEY_SPACE))
                {
                    FinishPlay();
                    if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
                    scene->life = 1;
                    state = GameState::PLAYING;
                }
                break;


            case GameState::WIN:
                StopMusicStream(GroundMusic);
                PlayMusicStream(WinMusic);
                UpdateMusicStream(WinMusic);
                if (IsKeyPressed(KEY_SPACE))
                {
                    
                    state = GameState::MAIN_MENU;
                }
                break;
        }
    }

    return AppStatus::OK;
}
void Game::Render()
{
    //Draw everything in the render texture, note this will not be rendered on screen, yet
    BeginTextureMode(target);
    ClearBackground({92,148,252, 255});
    
    switch (state)
    {
        
        case GameState::INTRO:
            DrawTexture(*img_menu2, 0, 0, WHITE);
            break;
    
        case GameState::MAIN_MENU:
            DrawTexture(*img_menu, 0, 0, WHITE);
            break;
        case GameState::WORLD:
            DrawTexture(*img_world, 0, 0, WHITE);
            break;
        case GameState::WORLD_2LIVES:
            DrawTexture(*img_world2lives, 0, 0, WHITE);
            break;
        case GameState::WORLD_1LIVES:
            DrawTexture(*img_world1lives, 0, 0, WHITE);
            break;
        case GameState::PLAYING:
            scene->Render();
            break;
        case GameState::GAMEOVER:
            DrawTexture(*img_gameOver, 0, 0, WHITE);
            break;
        case GameState::WIN:
            DrawTexture(*img_win, 0, 0, WHITE);
            break;
    }
    
    EndTextureMode();

    //Draw render texture to screen, properly scaled
    BeginDrawing();
    DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
    if (fade_transition.IsActive()) fade_transition.Render();
    EndDrawing();
}
void Game::Cleanup()
{
    UnloadResources();
    CloseAudioDevice();
    CloseWindow();
}
void Game::UnloadResources()
{
    ResourceManager& data = ResourceManager::Instance();
    data.ReleaseTexture(Resource::IMG_MENU);
    data.ReleaseTexture(Resource::IMG_MENU_INTRO);
    data.ReleaseTexture(Resource::IMG_GAMEOVER);
    data.ReleaseTexture(Resource::IMG_WIN);
    data.ReleaseTexture(Resource::IMG_WORLD);
    data.ReleaseTexture(Resource::WORLD_2LIVES);
    data.ReleaseTexture(Resource::WORLD_1LIVES);


    UnloadMusicStream(GroundMusic);
    UnloadMusicStream(GameOverMusic);
    UnloadMusicStream(WinMusic);
    UnloadRenderTexture(target);
}