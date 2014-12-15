#include "pn532test.h"

const quint8 normal_data1[19] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
                               0xFF, 0x06, 0xFA, 0xD5, 0x03, 0x32, 0x01, 0x06,
                               0x07, 0xE8, 0x00};

const quint8 normal_data2[15] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
                               0xFF, 0x02, 0xFE, 0xD5, 0x15, 0x16, 0x00};

//error start_code
const quint8 error_data1[15] = {0x00, 0x00, 0x0F, 0x00, 0xFF, 0x00, 0x00, 0x00,
                              0xFF, 0x02, 0xFE, 0xD5, 0x15, 0x16, 0x00};

//error length
const quint8 error_data2[15] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
                              0xFF, 0x03, 0xFE, 0xD5, 0x15, 0x16, 0x00};

//error postamble
const quint8 error_data3[15] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
                               0xFF, 0x02, 0xFE, 0xD5, 0x15, 0x16, 0x0F};

PN532Test::PN532Test(QObject *parent) :
    QObject(parent)
{
    pn532_demo =  new PN532(this);
}

PN532Test::~PN532Test()
{

}

void PN532Test::pn532_test_normal()
{
    pn532_demo->test_parse_function(QByteArray((const char *)normal_data1, sizeof(normal_data1)));
    pn532_demo->test_parse_function(QByteArray((const char *)normal_data2, sizeof(normal_data2)));
}

void PN532Test::pn532_test_error()
{
    pn532_demo->test_parse_function(QByteArray((const char *)error_data1, sizeof(error_data1)));
    pn532_demo->test_parse_function(QByteArray((const char *)error_data2, sizeof(error_data2)));
    pn532_demo->test_parse_function(QByteArray((const char *)error_data3, sizeof(error_data3)));
}
