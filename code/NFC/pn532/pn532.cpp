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


PN532::PN532(QObject *parent) :
    QObject(parent)
{
    pn532_timer = new QTimer(this);
    connect(pn532_timer, SIGNAL(timeout()),
            this, SLOT(pn532_serial_timeout()));

    pn532_port = new SerialPort(this);

    pn532_status = PN532_POWER_DOWN;
    pn532_response_status = PN532_RESPONSE_INIT;
    packet_ack_status = ACK_PACKET_PREAMBLE;
    packet_repsonse_status = ACK_PACKET_PREAMBLE;

    init_cmd_packet();
}

PN532::~PN532()
{

}

bool PN532::pn532_open(QString port_name)
{
    bool ret = false;
    ret = pn532_port->open(port_name, 115200);
    ret = wake_up();
    return ret;
}

//lower byte of [LEN + LCS] = 0x00
quint8 PN532::calc_checksum_lcs(quint8 len)
{
    quint8 lcs = 0;
    lcs = 0x100 - len;
    return lcs;
}

//lower byte of [TFI + PD0 + PD1 + ... + PDn + DCS] = 0x00
quint8 PN532::calc_checksum_dcs(quint8 *data, quint8 len)
{
    quint8 dcs = 0;
    for(quint8 i = 0; i < len; i++)
        dcs += data[i];
    dcs = 0x100 - dcs;
    return dcs;
}

qint16 PN532::generate_cmd_frame(quint8 *cmd_data, quint8 len)
{
    qint16 total_len;
    pn532_cmd_packet.len = len + 1;
    pn532_cmd_packet.lcs = calc_checksum_lcs(pn532_cmd_packet.len);
    memcpy(pn532_cmd_packet.data, cmd_data, len);
    pn532_cmd_packet.dcs = calc_checksum_dcs(&pn532_cmd_packet.tfi, len + 1);
    pn532_cmd_packet.data[len] = pn532_cmd_packet.dcs;
    pn532_cmd_packet.data[len + 1] = pn532_cmd_packet.postamble;
    total_len = FRAME_CMD_INFORMATION_SIZE;
    total_len += pn532_cmd_packet.len;
    return total_len;
}

bool PN532::parse_ack_frame(quint8 data)
{
    static quint8 packet_ack_status = ACK_PACKET_PREAMBLE;
    static quint8 offset = 0;
    bool ret = false;

    switch(packet_ack_status)
    {
    case ACK_PACKET_PREAMBLE:
        if(data == 0x00)
        {
            packet_ack_status = ACK_PACKET_START_CODE;
            offset = 0;
        }
        break;
    case ACK_PACKET_START_CODE:
        switch(offset)
        {
        case 0:
            if(data == 0x00)
            {
                offset += 1;
            }
            else
            {
                packet_ack_status = ACK_PACKET_PREAMBLE;
            }
            break;
        case 1:
            if(data == 0xff)
            {
                packet_ack_status = ACK_PACKET_ACK_CODE;
            }
            else
            {
                packet_ack_status = ACK_PACKET_PREAMBLE;
            }
            break;
        default:
            packet_ack_status = ACK_PACKET_PREAMBLE;
            break;
        }
        break;
    case ACK_PACKET_ACK_CODE:
        switch(offset)
        {
        case 0:
            if(data == 0x00)
            {
                offset += 1;
            }
            else
            {
                packet_ack_status = ACK_PACKET_PREAMBLE;
            }
            break;
        case 1:
            if(data == 0xff)
            {
                packet_ack_status = ACK_PACKET_POSTAMBLE;
            }
            else
            {
                packet_ack_status = ACK_PACKET_PREAMBLE;
            }
            break;
        default:
            packet_ack_status = ACK_PACKET_PREAMBLE;
            break;
        }
        break;
    case ACK_PACKET_POSTAMBLE:
        if(data == 0x00)
        {
            ret = true;
        }
        packet_status = ACK_PACKET_PREAMBLE;
        break;
    }

    return ret;
}


