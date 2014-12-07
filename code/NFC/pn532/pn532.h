#ifndef PN532_H
#define PN532_H

#include <QObject>
#include <QTimer>

#pragma pack(push)
#pragma pack(1)
struct pn532_cmd_t {
    quint8 preamble;        //0x00
    quint16 start_code;     //0x00 0xff
    quint8 len;             //TFI and PD0 to PDn
    quint8 lcs;             //length checksum, lower byte of[len + lcs] = 0x00
    quint8 tfi;             //D4H(host to PN532), D5H(PN532 to host)
    quint8 data[50];        //(LEN - 1) data
    quint8 dcs;             //lower byte of[TFI + PD0 + PD1 + ... + PDn + DCS] = 0x00
    quint8 postamble;       //0x00
};
#pragma pack(pop)

#if 0
enum {
    CMD_GET_FIRMWARE_VERSION = 0x02,
    CMD_WAKE_UP = 0x14,
};
#endif

enum {
    FRAME_CMD_PREAMBLE_SIZE =   1,
    FRAME_CMD_START_CODE_SIZE = 2,
    FRAME_CMD_LEN_SIZE =        1,
    FRAME_CMD_LCS_SIZE =        1,
    FRAME_CMD_DCS_SIZE =        1,
    FRAME_CMD_POSTAMBLE_SIZE =  1
};

#define     FRAME_CMD_INFORMATION_SIZE     (FRAME_CMD_POSTAMBLE_SIZE + \
                FRAME_CMD_START_CODE_SIZE + FRAME_CMD_LEN_SIZE + \
                FRAME_CMD_LCS_SIZE + FRAME_CMD_DCS_SIZE + FRAME_CMD_POSTAMBLE_SIZE)


class pn532 : public QObject
{
    Q_OBJECT
public:
    explicit pn532(QObject *parent = 0);
    ~pn532();

    bool open_pn532(QString port_name);
    bool pn532_wake_up();

private:
    bool pn532_on;
    bool response_ok;
    bool ack_ok;
    QTimer *pn532_timer;
    pn532_cmd_t pn532_packet;

private:
    quint8 calc_checksum_lcs(quint8 len);
    quint8 calc_checksum_dcs(quint8 *data, quint8 len);
    qint16 generate_cmd_frame(quint8 *cmd_data, quint8 len);
    bool parse_frame(quint8 data);
    void init_cmd_packet();
    void show_cmd_packet(quint8 len);

signals:

public slots:
    void pn532_serial_timeout();

};

#endif // PN532_H
