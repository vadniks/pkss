
#pragma clang diagnostic ignored "-Wgnu-folding-constant"

#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifndef __LINUX__
#   warning "Tested only on linux"
#endif

static inline bool t19(float a, float b, float c, float d)
{ return a * b <= c * d && a <= c && b <= d; }

static void** t22(float a, float b, float c, float d) {
    float values[4] = {a, b, c, d};
    float* values2 = NULL;
    int size = 0;

    for (int i = 0; i < 4; i++)
        if (values[i] < 3 || values[i] > 15)
            (values2 = SDL_realloc(values2, ++size * sizeof(float))) && (values2[size - 1] = values[i]);

    void** result = SDL_malloc(2 * sizeof(void*));
    result[0] = values2;

    result[1] = SDL_malloc(sizeof(int));
    *((int*) result[1]) = size;

    return result;
}

static float t25(float a, float b, float c) {
    if (a + b <= c || a + c <= b || b + c <= a)
        return -1;

    const float s = (a + b + c) / 2.0f;
        return sqrtf(s * (s - a) * (s - b) * (s - c));
}

static inline float t28(float x, float y) { return asinf(x + y); }

static inline bool t3(float a, float b, float c, float x, float y) {
#   define cmp(aa, bb) aa < x && bb < y
    return cmp(a, b) || cmp(b, a) || cmp(a, c) || cmp(c, a) || cmp(b, c) || cmp(c, b);
#   undef cmp
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

#define VALUES_AND_FIELD(n, c) \
    static float t ## n ## Values[c] = {0}; \
    static int t ## n ## Field = 0;

VALUES_AND_FIELD(19, 4)
VALUES_AND_FIELD(22, 4)
VALUES_AND_FIELD(25, 3)
VALUES_AND_FIELD(28, 2)
VALUES_AND_FIELD(3, 5)

const char* WAITING_FOR_INPUT = "Result: waiting for input...";

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

static bool drawButtonAndCheckHover(const char* name, SDL_Rect rect) {
    const bool withinButton = gMouseHoverX >= rect.x && gMouseHoverX <= rect.x + rect.w
        && gMouseHoverY >= rect.y && gMouseHoverY <= rect.y + rect.h;

    drawButton(rect, name, withinButton ? colorWhite() : colorGray());
    return withinButton;
}

static void drawTemplate(int fields, const char* title, int* chosenField, float* values, char (*labelSupplier)(int)) { // TODO: add 'back' button which returns to main page
    drawCenteredXText(10, title, colorWhite());
    drawCenteredXText(10 * 2 + TEXT_HEIGHT, "Enter values:", colorGray());

    if (*chosenField >= 0 && *chosenField < fields)
        values[*chosenField] = (float) SDL_atof(gBuffer);

    for (int i = 1; i <= fields; i++) {
        const int valueTextSize = 1 << 5;
        char valueText[valueTextSize] = {0};
        SDL_snprintf(valueText, valueTextSize, "Value %c: %f", (*labelSupplier)(i - 1), values[i - 1]);
        drawTextAutoSized(10, 10 * (2 + i) + TEXT_HEIGHT * (1 + i), valueText, *chosenField == i - 1 ? colorWhite() : colorGray());
    }

    const int buttonWidth = 80;
    const bool withinButtonNext = drawButtonAndCheckHover("Next", (SDL_Rect) {
        gWidth / 4,
        gHeight - 10 * 3 - BUTTON_SIZE,
        buttonWidth,
        TEXT_HEIGHT
    });

    const bool withinButtonBack = drawButtonAndCheckHover("Back", (SDL_Rect) {
        gWidth - gWidth / 4 - buttonWidth,
        gHeight - 10 * 3 - BUTTON_SIZE,
        buttonWidth,
        TEXT_HEIGHT
    });

    if (gMouseClicked && withinButtonNext) {
        *chosenField < fields ? (*chosenField)++ : (*chosenField = 0);
        clearBuffer();
        gMouseClicked = false;
    }

    if (gMouseClicked && withinButtonBack) {
        clearBuffer();
        gMouseClicked = false;
        gPage = PAGE_MAIN;
    }
}

static char defaultLabelSupplier(int i) { return (char) ('A' + i); }

static void drawT19Page(void) {
    drawTemplate(4, "Task 19", &t19Field, t19Values, &defaultLabelSupplier);
    const int resultTextSize = 1 << 5;
    char* resultText = SDL_calloc(resultTextSize, 1);

    if (t19Field == 4)
        SDL_snprintf(resultText, resultTextSize, "Result: %s",
            t19(t19Values[0], t19Values[1], t19Values[2], t19Values[3]) ? "true" : "false");
    else
        SDL_memcpy(resultText, WAITING_FOR_INPUT, resultTextSize);

    drawCenteredXText(gHeight - 10 - TEXT_HEIGHT, resultText, t19Field == 4 ? colorWhite() : colorGray());
    SDL_free(resultText);
}

static void drawT22Page(void) {
    drawTemplate(4, "Task 22", &t22Field, t22Values, &defaultLabelSupplier);
    const int resultTextSize = 255;
    char* resultText = SDL_calloc(resultTextSize, 1);

    if (t22Field == 4) {
        void** result = t22(t22Values[0], t22Values[1], t22Values[2], t22Values[3]);
        float* resultArray = result[0];
        const int resultSize = *((int*) result[1]);

        int offset = 0;

        for (int i = 0; i < resultSize; i++) {
            const int numberSize = 6;
            char number[numberSize] = {0};

            const int length = SDL_snprintf(number, numberSize, "%1.2f,", resultArray[i]);
            SDL_memcpy(resultText + offset, number, numberSize);

            offset += length;
        }

        SDL_free(result[1]);
        SDL_free(resultArray);
        SDL_free(result);
    } else
        SDL_memcpy(resultText, WAITING_FOR_INPUT, 29);

    drawCenteredXText(gHeight - 10 - TEXT_HEIGHT, resultText, t22Field == 4 ? colorWhite() : colorGray());
    SDL_free(resultText);
}

static void drawSimpleFloatOutput(bool showResult, float result) {
    if (!showResult) {
        drawCenteredXText(gHeight - 10 - TEXT_HEIGHT, WAITING_FOR_INPUT, colorGray());
        return;
    }

    char resultText[15];
    SDL_snprintf(resultText, sizeof resultText, "Result: %f", result);
    drawCenteredXText(gHeight - 10 - TEXT_HEIGHT, resultText, colorWhite());
}

static void drawT25Page(void) {
    drawTemplate(3, "Task 25", &t25Field, t25Values, &defaultLabelSupplier);
    drawSimpleFloatOutput(t25Field == 3, t25(t25Values[0], t25Values[1], t25Values[2]));
}

static char t28LabelSupplier(int i) { return (char) ('X' + i); }

static void drawT28Page(void) {
    drawTemplate(2, "Task 28", &t28Field, t28Values, &t28LabelSupplier);
    drawSimpleFloatOutput(t28Field == 2, t28(t28Values[0], t28Values[1]));
}

static char t3LabelSupplier(int i) { return (char) (i < 3 ? ('A' + i) : ('X' + i - 3)); }

static void drawT3Page(void) {
    drawTemplate(5, "Task 3", &t3Field, t3Values, &t3LabelSupplier);
    const bool showResult = t3Field == 5;

    const char* resultText = showResult
        ? t3(t3Values[0], t3Values[1], t3Values[2], t3Values[3], t3Values[4]) ? "Result: true" : "Result: false"
        : WAITING_FOR_INPUT;

    drawCenteredXText(gHeight - 10 - TEXT_HEIGHT, resultText, showResult ? colorWhite() : colorGray());
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
            drawT19Page();
            break;
        case PAGE_T22:
            drawT22Page();
            break;
        case PAGE_T25:
            drawT25Page();
            break;
        case PAGE_T28:
            drawT28Page();
            break;
        case PAGE_T3:
            drawT3Page();
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

    if (SDL_GetNumAllocations() > 1) abort();

    return 0;
}
