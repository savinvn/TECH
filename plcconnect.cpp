#include "plcconnect.h"
#include <QDebug>
#include <QTime>

PLCConnect::PLCConnect(QObject *parent, QString ip) :
    QObject(parent)
{
    Client = new TS7Client;
    Client->SetConnectionType(3); //устанавливаем тип подключения
    Client->ConnectTo("10.12.61.40",0,2);

    ClientL1 = new TS7Client;
    ClientL1->SetConnectionType(3); //устанавливаем тип подключения
    ClientL1->ConnectTo("10.12.61.41",0,2);

//    ClientL2 = new TS7Client;
//    ClientL2->SetConnectionType(3); //устанавливаем тип подключения
//    ClientL2->ConnectTo("10.12.61.42",0,2);


    RxTimer = new QTimer;
    RxTimer->setInterval(300);

    connect(RxTimer,SIGNAL(timeout()),this,SLOT(ReadFromPLC()));
//    connect(RxTimer,SIGNAL(timeout()),this,SLOT(ReadFromPLCL1()));

  //  connect(RxTimer,SIGNAL(timeout()),this,SLOT(ReadFromPLCL2()));

    RxTimer->start();
    StartCubeIND=0;
    //    CubeTimer = new QTimer;
    //    CubeTimer->setInterval(1000);

    //    connect(CubeTimer,SIGNAL(timeout()),this,SLOT(ReadStartCube()));
    //    CubeTimer->start();
}
QString PLCConnect::getServIP() const
{
    return ServIP;
}

void PLCConnect::setServIP(const QString &value)
{
    ServIP = value;
}

int PLCConnect::getMoveType() const
{
    return moveType;
}

void PLCConnect::setMoveType(int value)
{
    moveType = value;
}

void PLCConnect::setPacksType(int value)
{
    PacksType = value;
}

void PLCConnect::setD_time(int value)
{
    d_time = value;
}

void PLCConnect::setWritePremition(bool value)
{
    writePremition = value;
}

void PLCConnect::MakeStop1()
{
    emit RecivedData("stop");
    StartPrem1 = true;
}

void PLCConnect::MakeStop2()
{
    emit RecivedData("stop1");
    StartPrem2 = true;
}


//проверка выполнения команды для ПЛК
bool PLCConnect::Check(int Result, const char * function)
{
    if (Result!=0)
    {
        printf("| ERROR !!! \n");
        emit ConnectError(function);
    }
    if (Result<0)
    {
        printf("| Library Error (-1)\n");
    }
    else
    {
    }
    return Result==0;
}

