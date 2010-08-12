#ifndef CVTTEST_H
#define CVTTEST_H

typedef bool (*CVTTestFunc)( void );

typedef struct {
    CVTTestFunc func;
    const char* name;
} CVTTest;


#define BEGIN_CVTTEST(x) extern "C" { bool x##_test( void ) {
#define END_CVTTEST } }

#endif
