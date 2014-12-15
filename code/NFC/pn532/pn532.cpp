#include "pn532.h"

//wake up
const quint8 wake_up_code[14] = {0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const quint8 wake_up_cmd[2] = {0x14, 0x01};

//get firmware
const quint8 get_fireware_cmd[1] = {0x02};

//search tag
const quint8 list_passive_target_cmd[3] = {0x4a, 0x01, 0x00};



#define     DEBUG       1
#if DEBUG
#include <stdio.h>
#define     TRACE(...)      {printf(__VA_ARGS__); fflush(stdout);}
#else
#define     TRACE(...)
#endif

/******************************************************************************/
/**************************    Init PN532       *******************************/

PN532::PN532(QObject *parent) :
    QObject(parent)
{

    pn532_port = new SerialPort(this);

    pn532_status = PN532_POWER_DOWN;

    init_cmd_packet();
}

PN532::~PN532()
{
    if(pn532_port)
        pn532_port->close();
}

void PN532::init_cmd_packet()
{
    pn532_cmd_packet.preamble = 0x00;
    pn532_cmd_packet.start_code = 0x00;
    pn532_cmd_packet.start_code |= 0xFF << 8;
    pn532_cmd_packet.postamble = 0x00;
    pn532_cmd_packet.tfi = 0xd4;
}

/******************************************************************************/

/******************************************************************************/
/**************************    packet process   *******************************/

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
				offset = 0;
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
				offset = 0;
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
        packet_ack_status = ACK_PACKET_PREAMBLE;
        break;
    }

    return ret;
}


bool PN532::parse_response_frame(quint8 data)
{
    static quint8 offset = 0;
    quint8 tmp;
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
                offset = 0;
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
        tmp = calc_checksum_lcs(pn532_response_packet.len);
        if(tmp == data)
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
            pn532_response_packet.tfi = data;
        }
        else
        {
            packet_repsonse_status = RESPONSE_PACKET_PREAMBLE;
        }
        break;
    case RESPONSE_PACKET_DATA:
        if(offset < (pn532_response_packet.len - 1))
        {
            pn532_response_packet.data[offset++] = data;
        }
        if(offset == (pn532_response_packet.len - 1))
        {
            packet_repsonse_status = RESPONSE_PACKET_DCS;
        }
        break;
    case RESPONSE_PACKET_DCS:
        tmp = calc_checksum_dcs(&pn532_response_packet.tfi, pn532_response_packet.len);
        if(tmp == data)
        {
            packet_repsonse_status = RESPONSE_PACKET_POSTAMBLE;
        }
        else
        {
            packet_repsonse_status = RESPONSE_PACKET_PREAMBLE;
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
/******************************************************************************/


/******************************************************************************/
/**************************    pn532  process   *******************************/

bool PN532::pn532_open(QString port_name)
{
    bool ret = false;
    ret = pn532_port->open(port_name, 115200);
    if(ret == false)
        return ret;
    pn532_status = PN532_CMD;
    ret = wake_up();
    if(ret == false)
    {
        pn532_port->close();
        pn532_status = PN532_POWER_DOWN;
    }
    return ret;
}

bool PN532::pn532_close()
{
    if(pn532_port)
    {
        pn532_port->close();
    }
    pn532_status = PN532_POWER_DOWN;
    return true;
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
    if(!pn532_cmd_process((quint8 *)wake_up_cmd, sizeof(wake_up_cmd)))
        return false;

	return true;
}


//#get firmware
//send:  00 00 FF 02 FE D4 02 2A 00
//return:
//00 00 FF 00 FF 00
//00 00 FF 06 FA D5 03 32 01 06 07 E8 00
bool PN532::pn532_get_firmware_version(pn532_version_t &version)
{
   if(!pn532_cmd_process((quint8 *)get_fireware_cmd,
                         sizeof(get_fireware_cmd)))
       return false;

    if(pn532_response_packet.len != 6)
        return false;

    version.ic = pn532_response_packet.data[1];
    version.ver = pn532_response_packet.data[2];
    version.rev = pn532_response_packet.data[3];
    version.support = pn532_response_packet.data[4];
    return true;
}


//#read the tag
//send: 00 00 FF 04 FC D4 4A 01 00 E1 00
//0x00: 106 kbps type A(ISO/IEC14443 Type A)
//return:
//00 00 FF 00 FF 00
//00 00 FF 0C F4 D5 4B 01 01 00 04 08 04 XX XX XX XX 5A 00
//XX is tag.
bool PN532::pn532_list_passive_target(QByteArray &uid)
{

    if(!pn532_cmd_process((quint8 *)list_passive_target_cmd,
                          sizeof(list_passive_target_cmd)))
        return false;

    if(pn532_response_packet.data[1] == 0)      //NbTg
        return false;

    if(pn532_response_packet.data[6] >= 20)     //NFCIDLength
        return false;

    QByteArray tmp((const char *)&pn532_response_packet.data[7],
            pn532_response_packet.data[6]);
    uid = tmp;

    return true;
}

bool PN532::pn532_read_data(quint8 block_no, QByteArray &data)
{
    quint8 cmd_data[3];
    QByteArray uid;
    if(!pn532_list_passive_target(uid))
        return false;

    cmd_data[0] = CMD_IN_DATA_EXCHANGE;
    cmd_data[1] = 0x30;
    cmd_data[2] = block_no;
    if(!pn532_cmd_process(cmd_data, 3))
        return false;

    if(pn532_response_packet.data[1] != 0x00)  //status
        return false;
    data = QByteArray((const char *)&pn532_response_packet.data[2], 16);
    return data.size();
}

bool PN532::pn532_write_data(quint8 block_no, QByteArray data)
{
    QByteArray uid;
    quint8 cmd_data[];
    if(data.size() != 4)
        return false;
    if(!pn532_list_passive_target(uid))
        return false;

    cmd_data[0] = CMD_IN_DATA_EXCHANGE;
    cmd_data[1] = 0xA2;
    memcpy(cmd_data[2], data.data(), data.size());
}

bool PN532::pn532_cmd_process(quint8 *cmd_data, quint16 cmd_len)
{
    if(pn532_status == PN532_POWER_DOWN)
        return false;

    qint16 len;
    quint8 cmd;
    cmd = cmd_data[0];
    switch(cmd)
    {
    case CMD_WAKE_UP:
        pn532_port->write((quint8 *)wake_up_code, sizeof(wake_up_code));
        break;
    }

    len = generate_cmd_frame(cmd_data, cmd_len);
    pn532_port->write((quint8 *)&pn532_cmd_packet, sizeof(pn532_cmd_packet));
#if DEBUG
    show_cmd_packet(len);
#endif

    sleep(50);
    if(!pn532_serial_process())
        return false;

    if(pn532_response_packet.data[0] != (cmd + 1))
        return false;

    return true;
}

bool PN532::pn532_serial_process()
{
    bool ret;
    quint16 len, i;
    QByteArray buf;
    len = pn532_port->read(buf);

    if(pn532_status == PN532_CMD)
        pn532_status = PN532_ACK;
    packet_ack_status = ACK_PACKET_PREAMBLE;
    packet_repsonse_status = ACK_PACKET_PREAMBLE;

	TRACE("Receive data: ");
    for(i = 0; i < len; i++)
    {
        TRACE("%02x ", (quint8)buf.at(i));
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
                TRACE("\r\n");
                pn532_status = PN532_CMD;
				return true;
			}
            break;
        default:
            pn532_status = PN532_CMD;
            return false;
            break;
        }
    }

    TRACE("\r\n");
    pn532_status = PN532_CMD;
    return false;

}

