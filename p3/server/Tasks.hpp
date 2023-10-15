
#pragma once

#pragma ide diagnostic ignored "cert-flp30-c"

#include <cmath>
#include <QtCore/QList>
#include <QtCore/QDebug>

class Tasks final { public:

    [[deprecated("test only")]] Tasks() {
        qInfo() << Tasks::t19(5);
        qInfo() << Tasks::t22(10);
        qInfo() << Tasks::t25(1.0f);
        qInfo() << Tasks::t28();
        qInfo() << Tasks::t3({1.5f, -2.1f, 3.0f, -4.0f, 5.0f});
    }

    static float t19(int n) {
        if (n < 0) return NAN;
        for (float i = 0, j = 0; i <= (float) n; j = sinf(tanf(i / 2.0f)), i += 1.0f)
            if (j < 0) return j;
        return NAN;
    }

    static float t22(float n) {
        if (n < 0) return NAN;
        float k = 0;
        for (float i = 0, j; (j = powf(3, i)) < n; k = j, i += 1.0f);
        return k;
    }

    static QList<float> t25(float a) {
        QList<float> list;
        for (float i = -2.75f; i < 8.0f; list.append((4 * i - 3 * i + tanf(i)) / a), i += 0.75f);
        return list;
    }

    static QList<float> t28() {
        QList<float> list;
        for (float i = 5; i >= -12; list.append(sinf(i) - 5.0f * cosf(i - 2.0f)), i -= 1.2f);
        return list;
    }

    static QList<float> t3(const QList<float>& input) {
        QList<float> output;
        for (float i : input)
            output.append(fabsf(i));
        return output;
    }
};
