#ifndef ARDUINOSERIAL_H
#define ARDUINOSERIAL_H
#include <QtSerialPort/QSerialPort>
#include <QIODevice>

#include <QObject>

class ArduinoSerial : public QObject
{
    Q_OBJECT
public:
    explicit ArduinoSerial(QObject *parent = 0,int PortN=0);
    ~ArduinoSerial();
 QSerialPort serial;
 int PortN;
 bool stop;
 int iters;
private:
  QString myString;
signals:
    void RecivedData(QString);
    void elapsedTime(QString);
    void recievedBA(QByteArray b);
public slots:

 void readData();

 void WriteToPort(QString data);
 void WriteToPort(QByteArray data);



};

#endif // ARDUINOSERIAL_H
