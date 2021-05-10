#include "verycam.h"

VeryCam::VeryCam(QObject *parent, int tip) : QObject(parent)
{
type = tip;
}

void VeryCam::verifyQR(QString code)
{
    if(type==2)
    {
        if(qr_found==false)
        {
            QR = code;
            qr_found=true;
            if(ean13_found)
                emit stopCam();
        }
    }

    if(type==1)
    {
        if(qr_found==false)
        {
            QR = code;
            if(!QRs.contains(QR))
            {
                QRs.push_back(QR);
            }

        }
    }


}

void VeryCam::verifyEAN(QString code)
{
    if(ean13_found==false)
    {
        ean13_found = true;
        EAN = code;
        if(qr_found)
            emit stopCam();
    }


}
