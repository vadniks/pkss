
#pragma once

#include <cmath>
#include <QList>

class Tasks final { public:
    static float t19(int n) {
        if (n < 0) return NAN;
        for (float i = 0, j = 0; i <= n; j = sinf(tanf(i / 2.0f)), i += 1.0f)
            if (j < 0) return j;
        return NAN;
    }

    static float t22(float n) {
        if (n < 0) return NAN;
        int k = 0;
        for (int i = 0, j; (j = powf(3, i)) < n; k = j, i++);
        return k;
    }

    static QList<float> t25(float a) {
        QList<float> list;
        for (float i = -2.75f; i < 8.0f; list.push_back((4 * i - 3 * i + tanf(i)) / a), i += 0.75f);
        return list;
    }

    static QList<float> t28() {
        // TODO
    }
};
