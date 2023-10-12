
#include "util.h"

typedef enum {
    // exception number zero is invalid
    EXCEPT_ONE = 100,
    EXCEPT_TWO,
    EXCEPT_THREE,
    EXCEPT_FOUR,
} exceptions;

void func4() {

    printf("before raising 2\n");
    RAISE(0x8080, "this is exception two");
    printf("after raising 2\n");
}

void func3() {

    printf("before calling 4\n");
    func4();
    printf("after calling 4\n");
}

void func2() {

    printf("before calling 3\n");
    func3();
    printf("after calling 3\n");
}

void func1() {

    printf("before calling 2\n");
    func2();
    printf("after calling 2\n");
}

int main() {

    TRY {
        func1();
    }
    EXCEPT(EXCEPT_TWO) {
        printf("exception: %s\n", EXCEPTION_MSG);
    }
    /*
    ANY_EXCEPT() {
        printf("exception: 0x%04X: %s\n", EXCEPTION_NUM, EXCEPTION_MSG);
    }
    */
    FINAL

    printf("main() is about to exit.\n");

    return 0;
}
