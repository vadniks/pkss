
#pragma clang diagnostic ignored "-Wgnu-folding-constant"

#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define nullable _Nullable

static bool t19(float a, float b, float c, float d)
{ return a * b <= c * d && a <= c && b <= d; }

static void** nullable t22(float a, float b, float c, float d) {
    float values[4] = {a, b, c, d};
    float* values2 = NULL;
    int size = 0;

    for (int i = 0; i < 4; i++)
        if (values[i] < 3 || values[i] > 15)
            (values2 = SDL_realloc(values2, ++size * sizeof(float))) && (values2[size - 1] = values[i]);

    void** result = SDL_malloc(2 * sizeof(void*));
    result[0] = values2;
    *((int*) result[1]) = size;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum : int {
    PAGE_ERROR = 0,
    PAGE_MAIN = -1,
    PAGE_T19 = 19,
    PAGE_T22 = 22,
    PAGE_T25 = 25,
    PAGE_T28 = 28,
    PAGE_T3 = 3
} Pages;

static Pages gPage = PAGE_MAIN;

static SDL_Window* gWindow = NULL;
static SDL_Renderer* gRenderer = NULL;
static TTF_Font* gFont = NULL;

static int gWidth = 0, gHeight = 0;

static const int gBufferSize = 255;
static char gBuffer[gBufferSize];
static int gBufferPosition = 0;

static int gTaskNumber = 0;

static int gMouseHoverX = 0, gMouseHoverY = 0, gMouseClickX = 0, gMouseClickY = 0;
static bool gMouseClicked = false;

static const int TEXT_HEIGHT = 25, BUTTON_INNER_MARGIN = 5, BUTTON_SIZE = TEXT_HEIGHT + BUTTON_INNER_MARGIN * 2;

static float t19Values[4] = {0};
static int t19Field = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void clearBuffer(void) {
    gBufferPosition = 0;
    SDL_memset(gBuffer, 0, gBufferSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

static inline SDL_Color colorWhite(void) { return (SDL_Color) {255, 255, 255, 200}; }
static inline SDL_Color colorGray(void) { return (SDL_Color) {255, 255, 255, 100}; }
static inline SDL_Color colorBlack(void) { return (SDL_Color) {0, 0, 0, 0}; }
static inline SDL_Color colorDefault(void) { return (SDL_Color) {30, 34, 41, 0}; }
static inline SDL_Color colorRed(void) { return (SDL_Color) {255, 75, 75, 0}; }

static inline SDL_Rect rectOf(int x, int y, int w, int h) { return (SDL_Rect) {x, y, w, h}; }

static void drawCenteredXText(int y, const char* text, SDL_Color color) {
    const int length = (int) SDL_strlen(text) * 10;
    drawText(rectOf(gWidth / 2 - length / 2, y, length, TEXT_HEIGHT), text, color);
}

static void drawTextAutoSized(int x, int y, const char* text, SDL_Color color) {
    const int length = (int) SDL_strlen(text) * 10;
    drawText(rectOf(x, y, length, TEXT_HEIGHT), text, color);
}

static inline void drawCenteredXYText(const char* text, SDL_Color color)
{ drawCenteredXText(gHeight / 2 - TEXT_HEIGHT / 2, text, color); }

static inline void setRendererDrawColor(SDL_Color color)
{ SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a); }

static SDL_Color getRendererDrawColor(void) {
    SDL_Color color;
    SDL_GetRenderDrawColor(gRenderer, &(color.r), &(color.g), &(color.b), &(color.a));
    return color;
}

static void drawButton(SDL_Rect rect, const char* text, SDL_Color color) {
    if (rect.x < 10 || rect.y < 10) abort();

    const SDL_Color previousColor = getRendererDrawColor();
    setRendererDrawColor(color);

    SDL_RenderDrawRect(gRenderer, &rect);

    const SDL_Rect innerRect = {
        rect.x + BUTTON_INNER_MARGIN,
        rect.y + BUTTON_INNER_MARGIN,
        rect.w - BUTTON_INNER_MARGIN * 2,
        rect.h - BUTTON_INNER_MARGIN * 2
    };
    drawText(innerRect, text, color);

    setRendererDrawColor(previousColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void drawErrorPage(void) { drawCenteredXYText("An error has occurred!", colorRed()); }

static void drawMainPage(void) {
    drawCenteredXText(10, "Main page", colorWhite());

    gTaskNumber = SDL_atoi(gBuffer);

    const int taskNumberTextSize = 255;
    char taskNumberText[taskNumberTextSize] = {0};
    SDL_strlcat(taskNumberText, "Enter task number via keyboard: ", 33);
    SDL_itoa(gTaskNumber, taskNumberText + SDL_strlen(taskNumberText), 10);

    drawCenteredXYText(taskNumberText, colorGray());

    if (gTaskNumber == PAGE_T19
        || gTaskNumber == PAGE_T22
        || gTaskNumber == PAGE_T25
        || gTaskNumber == PAGE_T28
        || gTaskNumber == PAGE_T3) {
        clearBuffer();
        gPage = (Pages) gTaskNumber;
    } else
        drawCenteredXText(gHeight - 10 - TEXT_HEIGHT, "Unknown task number", colorRed());
}

static void drawT19(void) {
    drawCenteredXText(10, "Task 19", colorWhite());
    drawCenteredXText(10 * 2 + TEXT_HEIGHT, "Enter values:", colorGray());

    if (t19Field >= 0 && t19Field < 4)
        t19Values[t19Field] = (float) SDL_atof(gBuffer);

    for (int i = 1; i <= 4; i++) {
        const int valueTextSize = 1 << 5;
        char valueText[valueTextSize] = {0};
        SDL_snprintf(valueText, valueTextSize, "Value %c: %f", 'A' + i - 1, t19Values[i - 1]);
        drawTextAutoSized(10, 10 * (2 + i) + TEXT_HEIGHT * (1 + i), valueText, t19Field == i - 1 ? colorWhite() : colorGray());
    }

    const int buttonWidth = 80;
    SDL_Rect buttonRect = {gWidth / 2 - buttonWidth / 2, gHeight - 10 * 3 - BUTTON_SIZE, buttonWidth, TEXT_HEIGHT};

    const bool withinButton = gMouseHoverX >= buttonRect.x && gMouseHoverX <= buttonRect.x + buttonRect.w
        && gMouseHoverY >= buttonRect.y && gMouseHoverY <= buttonRect.y + buttonRect.h;

    drawButton(buttonRect, "Next", withinButton ? colorWhite() : colorGray());

    if (gMouseClicked && withinButton) {
        t19Field < 4 ? t19Field++ : (t19Field = 0);
        clearBuffer();
        gMouseClicked = false;
    }

    const int resultTextSize = 1 << 5;
    char* resultText = SDL_malloc(resultTextSize);

    if (t19Field == 4)
        SDL_snprintf(resultText, resultTextSize, "Result: %s",
             t19(t19Values[0], t19Values[1], t19Values[2], t19Values[3]) ? "true" : "false");
    else
        SDL_memcpy(resultText, "Result: waiting for input...", resultTextSize);

    drawCenteredXText(gHeight - 10 - TEXT_HEIGHT, resultText, t19Field == 4 ? colorWhite() : colorGray());
    SDL_free(resultText);
}

static void drawPage(void) {
    setRendererDrawColor(colorDefault());
    SDL_RenderClear(gRenderer);

    switch (gPage) {
        case PAGE_ERROR:
            drawErrorPage();
            break;
        case PAGE_MAIN:
            drawMainPage();
            break;
        case PAGE_T19:
            drawT19();
            break;
        case PAGE_T22:
            break;
        case PAGE_T25:
            break;
        case PAGE_T28:
            break;
        case PAGE_T3:
            break;
    }

    SDL_RenderPresent(gRenderer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void keyPressed(const SDL_Event* event) {
    const SDL_KeyCode keyCode = event->key.keysym.sym;
    switch (keyCode) {
        case SDLK_BACKSPACE:
            gBuffer[gBufferPosition > 0 ? gBufferPosition-- : 0] = 0;
            break;
        case SDLK_RETURN:
            // TODO
            break;
        default:
            if ((keyCode >= SDLK_0 && keyCode <= SDLK_9
                || keyCode >= SDLK_a && keyCode <= SDLK_z
                || keyCode == SDLK_PERIOD)
                && gBufferPosition < gBufferSize)
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

    gFont = TTF_OpenFont("RobotoMono-Regular.ttf", 100);

    while (true) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    goto end;
                case SDL_KEYDOWN:
                    keyPressed(&event);
                    break;
                case SDL_MOUSEMOTION:
                    SDL_GetMouseState(&gMouseHoverX, &gMouseHoverY);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    gMouseClicked = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    SDL_GetMouseState(&gMouseClickX, &gMouseClickY);
                    gMouseClicked = false;
                    break;
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
