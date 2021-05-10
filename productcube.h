#ifndef PRODUCTCUBE_H
#define PRODUCTCUBE_H

#include <QObject>
#include <QVector>
#include "product.h"


class ProductCube : public QObject
{
    Q_OBJECT
public:
    explicit ProductCube(QObject *parent = 0);
    int getCubeSize() const;
    void setCubeSize(int value);

    QString getCubeID() const;
    void setCubeID(QString value);
    int currentSize;

private:
    int cubeSize; //количество пачек в кубе
    QString CubeID;   //идентификатор куба (Code128)
    std::vector<Produc*> CubeItems;

signals:

public slots:
    Produc* getCubeItem(int ind);
    void WriteToFile();
    void AddCubeItem(Produc *item);

};

#endif // PRODUCTCUBE_H
