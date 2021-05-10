#ifndef PLCCONNECT_H
#define PLCCONNECT_H

#include <QObject>

#include <QTimer>
#include <QThread>
#include <QElapsedTimer>
#include "snap7.h"

class PLCConnect : public QObject
{
    Q_OBJECT
public:
    explicit PLCConnect(QObject *parent = 0,QString ip = "");

    QString getServIP() const;
    void setServIP(const QString &value);

    int getMoveType() const;
    void setMoveType(int value);

    void setPacksType(int value);

    void setD_time(int value);

    void setWritePremition(bool value);
    byte bufferStop[1];
     bool Mybuffer[1] = {false};
     bool PingBuff[1]={true};
private:
    bool writePremition=false;
    QElapsedTimer t;
    int d_time;
    int moveType=0;
    int PacksType=0;
    QString ServIP;
    QTimer *RxTimer;

    QTimer *CubeTimer;


    QTimer *StopTimer;
    QTimer *StopTimer2;


    TS7Client *Client; //клиентское подключение
    TS7Client *ClientL1; //клиентское подключение
    TS7Client *ClientL2; //клиентское подключение

    bool I_photocell_external_roller_conveyor_busy;

    bool I_photocell_pack1;

    bool I_photocell_pack11;
    bool sig1StateL11 = false;


    bool I_photocell_pack1L1;
    bool I_photocell_pack1L1out;
    bool I_photocell_pack1L1Height;
    bool I_signal_empty_move1;
    bool I_signal_startMove;
    bool I_signal_startMoveState;


    bool I_photocell_pack1L2;
    bool I_photocell_pack1L2out;
    bool I_photocell_pack1L2Height;
    bool I_signal_empty_move2;
    bool I_photocell_pack2;
    bool I_signal_startMove2;
bool I_signal_startMove2State;


    bool I_photocell_pack21;
    bool sig1StateL21 = false;


    int StartCubeIND;
    int PrevValue=0;

    bool I_photocell_signal_inlet_stopper_up;
    bool I_signal_inlet_stopper_down;
    bool I_signal_pinza_pack2;


bool StartPrem1=true;
bool StartPrem2=true;

    bool cam1Start = false;
    bool cam2Start = false;
    bool cam3Start = false;

    bool sig1State = false;
    bool sig1StateL1 = false;
    bool sig1StateL2 = false;
    bool sig1StateL1out = false;
    bool sig1StateL2out = false;

    bool sig1StateL1Height = false;
    bool sig1StateL2Height = false;

    bool sigStateL1EmptyMove = false;
    bool sigStateL2EmptyMove = false;



    bool sig2State = false;
    double Pack_Height;
    double Palete1_Height;
    double LayersN;


    bool wasStarted1=false;

    bool wasStarted2 = false;
//    bool StopperPhotocell2 = false;
//    bool sig_WriteDB = false;
signals:
  void RecivedData(QString);
  void RecivedDataL1(QString);
  void RecivedDataL2(QString);
  void RecivedDataA(QString);
  void ConnectError(QString);
  void StartNewCube(int);
  void ProductHeight(int);
  void LayersCount(int);
  void Do_emptyMoveL1();
  void Do_emptyMoveL2();
public slots:

    void MakeStop1();
    void MakeStop2();
  bool Check(int Result, const char * function);


    void ReadFromPLC();
    void ReadFromPLCL1();
    void ReadFromPLCL2();
    void ReadStartCube();
    void WriteToPLC();

};

#endif // PLCCONNECT_H
