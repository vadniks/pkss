
#pragma clang diagnostic ignored "-Wgnu-folding-constant"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
//#include <form.h>

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
static unsigned gFieldsState = 0;
static const unsigned gLargeBufferSize = 0xff;

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
    static char edit1[gBufferSize], edit2[gBufferSize];
    static unsigned edit1Position, edit2Position;

    if (edit1Position > gBufferSize || edit2Position > gBufferSize) {
        memset(edit1, 0, gBufferSize);
        memset(edit2, 0, gBufferSize);
        edit1Position = 0;
        edit2Position = 0;
    }

    char field1[gLargeBufferSize] = { 0 };
    const int field1Size = snprintf(field1, gLargeBufferSize, "Enter A and B (in form of 1,2): "
                                                              "%.*s", edit1Position, edit1);
    const int field1Y = height / 3 - 1, field1X = width / 3;
    move(field1Y, field1X);
    mvprintw(field1Y, field1X, "%s", field1);
    refresh();

    char field2[gLargeBufferSize] = { 0 };
    const int field2Size = snprintf(field2, gLargeBufferSize, "Enter C and D (in form of 1,2): "
                                                              "%.*s", edit2Position, edit2);
    const int field2Y = height - height / 3, field2X = width / 3;
    mvprintw(field2Y, field2X, "%s", field2);
    refresh();

    if (gFieldsState == 0) {
        memcpy(edit1, gBuffer, gBufferSize);
        edit1Position = gBufferPosition;
        move(field1Y, field1X + field1Size);
    } else if (gFieldsState == 1) {
        memcpy(edit2, gBuffer, gBufferSize);
        edit2Position = gBufferPosition;
        move(field2Y, field2X + field2Size);
    } else if (gFieldsState == 2) {
        mvprintw(height / 2 - 1, width / 3 - 3, "Result: %f", t19(1, 2, 3, 4));
    } else
        gFieldsState = 0;
}

static void errorScreen(void) {
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    const char* title = "An error has occurred, press q to exit";
    mvprintw(height / 2 - 1, width / 2 - strlen(title) / 2, "%s", title);

    attroff(COLOR_PAIR(1));
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

static void enterPressed(void) {
    if (gScreen == SCREEN_HOME)
        gScreen = (Screens) atoi(gBuffer);
    else
        gFieldsState++;

    memset(gBuffer, 0, gBufferSize);
    gBufferPosition = 0;
}

int main(void) {
    initscr();
    noecho();

    int input;
    do {
        draw();

        if ((input = getch()) == 127)
            gBuffer[gBufferPosition ? gBufferPosition-- : 0] = 0;
        else if (input == 10)
            enterPressed();
        else if (input >= '0' && input <= '9' || input >= 'a' && input <= 'z')
            gBuffer[gBufferPosition < gBufferSize ? gBufferPosition++ : gBufferPosition] = (char) input;
        else
            exit(input);

    } while (input != 'q');

    endwin();
    return EXIT_SUCCESS;
}
