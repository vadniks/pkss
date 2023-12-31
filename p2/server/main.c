
#pragma clang diagnostic ignored "-Wgnu-folding-constant"

#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#if !defined(__clang__) || !defined(__LITTLE_ENDIAN__) || __STDC_VERSION__ < 201112L
#   error "Unsupported configuration"
#endif

#define overloadable __attribute__((overloadable))

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
    return n <= 0 ? -1
        : n == (int) SDL_powf((float) 2, (float) a) ? a : -1;
}

static inline float t28(float x, float y) {
    return (2.0f * SDL_powf(x, 3.0f) - 4.0f * SDL_powf(x, 2.0f) + x + 1.0f)
        / (9 * SDL_powf(y, 3.0f) + y + 4) + (3 * SDL_powf(y, 2.0f) + 5 * y);
}

static inline float t3(float a, float b, float c) overloadable
{ return (2 * a - b - SDL_sinf(c)) / (5 + c); }

static inline float t3(float s, float t) overloadable // main
{ return t3(t, -1 * 2 * s, 1.17f) + t3(2.2f, t, s - t); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_Static_assert(sizeof(char) == 1 && sizeof(short) == 2 && sizeof(int) == 4 && sizeof(long) == 8 && sizeof(void*) == 8, "");

typedef unsigned char byte;

static _Atomic bool running = true;

static inline int* asIntPtr(void* x) { return ((int*) x); }
static inline float* asFloatPtr(void* x) { return ((float*) x); }

static SDL_mutex* mutex = NULL;

static char* _Nullable resolveHost(const IPaddress* ipAddress) { // nullable
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
                    *asFloatPtr(receiveBuffer + 4 * 2),
                    *asFloatPtr(receiveBuffer + 4 * 3)
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
            case 0:
                running = false;
                SDL_Log("stop command received from client %s:%u", host, ipAddress->port);
                goto end;
            default:
                continue;
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

static unsigned timerCallback(__attribute_maybe_unused__ unsigned _, const void* parameter) {
    if (!running) return 0;
    SDL_Log("\tClients count: %d", *((const int*) parameter));
    return running ? 1000 : 0;
}

static void signalHandler(int signal) {
    SDL_Log("received stopping signal (%d)", signal);
    running = false;
}

int main(void) { // 19, 22, 25, 28, 3
    signal(SIGTERM, &signalHandler);
    signal(SIGINT, &signalHandler);

    SDL_Init(SDL_INIT_TIMER);
    SDLNet_Init();

    IPaddress ipAddress = {INADDR_NONE, SDL_Swap16(8080)};
    TCPsocket socket = SDLNet_TCP_Open(&ipAddress);
    if (!socket) abort();

    mutex = SDL_CreateMutex();

    SDL_Log("server started");

    SDL_Thread** threads = NULL;
    _Atomic int threadsSize = 0;

    const SDL_TimerID timerId = SDL_AddTimer(1000, (SDL_TimerCallback) &timerCallback, &threadsSize);

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

    SDL_RemoveTimer(timerId);

    SDL_DestroyMutex(mutex);

    SDLNet_TCP_Close(socket);
    SDL_Log("server stopped");

    SDLNet_Quit();
    SDL_Quit();

    if (SDL_GetNumAllocations() > 0) abort();
    return EXIT_SUCCESS;
}
