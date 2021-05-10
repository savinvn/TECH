#ifndef Produc_H
#define Produc_H
#include <QObject>
#include <QDateTime>

class Produc : public QObject
{
    Q_OBJECT
public:
    explicit Produc(QObject *parent = 0);

    QString getEANCode() const;
    void setEANCode(QString value);

    QString getQRCode() const;
    void setQRCode(const QString &value);

    QDateTime getProducTime() const;
    void setProducTime(QDateTime value);

private:
    QString EANCode;
    QString QRCode;
    QDateTime ProducTime;

signals:

public slots:

};

#endif // Produc_H
