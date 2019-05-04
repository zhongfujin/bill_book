#ifndef ACBOOKTHREAD_H
#define ACBOOKTHREAD_H
#include <QThread>
#include "login.h"

class LoginThread : public QThread
{
    Q_OBJECT

public:
    LoginThread();

private:
    login log;
protected:
    void run();
};


#endif // ACBOOKTHREAD_H