bool PN532::parse_response_frame(quint8 data)
{
    static quint8 packet_repsonse_status = RESPONSE_PACKET_PREAMBLE;
    static quint8 offset = 0;
    bool ret = false;

    switch(packet_repsonse_status)
    {
    case RESPONSE_PACKET_PREAMBLE:
        if(data == 0x00)
        {
            packet_repsonse_status = RESPONSE_PACKET_START_CODE;
            offset = 0;
        }
        break;
    case RESPONSE_PACKET_START_CODE:
        switch(offset)
        {
        case 0:
            if(data == 0x00)
            {
                offset += 1;
            }
            else
            {
                packet_repsonse_status = RESPONSE_PACKET_PREAMBLE;
            }
            break;
        case 1:
            if(data == 0xff)
            {
                packet_repsonse_status = RESPONSE_PACKET_LEN;
            }
            else
            {
                packet_repsonse_status = RESPONSE_PACKET_PREAMBLE;
            }
            break;
        default:
            packet_repsonse_status = RESPONSE_PACKET_PREAMBLE;
            break;
        }
        break;
    case RESPONSE_PACKET_LEN:
        pn532_response_packet.len = data;
        packet_repsonse_status = RESPONSE_PACKET_LCS;
        break;
    case RESPONSE_PACKET_LCS:
        quint8 lcs = calc_checksum_lcs(pn532_response_packet.len);
        if(lcs == data)
        {
            packet_repsonse_status = RESPONSE_PACKET_TFI;
        }
        else
        {
            packet_repsonse_status = RESPONSE_PACKET_PREAMBLE;
        }
        break;
    case RESPONSE_PACKET_TFI:
        if(data == 0xD5)
        {
            packet_repsonse_status = RESPONSE_PACKET_DATA;
        }
        else
        {
            packet_repsonse_status = RESPONSE_PACKET_PREAMBLE;
        }
        break;
    case RESPONSE_PACKET_DATA:
        if(offset < pn532_response_packet.len)
        {
            pn532_response_packet.data[offset++] = data;
        }
        else
        {
            packet_repsonse_status = RESPONSE_PACKET_POSTAMBLE;
        }
        break;
    case RESPONSE_PACKET_POSTAMBLE:
        if(data == 0x00)
        {
            ret = true;
        }
        packet_repsonse_status = RESPONSE_PACKET_PREAMBLE;
        break;
    }

    return ret;
}


//#wake up reader
//send:
//55 55 00 00 00 00 00 00 00 00 00 00 00 00
//cmd: 00 00 ff 03 fd d4 14 01 17 00
//return:
//ack: 00 00 FF 00 FF 00
//response: 00 00 FF 02 FE D5 15 16 00
bool PN532::wake_up()
{
    qint16 len;

    len = generate_cmd_frame((quint8 *)wake_up_cmd, sizeof(wake_up_cmd));
    pn532_port->write((quint8 *)wake_up_code, sizeof(wake_up_code));
    pn532_port->write((quint8 *)&pn532_cmd_packet, sizeof(pn532_cmd_packet));
#if DEBUG
    show_cmd_packet(len);
#endif

    pn532_timer->start(20);
    while(pn532_response_status == PN532_RESPONSE_INIT);
    if(pn532_response_status == PN532_RESPONSE_ERROR)
        return false;

    return true;
}



bool PN532::pn532_get_firmware_version(pn532_version_t &version)
{
    quint16 len;
    len = generate_cmd_frame((quint8 *)get_fireware_cmd, sizeof(get_fireware_cmd));
#if DEBUG
    show_cmd_packet(len);
#endif
    return true;
}

void PN532::init_cmd_packet()
{
    pn532_cmd_packet.preamble = 0x00;
    pn532_cmd_packet.start_code = 0x00;
    pn532_cmd_packet.start_code |= 0xFF << 8;
    pn532_cmd_packet.postamble = 0x00;
    pn532_cmd_packet.tfi = 0xd4;
}

void PN532::show_cmd_packet(quint8 len)
{
    TRACE("cmd packet:\r\n");
    for(quint8 i = 0; i < len; i++)
    {
        TRACE("%02x ", ((quint8 *)&pn532_cmd_packet)[i]);
    }
    TRACE("\r\n");
}

void PN532::pn532_serial_timeout()
{
    bool ret;
    quint16 len, i;
    QByteArray buf;
    len = pn532_port->read(buf);
    for(i = 0; i < len; i++)
    {
        switch(pn532_status)
        {
        case PN532_ACK:
            ret = parse_ack_frame(buf.at(i));
            if(ret)
                pn532_status = PN532_RESPONSE;
            break;
        case PN532_RESPONSE:
            ret = parse_response_frame(buf.at(i));
            if(ret)
            {
                pn532_status = PN532_CMD;
                pn532_response_status = PN532_RESPONSE_OK;
            }
            return;
            break;
        default:
            pn532_status = PN532_CMD;
            pn532_response_status = PN532_RESPONSE_ERROR;
            return;
            break;
        }
    }

    if(i == len)
    {
        pn532_status = PN532_CMD;
        pn532_response_status = PN532_RESPONSE_ERROR;
    }
}
