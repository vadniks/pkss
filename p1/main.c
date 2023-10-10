
#pragma clang diagnostic ignored "-Wgnu-folding-constant"

#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

static bool t19(float a, float b, float c, float d)
{ return a * b <= c * d && a <= c && b <= d; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum : int {
    PAGE_ERROR = 0,
    PAGE_MAIN = 1,
    PAGE_T19 = 2
} Pages;

static Pages gPage = PAGE_MAIN;

static SDL_Window* gWindow = NULL;
static SDL_Renderer* gRenderer = NULL;
static TTF_Font* gFont = NULL;

static int gWidth = 0, gHeight = 0;

static const int gBufferSize = 255;
static char gBuffer[gBufferSize];
static int gBufferPosition = 0;

static SDL_Texture* makeTextTexture(const char* text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(gFont, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

static void drawText(SDL_Rect rect, const char* text, SDL_Color color) {
    SDL_Texture* texture = makeTextTexture(text, color);
    SDL_RenderCopy(gRenderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

static inline SDL_Color colorWhite(void) { return (SDL_Color) {255, 255, 255, 255}; }
static inline SDL_Color colorBlack(void) { return (SDL_Color) {0, 0, 0, 0}; }
static inline SDL_Color colorDefault(void) { return (SDL_Color) {30, 34, 41, 0}; }

static void drawMainPage(void) {
    drawText((SDL_Rect) {10, 10, 500, 100}, "Hello World!", colorWhite());
}

static void drawPage(void) {
    SDL_Color color = colorDefault();
    SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);

    SDL_RenderClear(gRenderer);

    switch (gPage) {
        case PAGE_MAIN:
            drawMainPage();
            break;
    }

    SDL_RenderPresent(gRenderer);
}

static void keyPressed(const SDL_Event* event) {
    const SDL_KeyCode keyCode = event->key.keysym.sym;
    switch (keyCode) {
        case SDLK_BACKSPACE:
            if (gBufferPosition > 0)
                gBuffer[gBufferPosition--] = 0;
            break;
        case SDLK_RETURN:
            // TODO
            break;
        default:
            if (
                (keyCode >= SDLK_0 && keyCode <= SDLK_9)
                || (keyCode >= SDLK_a && keyCode <= SDLK_z)
                && gBufferPosition < gBufferSize
            )
                gBuffer[gBufferPosition++] = (char) keyCode;
            break;
    }
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) abort();
    TTF_Init();

    gWindow = SDL_CreateWindow(
        "pkss 1",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        16 * 50,
        9 * 50,
        SDL_WINDOW_SHOWN
    );
    if (!gWindow) abort();

    gRenderer = SDL_CreateRenderer(
        gWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!gRenderer) abort();

    SDL_GetRendererOutputSize(gRenderer, &gWidth, &gHeight);

    gFont = TTF_OpenFont("RobotoMono-Regular.ttf", 500);

    while (true) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: goto end;
                case SDL_KEYDOWN: keyPressed(&event); break;
                default: break;
            }
        }

        drawPage();
        nanosleep(&((struct timespec) {
            0,
            (int) 1e3f / 60 * (int) 1e6f
        }), NULL);
    }

    end:
    TTF_CloseFont(gFont);

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    TTF_Quit();
    SDL_Quit();
    return 0;
}
