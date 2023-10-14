
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

int main(void) {
    SDL_Init(0);
    SDLNet_Init();

    IPaddress ipAddress;
    SDLNet_ResolveHost(&ipAddress, "127.0.0.1", 8080);
    TCPsocket socket = SDLNet_TCP_Open(&ipAddress);
    if (!socket) abort();



    SDLNet_TCP_Close(socket);

    SDLNet_Quit();
    SDL_Quit();

    if (SDL_GetNumAllocations() > 1) abort();
    return EXIT_SUCCESS;
}
