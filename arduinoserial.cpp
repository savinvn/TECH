#include "arduinoserial.h"
#include <QTime>

ArduinoSerial::ArduinoSerial(QObject *parent, int PortN) :
    QObject(parent)
{
    stop = false;
    myString = "OK";
    //for (int i=2;i<14;i++)

//    {
//    serial.setPortName("COM"+QString::number(i));
//    if(serial.open(QIODevice::ReadWrite))
//        break;
//    }
        serial.setPortName("COM"+QString::number(PortN));

    serial.open(QIODevice::ReadWrite);
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.setDataTerminalReady(false);
    serial.setRequestToSend(false);

    connect(&serial,SIGNAL(readyRead()),this,SLOT(readData()));
    iters=1;
}

ArduinoSerial::~ArduinoSerial()
{
    serial.close();
}
int c = 0;
void ArduinoSerial::readData()
{
    QByteArray data;
        if (serial.isOpen()&&serial.bytesAvailable()>3)
        {
           data = serial.readAll();
           emit recievedBA(data);
           myString=QString(data);
           emit RecivedData(myString);
           c++;
          serial.flush();
          emit elapsedTime(QString::number(c));
        }

}

void ArduinoSerial::WriteToPort(QString data)
{
    const char* ba = data.toStdString().data();
    if (serial.isOpen() && serial.isWritable() )
      {
              serial.write(data.toStdString().data());
       }

}

void ArduinoSerial::WriteToPort(QByteArray data)
{

    if (serial.isOpen() && serial.isWritable() )
      {
              serial.write(data);

       }

}
