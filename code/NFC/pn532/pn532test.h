#ifndef PN532TEST_H
#define PN532TEST_H

#include <QObject>
#include "pn532.h"

class PN532Test : public QObject
{
    Q_OBJECT
public:
    explicit PN532Test(QObject *parent = 0);
    ~PN532Test();
    void pn532_test_normal();
    void pn532_test_error();

private:
    PN532 *pn532_demo;

signals:

public slots:

};

#endif // PN532TEST_H