void PN532::sleep(quint16 msec)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed() < msec)
    {
        QCoreApplication::processEvents();
    }
}

/******************************************************************************/

/******************************************************************************/
/**************************    debug & test     *******************************/

void PN532::show_cmd_packet(quint8 len)
{
    TRACE("cmd packet:\r\n");
    for(quint8 i = 0; i < len; i++)
    {
        TRACE("%02x ", ((quint8 *)&pn532_cmd_packet)[i]);
    }
    TRACE("\r\n");
}

void PN532::print_response_packet()
{
    quint8 i;
    TRACE("len: %d\r\n", pn532_response_packet.len);
    TRACE("data: ");
    for(i = 0; i < (pn532_response_packet.len - 1); i++)
    {
        TRACE("%02x ", pn532_response_packet.data[i]);
    }
    TRACE("\r\n");
}

bool PN532::test_parse_function(QByteArray data)
{
    bool ret;
    quint16 len, i;
    len = data.size();

    pn532_status = PN532_ACK;
    packet_ack_status = ACK_PACKET_PREAMBLE;
    packet_repsonse_status = ACK_PACKET_PREAMBLE;

    for(i = 0; i < len; i++)
    {
        switch(pn532_status)
        {
        case PN532_ACK:
            ret = parse_ack_frame(data.at(i));
            if(ret)
                pn532_status = PN532_RESPONSE;
            break;
        case PN532_RESPONSE:
            ret = parse_response_frame(data.at(i));
            if(ret)
            {
                print_response_packet();
                pn532_status = PN532_CMD;
				return true;
            }
            break;
        default:
            pn532_status = PN532_CMD;
            return false;
            break;
        }
    }

    pn532_status = PN532_CMD;
    return false;

}

/******************************************************************************/



