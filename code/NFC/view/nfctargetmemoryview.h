#ifndef NFCTARGETMEMORYVIEW_H
#define NFCTARGETMEMORYVIEW_H

#include <QFrame>

namespace Ui {
class NFCTargetMemoryView;
}

class NFCTargetMemoryView : public QFrame
{
    Q_OBJECT

public:
    explicit NFCTargetMemoryView(QWidget *parent = 0);
    ~NFCTargetMemoryView();
    void show_memory_data(QByteArray data);

private slots:
     void on_ReadBtn_clicked();
     void on_WriteBtn_clicked();

signals:
     void read_memory_cmd(quint8 block_no);
     void write_memory_cmd(quint8 block_no, QByteArray data);

private:
    Ui::NFCTargetMemoryView *ui;
};

#endif // NFCTARGETMEMORYVIEW_H
