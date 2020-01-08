#include "monitor.h"
#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/select.h>

using namespace std;

void *producentA(void *ptr);

void *producentB(void *ptr);

void *producentC(void *ptr);

void *specProducent(void *ptr);

void *KonsA(void *ptr);

void *KonsB(void *ptr);

void *KonsC(void *ptr);


MyQueue *bufA, *bufB, *bufC;
double timeA, timeB, timeC;
int pr = 30;

int main() {
    pthread_t threadProducentA;
    pthread_t threadProducentB;
    pthread_t threadProducentC;
    //pthread_t threadProducentSpec;
    pthread_t threadKonsA;
    pthread_t threadKonsB;
    pthread_t threadKonsC;


    srand(time(0));

    bufA = new MyQueue();
    bufB = new MyQueue();
    bufC = new MyQueue();

    pthread_create(&threadProducentA, NULL, producentA, NULL);
    pthread_create(&threadProducentB, NULL, producentB, NULL);
    pthread_create(&threadProducentC, NULL, producentC, NULL);
    //pthread_create(&threadProducentSpec, NULL, specProducent, NULL);
    pthread_create(&threadKonsA, NULL, KonsA, NULL);
    pthread_create(&threadKonsB, NULL, KonsB, NULL);
    pthread_create(&threadKonsC, NULL, KonsC, NULL);


    while (1) {
        /*countA += bufA->getSize();
        countB += bufB->getSize();
        countC += bufC->getSize();
        ++n;*/
        if (pthread_kill(threadProducentA, 0) != 0) break; //threadA finished
    }

    cout << "koniec: " << endl;
    cout << "koniec: " << endl;
    cout << "koniec: " << endl;
    return 0;
}

void *producentA(void *ptr) {
    Message toInsert;
    for (int i = 0; i < 200; ++i) {
        delay(3);

        toInsert = generateMessage(0);

        bufA->insert(toInsert);

        cout << "message " << toInsert.message << " inserted - producent A" << endl;
    }
    pthread_exit(NULL);
}

void *producentB(void *ptr) {
    Message toInsert;
    for (int i = 0; i < 200; ++i) {
        delay(3);

        toInsert = generateMessage(0);

        bufB->insert(toInsert);

        cout << "message " << toInsert.message << " inserted - producent B" << endl;
    }
    pthread_exit(NULL);
}

void *producentC(void *ptr) {
    Message toInsert;
    for (int i = 0; i < 200; ++i) {
        delay(3);

        toInsert = generateMessage(0);

        bufC->insert(toInsert);

        cout << "message " << toInsert.message << " inserted - producent C" << endl;
    }
    pthread_exit(NULL);
}

void *specProducent(void *ptr) {
    Message toInsert;
    for (int i = 0; i < 200; ++i) {
        delay(3);

        toInsert = generateMessage(1);

        switch (generateSign()) {
            case 'A':
                bufA->insert(toInsert);
            case 'B':
                bufB->insert(toInsert);
            case 'C':
                bufC->insert(toInsert);
        }

        bufA->insert(toInsert);

        cout << "message " << toInsert.message << " inserted - producent spec" << endl;
    }
    pthread_exit(NULL);
}

void *KonsA(void *ptr) {
    Message popped;
    for (int i = 0; i < 200; ++i) {
        popped = bufA->take();
        delay(0.5);

        if (checkLength(popped)) {
            printf("received message %s kons A\n", popped.message);

            char consumedSign = popped.message[0];
            switch (checkLength(popped)) {
                case 1:
                    if (doWithProbabilty(pr)) {
                        popped.message[0] = generateSign();
                    } else {
                        popped.message[0] = 'X';
                    }
                case 2:
                    popped.message[0] = popped.message[1];
                    if (doWithProbabilty(pr)) {
                        popped.message[1] = generateSign();
                    } else {
                        popped.message[1] = 'X';
                    }
                case 3:
                    popped.message[0] = popped.message[1];
                    popped.message[1] = popped.message[2];
                    if (doWithProbabilty(pr)) {
                        popped.message[2] = generateSign();
                    } else {
                        popped.message[2] = 'X';
                    }
            }

            switch (consumedSign) {
                case 'A':
                    bufA->insert(popped);
                case 'B':
                    bufB->insert(popped);
                case 'C':
                    bufC->insert(popped);
            }

            printf("i reinsert message %s to queue %c\n", popped.message, consumedSign);

        } else {
            printf("dumping empty message \n");
        }
    }
    pthread_exit(NULL);
}

void *KonsB(void *ptr) {
    Message popped;
    for (int i = 0; i < 200; ++i) {
        popped = bufB->take();
        delay(0.5);

        if (checkLength(popped)) {
            printf("received message %s kons B\n", popped.message);

            char consumedSign = popped.message[0];
            switch (checkLength(popped)) {
                case 1:
                    if (doWithProbabilty(pr)) {
                        popped.message[0] = generateSign();
                    } else {
                        popped.message[0] = 'X';
                    }
                case 2:
                    popped.message[0] = popped.message[1];
                    if (doWithProbabilty(pr)) {
                        popped.message[1] = generateSign();
                    } else {
                        popped.message[1] = 'X';
                    }
                case 3:
                    popped.message[0] = popped.message[1];
                    popped.message[1] = popped.message[2];
                    if (doWithProbabilty(pr)) {
                        popped.message[2] = generateSign();
                    } else {
                        popped.message[2] = 'X';
                    }
            }

            switch (consumedSign) {
                case 'A':
                    bufA->insert(popped);
                case 'B':
                    bufB->insert(popped);
                case 'C':
                    bufC->insert(popped);
            }
            printf("i reinsert message %s to queue %c\n", popped.message, consumedSign);

        } else {
            printf("dumping empty message \n");
        }
    }
    pthread_exit(NULL);
}

void *KonsC(void *ptr) {
    Message popped;
    for (int i = 0; i < 200; ++i) {
        popped = bufC->take();
        delay(0.5);

        if (checkLength(popped)) {
            printf("received message %s kons C\n", popped.message);

            char consumedSign = popped.message[0];
            switch (checkLength(popped)) {
                case 1:
                    if (doWithProbabilty(pr)) {
                        popped.message[0] = generateSign();
                    } else {
                        popped.message[0] = 'X';
                    }
                case 2:
                    popped.message[0] = popped.message[1];
                    if (doWithProbabilty(pr)) {
                        popped.message[1] = generateSign();
                    } else {
                        popped.message[1] = 'X';
                    }
                case 3:
                    popped.message[0] = popped.message[1];
                    popped.message[1] = popped.message[2];
                    if (doWithProbabilty(pr)) {
                        popped.message[2] = generateSign();
                    } else {
                        popped.message[2] = 'X';
                    }
            }

            switch (consumedSign) {
                case 'A':
                    bufA->insert(popped);
                case 'B':
                    bufB->insert(popped);
                case 'C':
                    bufC->insert(popped);
            }
            printf("i reinsert message %s to queue %c\n", popped.message, consumedSign);

        } else {
            printf("dumping empty message \n");
        }
    }
    pthread_exit(NULL);
}