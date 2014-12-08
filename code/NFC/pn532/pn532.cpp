#include "pn532.h"

//wake up
const quint8 wake_up_code[14] = {0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const quint8 wake_up_cmd[2] = {0x14, 0x01};

//get firmware
const quint8 get_fireware_cmd[1] = {0x02};

//search tag
const quint8 in_list_passive_target_cmd[] = {0x4a, 0x01, 0x00};


#define     DEBUG       1
#if DEBUG
#include <stdio.h>
#define     TRACE(...)      {printf(__VA_ARGS__); fflush(stdout);}
#else
#define     TRACE(...)
#endif


pn532::pn532(QObject *parent) :
    QObject(parent)
{
    pn532_timer = new QTimer(this);
    connect(pn532_timer, SIGNAL(timeout()),
            this, SLOT(pn532_serial_timeout()));

    pn532_on = false;
    response_status = RESPONSE_INITED;
    ack_status = ACK_INITED;

    init_cmd_packet();
}

pn532::~pn532()
{

}

bool pn532::open_pn532(QString port_name)
{
    (void)port_name;
    return true;
}

//lower byte of [LEN + LCS] = 0x00
quint8 pn532::calc_checksum_lcs(quint8 len)
{
    quint8 lcs = 0;
    lcs = 0x100 - len;
    return lcs;
}

//lower byte of [TFI + PD0 + PD1 + ... + PDn + DCS] = 0x00
quint8 pn532::calc_checksum_dcs(quint8 *data, quint8 len)
{
    quint8 dcs = 0;
    for(quint8 i = 0; i < len; i++)
        dcs += data[i];
    dcs = 0x100 - dcs;
    return dcs;
}

qint16 pn532::generate_cmd_frame(quint8 *cmd_data, quint8 len)
{
    qint16 total_len;
    pn532_packet.len = len + 1;
    pn532_packet.lcs = calc_checksum_lcs(pn532_packet.len);
    memcpy(pn532_packet.data, cmd_data, len);
    pn532_packet.dcs = calc_checksum_dcs(&pn532_packet.tfi, len + 1);
    pn532_packet.data[len] = pn532_packet.dcs;
    pn532_packet.data[len + 1] = pn532_packet.postamble;
    total_len = FRAME_CMD_INFORMATION_SIZE;
    total_len += pn532_packet.len;
    return total_len;
}

void pn532::parse_ack_frame(quint8 data)
{
    static quint8 packet_status = ACK_PACKET_PREAMBLE;
    static
    switch(packet_status)
    {
        case ACK_PACKET_PREAMBLE:
            if(data != 0x00)
                ack_status = ACK_NACK;
            break;
        case ACK_PACKET_START_CODE:

    }
}


//#wake up reader
//send:
//55 55 00 00 00 00 00 00 00 00 00 00 00 00
//cmd: 00 00 ff 03 fd d4 14 01 17 00
//return:
//ack: 00 00 FF 00 FF 00
//response: 00 00 FF 02 FE D5 15 16 00
bool pn532::pn532_wake_up()
{
    qint16 len;
    len = generate_cmd_frame((quint8 *)wake_up_cmd, sizeof(wake_up_cmd));
#if DEBUG
    show_cmd_packet(len);
#endif
    pn532_timer->start(10);
    while(!ack_status);
    if(ack_status != ACK_OK)
    {
        ack_status = ACK_INITED;
        return false;
    }
    while(!response_status);
    if()

    return true;
}

bool pn532::pn532_get_firmware_version(pn532_version_t &version)
{
    quint16 len;
    len = generate_cmd_frame((quint8 *)get_fireware_cmd, sizeof(get_fireware_cmd));
#if DEBUG
    show_cmd_packet(len);
#endif

}

void pn532::init_cmd_packet()
{
    pn532_packet.preamble = 0x00;
    pn532_packet.start_code = 0x00;
    pn532_packet.start_code |= 0xFF << 8;
    pn532_packet.postamble = 0x00;
    pn532_packet.tfi = 0xd4;
}

void pn532::show_cmd_packet(quint8 len)
{
    TRACE("cmd packet:\r\n");
    for(quint8 i = 0; i < len; i++)
    {
        TRACE("%02x ", ((quint8 *)&pn532_packet)[i]);
    }
    TRACE("\r\n");
}

void pn532::pn532_serial_timeout()
{
    if(ack_ok == ACK_INITED)
    {
        ack_ok = ACK_TIMEOUT;
        return;
    }
}
