
#include "Game.h"
#include "MemLeaks.h"
#include "Globals.h"

int main()
{
    ReportMemoryLeaks();

    Game *game;
    bool end = false;

    LOG("Application start");
    game = new Game();
    if (!game->Initialise())
    {
        LOG("Failed to initialise game");
        end = true;
    }
        
    while (!end)
    {
        if(!game->Update()) break;
        game->Render();
    }

    LOG("Application finish");
    game->Cleanup();
 
    LOG("Bye :)");
    return 0;
}
