
#pragma clang diagnostic ignored "-Wgnu-folding-constant"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

typedef enum {
    SCREEN_HOME,
    SCREEN_T19,
    SCREEN_T22,
    SCREEN_T25,
    SCREEN_T28,
    SCREEN_T3
} Screens;

static Screens gScreen = SCREEN_HOME;
static const unsigned gBufferSize = 5;
static char gBuffer[gBufferSize] = { 0 };
static unsigned gBufferPosition = 0;
static unsigned gCursorX = 0;
static unsigned gCursorY = 0;

//static float maxf(float a, float b)
//{ return a > b ? a : b; }

static bool t19(float a, float b, float c, float d)
{ return a * b <= c * d && a <= c && b <= d; }

#define width (stdscr->_maxx)
#define height (stdscr->_maxy)

static void homeScreen(void) {
    clear();
    box(stdscr, '|', '-');
    refresh();

    const char* title = "Enter task number or q to exit";

    gCursorX = width / 2 - strnlen(title, 0xff) / 2;
    gCursorY = height / 2 - 1;

    move(gCursorY, gCursorX);
    printw("%s: %.*s", title, gBufferPosition, gBuffer);

    refresh();
}

static void draw(void) { switch (gScreen) {
    case SCREEN_HOME: homeScreen(); break;
    case SCREEN_T19: break;
    case SCREEN_T22: break;
    case SCREEN_T25: break;
    case SCREEN_T28: break;
    case SCREEN_T3: break;
    default: abort(); break;
} }

int main(void) {
    initscr();
    noecho();

    int input;
    do {
        draw();

        if ((input = wgetch(stdscr)) == KEY_BACKSPACE || input == 127) {
//            memset(gBuffer, 0, gBufferSize);
            gBuffer[gBufferPosition ? gBufferPosition-- : 0] = 0;
//            move(gCursorY, gCursorX);
//            refresh();
//            memmove(gBuffer, gBuffer, gBufferPosition ? --gBufferPosition : 0);
        } else if (input >= '0' && input <= '9' || input >= 'a' && input <= 'z')
            gBuffer[gBufferPosition < gBufferSize ? gBufferPosition++ : gBufferPosition] = (char) input;

    } while (input != 'q');

    endwin();
    return EXIT_SUCCESS; // 19, 22, 25, 28, 3
}
