
#include <math.h>
#include "vectmath.h"

struct dvect3 dvect3_direct(double x, double y, double z)
{
    struct dvect3 res;

    res.x = x;
    res.y = y;
    res.z = z;

    return res;
}

struct dvect3 dvect3_null(void)
{
    return (struct dvect3) {0.0, 0.0, 0.0};
}

bool dvect3_isnull(struct dvect3 v)
{
    return (v.x == 0 && v.y == 0 && v.z == 0);
}

bool dvect3_isequal(struct dvect3 v1, struct dvect3 v2)
{
    return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
}

struct dvect3 dvect3_add(struct dvect3 v1, struct dvect3 v2)
{
    struct dvect3 res;

    res.x = v1.x + v2.x;
    res.y = v1.y + v2.y;
    res.z = v1.z + v2.z;

    return res;
}

struct dvect3 dvect3_sub(struct dvect3 left, struct dvect3 right)
{
    struct dvect3 res;

    res.x = left.x - right.x;
    res.y = left.y - right.y;
    res.z = left.z - right.z;

    return res;
}

double dvect3_inmul(struct dvect3 v1, struct dvect3 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

struct dvect3 dvect3_smul(double s, struct dvect3 v)
{
    struct dvect3 res;

    res.x = s * v.x;
    res.y = s * v.y;
    res.z = s * v.z;

    return res;
}

struct dvect3 dvect3_exmul(struct dvect3 v1, struct dvect3 v2)
{
    struct dvect3 res;

    res.x = (v1.y * v2.z) - (v1.z * v2.y);
    res.y = (v1.z * v2.x) - (v1.x * v2.z);
    res.z = (v1.x * v2.y) - (v1.y * v2.x);

    return res;
}

double dvect3_length(struct dvect3 v)
{
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

struct dvect3 dvect3_normalize(struct dvect3 v)
{
    return dvect3_smul(1 / dvect3_length(v), v);
}