#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include <QMainWindow>
#include "pn532/pn532.h"

namespace Ui {
class MainProcess;
}

class MainProcess : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainProcess(QWidget *parent = 0);
    ~MainProcess();

private:
    Ui::MainProcess *ui;
    PN532 *nfc_module;
};

#endif // MAINPROCESS_H
