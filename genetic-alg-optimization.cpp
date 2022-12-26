#include <iostream>
#include <array>
#include <vector>
#include <algorithm>

using namespace std;

struct point
{
public:
    double x, y;

    point(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
};

struct bounds
{
public:
    double xL, xR;
    double yL, yR;

    bounds(double xL, double xR, double yL, double yR)
    {
        this->xL = xL;
        this->xR = xR;
        this->yL = yL;
        this->yR = yR;
    }
};

double function(point p)
{
    double x = p.x, y = p.y;
    //return x + y;
    //return pow(x, 2) + pow(y, 2);
    return 2 * exp(-pow(x, 2) - pow(y, 2)) + 5 * exp(-pow(x - 3, 2) - pow(y - 3, 2));
}

struct compareByFuncLess
{
    bool operator() (const point& p1, const point& p2)
    {
        return function(p1) < function(p2);
    }
};

struct compareByFuncBigger
{
    bool operator() (const point& p1, const point& p2)
    {
        return function(p1) > function(p2);
    }
};


const int GROUP_COUNT = 25;
const int MAX_ITER = 100;
const bounds BOUNDS = bounds(-100, 100, -100, 100);


double dRand(double dMin, double dMax) // both inclusive
{
    double d = (double)rand() / RAND_MAX;
    return dMin + d * (dMax - dMin);
}

vector<array<point, 4>> getFirstGen()
{
    vector<array<point, 4>> groups(GROUP_COUNT, { point(0, 0), point(0, 0), point(0, 0), point(0, 0) });
    for (int grIdx = 0; grIdx < GROUP_COUNT; grIdx++)
        for (int indIdx = 0; indIdx < 4; indIdx++)
            groups[grIdx][indIdx] = point(dRand(BOUNDS.xL, BOUNDS.xR), dRand(BOUNDS.yL, BOUNDS.yR));

    return groups;
}

void mutate(array<point, 4>& group, double maxDelta)
{
    double tmp = group[1].y;
    group[0] = point(group[2].x + dRand(-maxDelta, maxDelta), group[3].y);
    group[1] = point(group[1].x + dRand(-maxDelta, maxDelta), group[3].y);
    group[2] = point(group[3].x, group[2].y + dRand(-maxDelta, maxDelta));
    group[3] = point(group[3].x, tmp + dRand(-maxDelta, maxDelta));
}

void clamp(array<point, 4>& group, bounds bounds)
{
    for (int i = 0; i < 4; i++)
    {
        if (group[i].x < bounds.xL)
            group[i].x = bounds.xL;
        else if (group[i].x > bounds.xR)
            group[i].x = bounds.xR;
        else if (group[i].y < bounds.yL)
            group[i].y = bounds.yL;
        else if (group[i].y > bounds.yR)
            group[i].y = bounds.yR;
    }
}

void tryFindExtremum(vector<array<point, 4>> groups, bool max)
{
    int bestIdx = max ? 3 : 0;
    vector<double> lastGroupBest(GROUP_COUNT, INT16_MAX);
    double curDelta = (BOUNDS.xR - BOUNDS.xL) / 2;
    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        for (int grIdx = 0; grIdx < GROUP_COUNT; grIdx++)
        {
            array<point, 4>& group = groups[grIdx];
            if (max)
                sort(group.begin(), group.end(), compareByFuncLess());
            else
                sort(group.begin(), group.end(), compareByFuncBigger());
            lastGroupBest[grIdx] = function(groups[grIdx][bestIdx]);
            mutate(group, curDelta);
            clamp(group, BOUNDS);
        }
        curDelta *= 0.8;
    }
    auto it = max ? max_element(lastGroupBest.begin(), lastGroupBest.end()) :
        min_element(lastGroupBest.begin(), lastGroupBest.end());
    auto idx = distance(lastGroupBest.begin(), it);

    point ind = groups[idx][bestIdx];
    const double e = pow(10, -5);
    if ((abs(ind.x - BOUNDS.xL) <= e || abs(ind.x - BOUNDS.xR) <= e) && 
        (abs(ind.y - BOUNDS.yL) <= e || abs(ind.y - BOUNDS.yR)))
        printf("%s: нет\n", max ? "MAX" : "MIN");
    else
        printf("%s: f(%f, %f)=%f\n", max ? "MAX" : "MIN", ind.x, ind.y, *it);
}

int main()
{
    setlocale(LC_ALL, "Russian");
    srand(time(NULL));
    
    auto groups = getFirstGen();
    tryFindExtremum(groups, true);
    tryFindExtremum(groups, false);
}
