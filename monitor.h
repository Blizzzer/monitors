#ifndef __monitor_h
#define __monitor_h

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX 20

#endif

class Semaphore {
public:

    Semaphore(int value) {
#ifdef _WIN32
        sem = CreateSemaphore( NULL, value, 1, NULL );
#else
        if (sem_init(&sem, 0, value) != 0)
            throw "sem_init: failed";
#endif
    }

    ~Semaphore() {
#ifdef _WIN32
        CloseHandle( sem );
#else
        sem_destroy(&sem);
#endif
    }

    void p() {
#ifdef _WIN32
        WaitForSingleObject( sem, INFINITE );
#else
        if (sem_wait(&sem) != 0)
            throw "sem_wait: failed";
#endif
    }

    void v() {
#ifdef _WIN32
        ReleaseSemaphore( sem, 1, NULL );
#else
        if (sem_post(&sem) != 0)
            throw "sem_post: failed";
#endif
    }


private:

#ifdef _WIN32
    HANDLE sem;
#else
    sem_t sem;
#endif
};

class Condition {
    friend class Monitor;

public:
    Condition() : w(0) {
        waitingCount = 0;
    }

    void wait() {
        w.p();
    }

    bool signal() {
        if (waitingCount) {
            --waitingCount;
            w.v();
            return true;
        }//if
        else
            return false;
    }

private:
    Semaphore w;
    int waitingCount; //liczba oczekujacych watkow
};


class Monitor {
public:
    Monitor() : s(1) {}

    void enter() {
        s.p();
    }

    void leave() {
        s.v();
    }

    void wait(Condition &cond) {
        ++cond.waitingCount;
        leave();
        cond.wait();
    }

    void signal(Condition &cond) {
        if (cond.signal())
            enter();
    }


private:
    Semaphore s;
};

class Message {
public:
    int priority; // 0 for low ; 1 for high ; 2 for highest
    char message[3];
};

class SimpleQueue {
    Message buffer[MAX];
    int buf_size;

public:
    SimpleQueue() {
        buf_size = 0;
    };

    ~SimpleQueue() {};

    void insert(Message x) {
        buffer[buf_size] = x;
        buf_size++;
    };

    Message take() {
        Message ret = buffer[0];
        for (int i = 0; i < buf_size - 1; ++i) {
            buffer[i] = buffer[i + 1];
        }
        --buf_size;
        return ret;
    };

    int containsAny() {
        return buf_size > 0;
    }
};

class MyQueue : Monitor {
    Condition full, empty;
    SimpleQueue lowPrioQue;
    SimpleQueue highPrioQue;
    SimpleQueue highestPrioQue;

    int elements;
public:
    MyQueue() {
        lowPrioQue = SimpleQueue();
        highPrioQue = SimpleQueue();
        highestPrioQue = SimpleQueue();
        elements = 0;
    };

    ~MyQueue() {};

    void insert(Message mess) {
        enter();
        if (elements == MAX) wait(full);
        if (mess.priority == 2) {
            highestPrioQue.insert(mess);
            elements++;
        } else if (mess.priority == 1) {
            highPrioQue.insert(mess);
            elements++;
        } else if (mess.priority == 0) {
            lowPrioQue.insert(mess);
            elements++;
        }

        if (elements == 1) signal(empty);
        leave();
    };

    Message take() {
        Message ret;
        enter();
        if (elements == 0) wait(empty);

        if (highestPrioQue.containsAny()) {
            ret = highestPrioQue.take();
            elements--;
        } else if (highPrioQue.containsAny()) {
            ret = highPrioQue.take();
            elements--;
        } else if (lowPrioQue.containsAny()) {
            ret = lowPrioQue.take();
            elements--;
        } else {
            //sth went wrong
        }
        if (elements == MAX - 1) signal(full);
        leave();
        return ret;
    };

    int getSize() {
        int ret;
        enter();
        ret = elements;
        leave();
        return ret;
    }
};

#endif

void delay(float number_of_seconds) {
    clock_t start_time = clock();
    while (clock() < start_time + (number_of_seconds * CLOCKS_PER_SEC));
}

int doWithProbabilty(int pr) {
    return (random() % 100) <= pr;
}

char generateSign() {
    int tmp = random() % 3;
    switch (tmp) {
        case 0:
            return 'A';
        case 1:
            return 'B';
        case 2:
            return 'C';
    }
}

Message generateMessage(int prio) {
    Message result;
    result.priority = prio;
    result.message[0] = generateSign();
    result.message[1] = generateSign();
    result.message[2] = generateSign();
    return result;
}

int checkLength(Message msg) {
    int ret;
    if (msg.message[0] == 'X') {
        ret = 0;
    } else if (msg.message[1] == 'X') {
        ret = 1;
    } else if (msg.message[2] == 'X') {
        ret = 2;
    } else {
        ret = 3;
    }
    return ret;
}




