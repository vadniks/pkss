
#pragma clang diagnostic ignored "-Wgnu-folding-constant"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

typedef enum : unsigned {
    SCREEN_HOME = 255,
    SCREEN_T19 = 19,
    SCREEN_T22 = 22,
    SCREEN_T25 = 25,
    SCREEN_T28 = 28,
    SCREEN_T3 = 3
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

static void drawScreen(void (*screen)(void)) {
    clear();
    box(stdscr, '|', '-');
    refresh();

    (*screen)();
}

static void homeScreen(void) {
    const char* title = "Enter task number or q to exit";

    gCursorX = width / 2 - strlen(title) / 2;
    gCursorY = height / 2 - 1;

    move(gCursorY, gCursorX);
    printw("%s: %.*s", title, gBufferPosition, gBuffer);

    refresh();
}

static void t19Screen(void) {

}

static void errorScreen(void) {
    const char* title = "An error has occurred, press q to exit";
    mvprintw(height / 2 - 1, width / 2 - strlen(title) / 2, "%s", title);
    refresh();
}

static void draw(void) { switch (gScreen) {
    case SCREEN_HOME: drawScreen(&homeScreen); break;
    case SCREEN_T19: drawScreen(&t19Screen); break;
    case SCREEN_T22: break;
    case SCREEN_T25: break;
    case SCREEN_T28: break;
    case SCREEN_T3: break;
    default: drawScreen(&errorScreen); break;
} }

int main(void) {
    initscr();
    noecho();

    int input;
    do {
        draw();

        if ((input = getch()) == 127)
            gBuffer[gBufferPosition ? gBufferPosition-- : 0] = 0;
        else if (input == '\n')
            gScreen = (Screens) atoi(gBuffer);
        else if (input >= '0' && input <= '9' || input >= 'a' && input <= 'z')
            gBuffer[gBufferPosition < gBufferSize ? gBufferPosition++ : gBufferPosition] = (char) input;
        else
            abort();

    } while (input != 'q');

    endwin();
    return EXIT_SUCCESS;
}
