#include "productcube.h"

ProductCube::ProductCube(QObject *parent) :
    QObject(parent)
{
    currentSize=0;
}
int ProductCube::getCubeSize() const
{
    return cubeSize;
}

void ProductCube::setCubeSize(int value)
{
    cubeSize = value;
}
QString ProductCube::getCubeID() const
{
    return CubeID;
}

void ProductCube::setCubeID(QString value)
{
    CubeID = value;
}

Produc *ProductCube::getCubeItem(int ind)
{
    if (ind<=currentSize)
    return CubeItems[ind];
}

void ProductCube::WriteToFile()
{

}

void ProductCube::AddCubeItem(Produc *item)
{
    //CubeItems.push_back(item);
    currentSize++;
}


