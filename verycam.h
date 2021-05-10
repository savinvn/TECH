#ifndef VERYCAM_H
#define VERYCAM_H

#include <QObject>

class VeryCam : public QObject
{
    Q_OBJECT
public:
    explicit VeryCam(QObject *parent = nullptr,int tip=0);

    QString EAN;
    QString QR;
    bool ean13_found, qr_found;
    QVector<QString> QRs;
    int curidx=0;
    int type =0;

private slots:
  void verifyQR(QString code);
  void verifyEAN(QString code);

signals:
  void stopCam();

};

#endif // VERYCAM_H
