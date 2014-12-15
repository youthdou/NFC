#ifndef NFCTARGETLISTVIEW_H
#define NFCTARGETLISTVIEW_H

#include <QFrame>

namespace Ui {
class NFCTargetListView;
}

class NFCTargetListView : public QFrame
{
    Q_OBJECT

public:
    explicit NFCTargetListView(QWidget *parent = 0);
    ~NFCTargetListView();
    void show_uid(QString uid);

private slots:
    void on_searchBtn_clicked();

signals:
    void list_passive_target_cmd();

private:
    Ui::NFCTargetListView *ui;
};

#endif // NFCTARGETLISTVIEW_H