void PLCConnect::ReadFromPLC()
{
    byte buffer0[1]; //буффер 1 байт
    byte buffer1[4]; //буффер 4   байта
    byte buffer2[1]; //буффер 4   байта

    TS7DataItem Items[3]; //массив данных
    TS7DataItem ItemsW[1]; //массив данных


//    ItemsW[0].Area=S7AreaDB; //область данных = DB
//    ItemsW[0].WordLen=S7WLBit; //тип данных  = byte
//    ItemsW[0].DBNumber=600  ; //номер бло    ка данных = 600
//    ItemsW[0].Start=1066; //стартовый адрес = 128
//    ItemsW[0].Amount=1; //количество байт = 1
//    ItemsW[0].pdata=&Mybuffer; //буффер 1

//    ItemsW[0].Area=S7AreaDB; //область данных = DB
//    ItemsW[0].WordLen=S7WLBit; //тип данных  = byte
//    ItemsW[0].DBNumber=600  ; //номер бло    ка данных = 600
//    ItemsW[0].Start=1064; //стартовый адрес = 128
//    ItemsW[0].Amount=1; //количество байт = 1
//    ItemsW[0].pdata=&PingBuff; //буффер 1


        ItemsW[0].Area=S7AreaDB; //область данных = DB
        ItemsW[0].WordLen=S7WLByte; //тип данных  = byte
        ItemsW[0].DBNumber=600  ; //номер бло    ка данных = 600      для второго лахена 133,0 133,2
        ItemsW[0].Start=139; //стартовый адрес = 128                  для первого лахена 139,0 139,1
        ItemsW[0].Amount=1; //количество байт = 1
        ItemsW[0].pdata=&bufferStop; //буффер 1




    int writeret = 1;
    writeret = Client->WriteMultiVars(&ItemsW[0],1);




    Items[0].Area=S7AreaDB; //область данных = DB
    Items[0].WordLen=S7WLByte; //тип данных  = byte
    Items[0].DBNumber=600  ; //номер блока данных = 600
    Items[0].Start=129; //стартовый адрес = 128
    Items[0].Amount=1; //количество байт = 1
    Items[0].pdata=buffer0; //буффер 2

    Items[1].Area=S7AreaDB; //область данных = DB
    Items[1].WordLen=S7WLByte; //тип данных  = byte
    Items[1].DBNumber=600  ; //номер блока данных = 11
    Items[1].Start=124; //стартовый адрес = 78
    Items[1].Amount=4; //количество байт = 2
    Items[1].pdata=buffer1; //буффер 2


    Items[2].Area=S7AreaDB; //область данных = DB
    Items[2].WordLen=S7WLByte; //тип данных  = byte
    Items[2].DBNumber=600  ; //номер блока данных = 11
    Items[2].Start=135; //стартовый адрес = 78
    Items[2].Amount=1; //количество байт = 2
    Items[2].pdata=buffer2; //буффер 2


    int res1=Client->ReadMultiVars(&Items[0],3); //читаем данные из ПЛК

    if (Check(res1,"Main")) //проверка выполнения команды чтения из ПЛК

    {
        if (Items[0].Result==0) //данные 2 ПЛК получены
        {
            //cam1
            I_photocell_pack1=(buffer0[0]&1<<1)!=0; //вход фотодатчика внешний роликовый конвейер загружен B68D2 (I1.0)

            //cam2
            I_photocell_pack2=(buffer0[0]&1<<2)!=0;
        }


        if (Items[1].Result==0) //данные 2 ПЛК получены
        {
            Pack_Height=(buffer1[0]<<8 | buffer1[1]);
            LayersN = (buffer1[2]<<8 | buffer1[3]);
            //emit State(QString::number(Pack_Height));
            //emit ProductHeight((int)Pack_Height);
            emit LayersCount((int)LayersN);
        }

        if (Items[2].Result==0) //данные 2 ПЛК получены
        {
            //cam1
            I_photocell_pack11=(buffer2[0]&1<<6)!=0; //вход фотодатчика внешний роликовый конвейер загружен B68D2 (I1.0)

            //cam2
            I_photocell_pack21=(buffer2[0]&1<<7)!=0;
        }


        if (!sig1State && I_photocell_pack1)
        {
            emit RecivedData("star");
        }
        if(sig1State && !I_photocell_pack1)
        {
            emit RecivedData("stop");
        }

        if (!sig2State && I_photocell_pack2)
        {
            emit RecivedData("star1");
        }
        if(sig2State && !I_photocell_pack2)
        {
            emit RecivedData("stop1");
        }


        if(!sig1StateL11 && I_photocell_pack11)
        {
            emit RecivedData("change");
        }


        if(!sig1StateL21 && I_photocell_pack21)
        {
            emit RecivedData("change1");
        }

        sig1State = I_photocell_pack1;
        sig2State = I_photocell_pack2;
        sig1StateL11 = I_photocell_pack11;
        sig1StateL21 = I_photocell_pack21;

    }
    else //данные не получены
    {
        do //цикл
        {
            Client->Disconnect(); //отключаемся от ПЛК
            QThread::sleep(2); //ожидание 1 cекундy
            printf("reconnect \n");
            Client->SetConnectionType(3); //устанавливаем тип подключения к ПЛК
            res1 = Client->ConnectTo("10.12.61.40",0,2); //подключаемся к ПЛК
        } while (res1!= 0 ); //пока нет успешного подключения

    }

}

void PLCConnect::ReadFromPLCL1()
{
    byte buffer0[1]; //буффер 1 байт
    byte buffer1[2]; //буффер 1 байт

    TS7DataItem Items[2]; //массив данных

    Items[0].Area=S7AreaDB; //область данных = DB
    Items[0].WordLen=S7WLByte; //тип данных  = byte
    Items[0].DBNumber=600  ; //номер бло    ка данных = 600
    Items[0].Start=0; //стартовый адрес = 128
    Items[0].Amount=1; //количество байт = 1
    Items[0].pdata=buffer0; //буффер 2


    //высота поддона
    Items[1].Area=S7AreaDB; //область данных = DB
    Items[1].WordLen=S7WLByte; //тип данных  = byte
    Items[1].DBNumber=600  ; //номер блока данных = 600
    Items[1].Start=2; //стартовый адрес = 2
    Items[1].Amount=2; //количество байт = 1
    Items[1].pdata=buffer1; //буффер 1


    int res1=ClientL1->ReadMultiVars(&Items[0],2); //читаем данные из ПЛК

    if (Check(res1,"LACH1")) //проверка выполнения команды чтения из ПЛК

    {

        if (Items[0].Result==0) //данные 2 ПЛК получены
        {
            //cam1
            I_photocell_pack1L1=(buffer0[0]&1<<0)!=0; //вход фотодатчика внешний роликовый конвейер загружен B68D2 (I1.0)
            I_photocell_pack1L1out=(buffer0[0]&1<<1)!=0; //вход фотодатчика внешний роликовый конвейер загружен B68D2 (I1.0)
            I_photocell_pack1L1Height=(buffer0[0]&1<<2)!=0; //высота измерена
            I_signal_empty_move1=(buffer0[0]&1<<3)!=0; //высота измерена
            I_signal_startMove =(buffer0[0]&1<<4)!=0;//конвейер движется на входе в лахен2
        }
        //enter lachenmeir2
        if (!I_signal_startMoveState && I_signal_startMove&&I_photocell_pack1L1&& !wasStarted1)
        {
            emit RecivedDataL1("star");
            wasStarted1=true;
        }
        if(sig1StateL1 && !I_photocell_pack1L1)
        {
            if(wasStarted1)
            {
                emit RecivedDataL1("stop");
                wasStarted1 = false;
            }
        }
        //exit lachenmeir 1
        if(!sig1StateL1out &&  I_photocell_pack1L1out)
        {
            emit RecivedDataL1("exit");
        }


        if(!sigStateL1EmptyMove&&I_signal_empty_move1)
        {
            emit Do_emptyMoveL1();

        }
        sig1StateL1 = I_photocell_pack1L1;
        sig1StateL1out = I_photocell_pack1L1out;
        sig1StateL1Height = I_photocell_pack1L1Height;
        sigStateL1EmptyMove = I_signal_empty_move1;
        I_signal_startMoveState = I_signal_startMove;

    }
    else //данные не получены
    {
        do //цикл
        {
            ClientL1->Disconnect(); //отключаемся от ПЛК
            QThread::sleep(2); //ожидание 1 cекундy
            printf("reconnect \n");
            ClientL1->SetConnectionType(3); //устанавливаем тип подключения к ПЛК
            res1 = ClientL1->ConnectTo("10.12.61.41",0,2); //подключаемся к ПЛК
        } while (res1!= 0 ); //пока нет успешного подключения

    }

}

