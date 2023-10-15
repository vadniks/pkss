
#pragma clang diagnostic ignored "-Wgnu-folding-constant"

#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

static inline float t19(float a, float b, float c) {
    return a < b
        ? a < c ? a : c
        : b < c ? b : c;
}

static inline bool t22(int a, int b, int c, int d)
{ return a % 2 == 0 && b % 2 == 0 && c % 2 == 0 && d % 2 == 0; }

static inline float xLog2f(float n)
{ return SDL_logf(n) / SDL_logf(2); }

static int t25(int n) {
    const int a = (int) xLog2f((float) n);
    return n <= 0
        ? -1
        : n == (int) SDL_powf((float) 2, (float) a) ? a : -1;
}

static inline float t28(float x, float y) {
    return (2.0f * SDL_powf(x, 3.0f) - 4.0f * SDL_powf(x, 2.0f) + x + 1.0f)
        / (9 * SDL_powf(y, 3.0f) + y + 4) + (3 * SDL_powf(y, 2.0f) + 5 * y);
}

static inline float t3f(float a, float b, float c)
{ return (2 * a - b - SDL_sinf(c)) / (5 + c); }

static float t3(float s, float t)
{ return t3f(t, -1 * 2 * s, 1.17f) + t3f(2.2f, t, s - t); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_Static_assert(sizeof(char) == 1 && sizeof(short) == 2 && sizeof(int) == 4 && sizeof(long) == 8 && sizeof(void*) == 8, "");

typedef unsigned char byte;

static _Atomic bool running = true;

#define asIntPtr(x) ((int*) x)
#define asFloatPtr(x) ((float*) x)

static SDL_mutex* mutex = NULL;

static char* resolveHost(const IPaddress* ipAddress) { // nullable
    SDL_LockMutex(mutex);
    char* copy = NULL;

    const char* original = SDLNet_ResolveIP(ipAddress);
    if (!original) goto end;

    char j;
    for (int i = 0;; i++) {
        const bool stop = (j = *(original + i)) == 0;

        copy = SDL_realloc(copy, (i + 1) * sizeof(char));
        copy[i] = j;

        if (stop) break;
    }
    end:

    SDL_UnlockMutex(mutex);
    return copy;
}

static void connectionProcessor(TCPsocket connection) {
    const int bufferSize = 1024;
    byte receiveBuffer[bufferSize] = {0}, sendBuffer[bufferSize] = {0};

    const IPaddress* ipAddress = SDLNet_TCP_GetPeerAddress(connection);
    const unsigned short port = ipAddress->port;
    char* host = resolveHost(ipAddress);

    do {
        SDL_memset(receiveBuffer, 0, bufferSize);
        SDL_memset(sendBuffer, 0, bufferSize);

        if (SDLNet_TCP_Recv(connection, receiveBuffer, bufferSize) != bufferSize) break;
        const int command = *asIntPtr(receiveBuffer);

        switch (command) {
            case 19:
                *asFloatPtr(sendBuffer) = t19(
                    *asFloatPtr(receiveBuffer + 4),
                    *asFloatPtr(receiveBuffer + 4 + 8),
                    *asFloatPtr(receiveBuffer + 4 + 8 * 2)
                );
                break;
            case 22:
                *asIntPtr(sendBuffer) = t22(
                    *asIntPtr(receiveBuffer + 4),
                    *asIntPtr(receiveBuffer + 4 * 2),
                    *asIntPtr(receiveBuffer + 4 * 3),
                    *asIntPtr(receiveBuffer + 4 * 4)
                );
                break;
            case 25:
                *asIntPtr(sendBuffer) = t25(*asIntPtr(receiveBuffer + 4));
                break;
            case 28:
                *asFloatPtr(sendBuffer) = t28(
                    *asFloatPtr(receiveBuffer + 4),
                    *asFloatPtr(receiveBuffer + 4 * 2)
                );
                break;
            case 3:
                *asFloatPtr(sendBuffer) = t3(
                    *asFloatPtr(receiveBuffer + 4),
                    *asFloatPtr(receiveBuffer + 4 * 2)
                );
                break;
            default:
                running = false;
                goto end;
        }

        SDL_Log("client %s:%u requested command %d", host, ipAddress->port, command);

        if (SDLNet_TCP_Send(connection, sendBuffer, bufferSize) != bufferSize) break;
    } while (running && SDL_memcmp(receiveBuffer, (byte[bufferSize]) {0}, bufferSize));
    end:

    SDLNet_TCP_Close(connection);

    SDL_Log("client %s:%u disconnected", host, port);
    SDL_free(host);
}

static unsigned long timeMillis(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) abort();
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int main(void) { // 19, 22, 25, 28, 3
    SDL_Init(0);
    SDLNet_Init();

    IPaddress ipAddress = {INADDR_NONE, SDL_Swap16(8080)};
    TCPsocket socket = SDLNet_TCP_Open(&ipAddress);
    if (!socket) abort();

    mutex = SDL_CreateMutex();

    SDL_Log("server started");

    SDL_Thread** threads = NULL;
    int threadsSize = 0;

    const unsigned long startMillis = timeMillis();
    int connectionId = 0;

    while (running) {
        if (timeMillis() - startMillis >= 10000u) {
            SDL_Log("timeout exceeded");
            break;
        }

        TCPsocket connection = SDLNet_TCP_Accept(socket);
        if (!connection) continue;

        const IPaddress* newIpAddress = SDLNet_TCP_GetPeerAddress(connection);
        char* host = resolveHost(newIpAddress);
        SDL_Log("client %s:%u connected", host, newIpAddress->port);
        SDL_free(host);

        const int nameSize = 10;
        char name[nameSize] = {0};
        SDL_snprintf(name, nameSize, "%d", connectionId++);

        threads = SDL_realloc(threads, ++threadsSize * sizeof(SDL_Thread*));
        threads[threadsSize - 1] = SDL_CreateThread((SDL_ThreadFunction) &connectionProcessor, name, connection);
    }

    for (int i = 0; i < threadsSize; SDL_WaitThread(threads[i++], NULL));
    SDL_free(threads);

    SDL_DestroyMutex(mutex);

    SDLNet_TCP_Close(socket);
    SDL_Log("server stopped");

    SDLNet_Quit();
    SDL_Quit();

    if (SDL_GetNumAllocations() > 0) abort();
    return EXIT_SUCCESS;
}
