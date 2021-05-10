#include "product.h"

Produc::Produc(QObject *parent) :
    QObject(parent)
{
}
QString Produc::getEANCode() const
{
    return EANCode;
}

void Produc::setEANCode(QString value)
{
    EANCode = value;
}
QString Produc::getQRCode() const
{
    return QRCode;
}

void Produc::setQRCode(const QString &value)
{
    QRCode = value;
}
QDateTime Produc::getProducTime() const
{
    return ProducTime;
}

void Produc::setProducTime( QDateTime value)
{
    ProducTime = value;
}



