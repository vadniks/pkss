
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
        edit1Position = 0;
        edit2Position = 0;
    }

    const char* field1 = "Enter A and B in this form: 1,2";
    const int field1Y = height / 3 - 1, field1X = width / 3 - strlen(field1) / 2;
    mvprintw(field1Y, field1X, "%s\t%.*s", field1, edit1Position, edit1);
    refresh();

    const char* field2 = "Enter C and D in this form: 1,2";
    const int field2Y = height - height / 3, field2X = width / 3 - strlen(field2) / 2;
    mvprintw(field2Y, field2X, "%s\t%.*s", field2, edit2Position, edit2);
    refresh();

    if (gFieldsState == 0) {
        move(field1Y, field1X);
        memcpy(edit1, gBuffer, gBufferSize);
        edit1Position = gBufferPosition;
    } else if (gFieldsState == 1) {
        move(field2Y, field2X);
        memcpy(edit2, gBuffer, gBufferSize);
        edit2Position = gBufferPosition;
    } else {
        gFieldsState = 0;
        mvprintw(height / 2 - 1, width / 3 - 3, "%s", "aaa");
    }
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
    else {
        gFieldsState++;
        memset(gBuffer, 0, gBufferSize);
        gBufferPosition = 0;
    }
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
            abort();

    } while (input != 'q');

    endwin();
    return EXIT_SUCCESS;
}
