#include "nfctargetmemoryview.h"
#include "ui_nfctargetmemoryview.h"

NFCTargetMemoryView::NFCTargetMemoryView(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::NFCTargetMemoryView)
{
    ui->setupUi(this);
}

NFCTargetMemoryView::~NFCTargetMemoryView()
{
    delete ui;
}

void NFCTargetMemoryView::show_memory_data(QByteArray data)
{
    quint8 i;
    QString str;
    if(data.size != 16)
        return;

    str.clear();
    for(i = 0; i < 4; i++)
    {
        str += QString("%1").arg(data.at(i), 2, 16, '0');
    }
    ui->Block0LineEdit->setText(str);

    str.clear();
    for(i = 4; i < 8; i++)
    {
        str += QString("%1").arg(data.at(i), 2, 16, '0');
    }
    ui->Block1LineEdit->setText(str);

    str.clear();
    for(i = 8; i < 12; i++)
    {
        str += QString("%1").arg(data.at(i), 2, 16, '0');
    }
    ui->Block2LineEdit->setText(str);

    str.clear();
    for(i = 12; i < 16; i++)
    {
        str += QString("%1").arg(data.at(i), 2, 16, '0');
    }
    ui->Block3LineEdit->setText(str);
}

void NFCTargetMemoryView::on_ReadBtn_clicked()
{
    bool ok;
    quint8 block_no = ui->BlockNoLineEdit->text().toUShort(&ok, 10);
    if(!ok)
        return;
    emit(read_memory_cmd(block_no));
}

void NFCTargetMemoryView::on_WriteBtn_clicked()
{
    bool ok;
    quint8 block_no = ui->BlockNoLineEdit->text().toUShort(&ok, 10);
    if(!ok)
        return;
    QStringList list = ui->Block0LineEdit->text().split(" ");
    if(list.size() != 4)
        return;
    QByteArray data;
    data.clear();
    for(quint8 i = 0; i < list.size(); i++)
    {
        quint8 tmp;
        tmp = list.at(i).toUShort(&ok, 16);
        if(!ok)
            return;
        data.append(tmp);
    }
    emit(write_memory_cmd(block_no, data));
}