void PLCConnect::ReadFromPLCL2()
{
//    byte buffer0[1]; //буффер 1 байт
//    TS7DataItem Items[1]; //массив данных

//    Items[0].Area=S7AreaDB; //область данных = DB
//    Items[0].WordLen=S7WLByte; //тип данных  = byte
//    Items[0].DBNumber=600  ; //номер бло    ка данных = 600
//    Items[0].Start=0; //стартовый адрес = 128
//    Items[0].Amount=1; //количество байт = 1
//    Items[0].pdata=buffer0; //буффер 2

//    int res1=ClientL2->ReadMultiVars(&Items[0],1); //читаем данные из ПЛК

//    if (Check(res1,"LACH2")) //проверка выполнения команды чтения из ПЛК

//    {
//        if (Items[0].Result==0) //данные 2 ПЛК получены
//        {
//            //cam1
//            I_photocell_pack1L2=(buffer0[0]&1<<0)!=0; //вход фотодатчика внешний роликовый конвейер загружен B68D2 (I1.0)
//            I_photocell_pack1L2out=(buffer0[0]&1<<1)!=0; //вход фотодатчика внешний роликовый конвейер загружен B68D2 (I1.0)
//            I_photocell_pack1L2Height=(buffer0[0]&1<<2)!=0; //высота измерена
//            I_signal_empty_move2=(buffer0[0]&1<<3)!=0; //высота измерена
//            I_signal_startMove2 =(buffer0[0]&1<<4)!=0;//конвейер движется на входе в лахен2
//        }
//        //enter lachenmeir2
//        if (!I_signal_startMove2State && I_signal_startMove2&&I_photocell_pack1L2)
//        {
//            emit RecivedDataL2("star");
//            wasStarted2=true;
//        }
//        if(sig1StateL2 && !I_photocell_pack1L2)
//        {
//            if(wasStarted2)
//            {
//                emit RecivedDataL2("stop");
//                wasStarted2 = false;
//            }
//        }
//        //exit lachenmeir2
//        if(!sig1StateL2out &&  I_photocell_pack1L2out)
//        {
//            emit RecivedDataL2("exit");
//        }


//        if(!sigStateL2EmptyMove&&I_signal_empty_move2)
//        {
//            emit Do_emptyMoveL2();
//        }
//        sig1StateL2 = I_photocell_pack1L2;
//        sig1StateL2out = I_photocell_pack1L2out;
//        sig1StateL2Height = I_photocell_pack1L2Height;
//        sigStateL2EmptyMove = I_signal_empty_move2;
//        I_signal_startMove2State = I_signal_startMove2;

//    }

//    else //данные не получены
//    {
//        do //цикл
//        {
//            ClientL2->Disconnect(); //отключаемся от ПЛК
//            QThread::sleep(2); //ожидание 1 cекундy
//            printf("reconnect \n");
//            ClientL2->SetConnectionType(3); //устанавливаем тип подключения к ПЛК
//            res1 = ClientL2->ConnectTo("10.12.61.42",0,2); //подключаемся к ПЛК
//        } while (res1!= 0 ); //пока нет успешного подключения

//    }
}

void PLCConnect::ReadStartCube()
{

}

void PLCConnect::WriteToPLC()
{
    //    int writeret = 1;
    //    bool buffer[1] = {false}; //буффер 4 байта
    //    TS7DataItem Items[1]; //массив данных

    //    Items[0].Area=S7AreaMK; //область данных = PE
    //    Items[0].WordLen=S7WLBit; //тип данных  = byte
    //    Items[0].DBNumber=0  ; //номер блока данных = 0
    //    Items[0].Start=21; //стартовый адрес = M2.5
    //    Items[0].Amount=1; //количество бит = 1
    //    Items[0].pdata=&buffer; //буффер
    //    buffer[0]=true;
    //    writeret = Client->WriteMultiVars(&Items[0],1);

}
