

#ifndef __VECTMATH_H
#define __VECTMATH_H

struct dvect3
{
    double x;
    double y;
    double z;
};

struct dvect3 dvect3_direct(double x, double y, double z);
struct dvect3 dvect3_null(void);

bool dvect3_isnull(struct dvect3 v);
bool dvect3_isequal(struct dvect3 v1, struct dvect3 v2);

struct dvect3 dvect3_add(struct dvect3 v1, struct dvect3 v2);
struct dvect3 dvect3_sub(struct dvect3 left, struct dvect3 right);

double dvect3_inmul(struct dvect3 v1, struct dvect3 v2);
struct dvect3 dvect3_smul(double s, struct dvect3 v);
struct dvect3 dvect3_exmul(struct dvect3 v1, struct dvect3 v2);

double dvect3_length(struct dvect3 v);
struct dvect3 dvect3_normalize(struct dvect3 v);

#endif