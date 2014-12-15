#ifndef PN532_H
#define PN532_H

#include <QObject>
#include <QElapsedTimer>
#include <QCoreApplication>
#include "serialport.h"

#pragma pack(push)
#pragma pack(1)
struct pn532_packet_t {
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

struct pn532_version_t {
    quint8 ic;              //Version of the IC. For PN532, byte is 0x32
    quint8 ver;             //Version of the firmware
    quint8 rev;             //Revision of the firmware
    //Bit0:ISO/IEC 14443 TYPEA, Bit1: ISO/IEC 14443 TYPEB, Bit2: ISO18092
    quint8 support;
};

enum pn532_cmd_t{
    CMD_GET_FIRMWARE_VERSION = 0x02,
    CMD_WAKE_UP = 0x14,
    CMD_IN_LIST_PASSIVE_TARGET = 0x4a,
    CMD_IN_DATA_EXCHANGE = 0x40,
};

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

enum pn532_status_t{
    PN532_POWER_DOWN = 0,
    PN532_CMD = 1,
    PN532_ACK = 2,
    PN532_RESPONSE = 3,
};


enum {
    RESPONSE_PACKET_PREAMBLE = 0,
    RESPONSE_PACKET_START_CODE = 1,
    RESPONSE_PACKET_LEN = 2,
    RESPONSE_PACKET_LCS = 3,
    RESPONSE_PACKET_TFI = 4,
    RESPONSE_PACKET_DATA = 5,
    RESPONSE_PACKET_DCS = 6,
    RESPONSE_PACKET_POSTAMBLE = 7
};

enum {
    ACK_PACKET_PREAMBLE = 0,
    ACK_PACKET_START_CODE = 1,
    ACK_PACKET_ACK_CODE = 2,
    ACK_PACKET_POSTAMBLE = 4
};


class PN532 : public QObject
{
    Q_OBJECT
public:
    explicit PN532(QObject *parent = 0);
    ~PN532();

    bool pn532_open(QString port_name);
    bool pn532_close();
    bool pn532_get_firmware_version(pn532_version_t &version);
    bool pn532_list_passive_target(QByteArray &uid);
    bool pn532_read_data(quint8 block_no, QByteArray &data);
    bool pn532_write_data(quint8 block_no, QByteArray data);

    bool test_parse_function(QByteArray data);


private:
    SerialPort *pn532_port;

    quint8 packet_repsonse_status;
    quint8 packet_ack_status;

    pn532_status_t pn532_status;
    pn532_packet_t pn532_cmd_packet;
    pn532_packet_t pn532_response_packet;


private:
    quint8 calc_checksum_lcs(quint8 len);
    quint8 calc_checksum_dcs(quint8 *data, quint8 len);
    qint16 generate_cmd_frame(quint8 *cmd_data, quint8 len);

    bool wake_up();

    bool parse_response_frame(quint8 data);
    bool parse_ack_frame(quint8 data);
    void init_cmd_packet();

    void show_cmd_packet(quint8 len);
    void print_response_packet();

    bool pn532_cmd_process(quint8 *cmd_data, quint16 len);
    void sleep(quint16 msec);
    bool pn532_serial_process();

};



#endif // PN532_H
