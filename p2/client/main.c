
#pragma clang diagnostic ignored "-Wgnu-folding-constant"
#pragma ide diagnostic ignored "cert-err34-c"

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#if !defined(__clang__) || !defined(__LITTLE_ENDIAN__) || __STDC_VERSION__ < 201112L
#   error "Unsupported configuration"
#endif

_Static_assert(sizeof(char) == 1 && sizeof(short) == 2 && sizeof(int) == 4 && sizeof(long) == 8 && sizeof(void*) == 8, "");

typedef unsigned char byte;

static inline int* asIntPtr(void* x) { return ((int*) x); }
static inline float* asFloatPtr(void* x) { return ((float*) x); }

static void floatPrinter(const void* a) { printf("Result: %f\n", *((const float*) a)); }
static void intPrinter(const void* a) { printf("Result: %d\n", *((const int*) a)); }
static void boolPrinter(const void* a) { printf("Result: %s\n", *((const bool*) a) ? "true" : "false"); }

int main(void) {
    SDL_Init(0);
    SDLNet_Init();

    IPaddress ipAddress;
    SDLNet_ResolveHost(&ipAddress, "127.0.0.1", 8080);
    TCPsocket socket = SDLNet_TCP_Open(&ipAddress);
    if (!socket) abort();

    int command = 0;

    const int bufferSize = 1024;
    byte sendBuffer[bufferSize] = {0}, receiveBuffer[bufferSize] = {0};
    void (*printer)(const void*) = NULL;
    const void* result = NULL;

    while (true) {
        printf("enter command: ");
        scanf("%d", &command);

        SDL_memset(sendBuffer, 0, bufferSize);
        SDL_memset(receiveBuffer, 0, bufferSize);
        *asIntPtr(sendBuffer) = command;

        switch (command) {
            case 19: {
                printf("enter 3 float arguments separated by spaces: ");
                float a, b, c;
                scanf("%f %f %f", &a, &b, &c);

                *asFloatPtr(sendBuffer + 4) = a;
                *asFloatPtr(sendBuffer + 4 * 2) = b;
                *asFloatPtr(sendBuffer + 4 * 3) = c;

                printer = &floatPrinter;
                } break;
            case 22: {
                printf("enter 4 int arguments separated by spaces: ");
                int a, b, c, d;
                scanf("%d %d %d %d", &a, &b, &c, &d);

                *asIntPtr(sendBuffer + 4) = a;
                *asIntPtr(sendBuffer + 4 * 2) = b;
                *asIntPtr(sendBuffer + 4 * 3) = c;
                *asIntPtr(sendBuffer + 4 * 4) = d;

                printer = &boolPrinter;
                } break;
            case 25: {
                printf("enter 1 int argument: ");
                int a;
                scanf("%d", &a);

                *asIntPtr(sendBuffer + 4) = a;

                printer = &intPrinter;
                } break;
            case 28: __attribute__((fallthrough));
            case 3: {
                printf("enter 2 float argument: ");
                float a, b;
                scanf("%f %f", &a, &b);

                *asFloatPtr(sendBuffer + 4) = a;
                *asFloatPtr(sendBuffer + 4 * 2) = b;

                printer = &floatPrinter;
                } break;
            case 0:
                break;
            default:
                goto end;
        }

        if (SDLNet_TCP_Send(socket, sendBuffer, bufferSize) != bufferSize) goto end;
        if (SDLNet_TCP_Recv(socket, receiveBuffer, bufferSize) != bufferSize) goto end;

        if (printer == &floatPrinter)
            result = asFloatPtr(receiveBuffer);
        else if (printer == &intPrinter)
            result = asIntPtr(receiveBuffer);
        else if (printer == boolPrinter)
            result = (bool*) asIntPtr(receiveBuffer);

        if (!printer || !result) abort();
        (*printer)(result);
    }
    end:

    SDLNet_TCP_Close(socket);

    SDLNet_Quit();
    SDL_Quit();

    if (SDL_GetNumAllocations() > 0) abort();
    return EXIT_SUCCESS;
}
