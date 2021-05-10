#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QThread>

#include <QFile>

#include <QXmlStreamWriter>
#include <QModelIndex>
#include <QTableWidgetItem>

#include "cameraconnection.h"
#include "barcodeprocessing.h"
#include "arduinoserial.h"
#include "productcube.h"
#include "product.h"
#include "plcconnect.h"
#include "verycam.h"

#include <QQueue>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    CameraConnection *camcon;
    BarCodeProcessing *bcp;
    VeryCam *v_cam;

    CameraConnection *camcon1;
    BarCodeProcessing *bcp1;
    VeryCam *v_cam1;

    QVector<QString> nextId1;

    QVector<QString> nextId2;

    QSqlDatabase db_Main;

    CameraConnection *camcon2;
    BarCodeProcessing *bcp2;
    VeryCam *v_cam2;



    int countOnLach=0;
    int countOnConv =0;
    int countOnRead = 0;

    int countGood;
    int countError;
    int countAll;

    bool qr_found, ean13_found;
    bool qr_found1, ean13_found1;
private slots:

    void ReadSettingsFile();
    void on_pushButton_clicked();
    void Draw_Frame(cv::Mat m);

    void on_pushButton_2_clicked();
    void Check_serial(QString str);

    void DrawThresh(cv::Mat a);
    void DrawThresh1(cv::Mat a);
    void DrawThresh2(cv::Mat a);
    void DrawThresh3(cv::Mat a);

    void StopCam();
    void StopCam1();
    void StopCam2();
    void StopCam3();

    void RecieveCommand(QString command);
    void RecieveCommandL1(QString command);
    void RecieveCommandL2(QString command);
    void RecieveCommandBA(QByteArray bytearr);
    void on_pushButton_3_clicked();

    void on_action_triggered();

    void on_Settings_OK_clicked();


    void FullCubeReady();
    void FullCubeReady1();

    void on_pushButton_6_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_7_clicked();





    void on_pushButton_exit_clicked();

    void on_eanQrMode_clicked();

    void on_eanMode_clicked();

    void delLastPack();
    void on_dropControl_clicked();

    void on_video_ENB_clicked();

    void on_videoClose_clicked();

    void CameraError();

    void on_del_cur_cube_clicked();

    void ErrorPLC(QString pN);
    void on_cam1exp_valueChanged(int arg1);

    void on_cam2exp_valueChanged(int arg1);

    void StartCube(int num);

    void on_spinBox_2_valueChanged(int arg1);
    void SirenStop();

    void on_del_selected_Pack_clicked();



    void on_applyPartNum_clicked();





    void on_strt_clicked();

    void on_ProdQuantity_valueChanged(int arg1);

    void on_foc_clicked();

    void on_foc1_clicked();
    void move1obj();

    void move2obj();

    void ChangePackHeight(int h);
    void ChangeLayerN(int n);

    void on_testdb_clicked();

    void on_testCube_clicked();

    void on_exitLach1_clicked();

    void on_StartId_clicked();

    void on_stopID_clicked();

    void on_enb_lach1_clicked();

    void on_enbLach2_clicked();


    void EmptyMove1();
    void EmptyMove2();

    void on_emptyExit1_clicked();

    void on_emptyExit2_clicked();

    void on_Lach1OFF_clicked();

    void on_Lach2OFF_clicked();

    void delayEmptyMove1();

    void delayEmptyMove2();

    void on_test_cams_clicked();

    void on_test2_clicked();

private:

    bool EmptyEx1 = false;
    bool EmptyEx2 = false;

    bool blockL1Start = false;
    bool blockL2Start = false;


    int readLach = 0;

    bool enb_lach1 = true;
    bool enb_lach2 = true;


    QQueue<QVector<QString>> idQueue; //Lach1
     QSqlDatabase dbAgr;

    QVector<QString> TopLayerID_L1;
    QQueue<QVector<QString>> idChangeQueue;
    QQueue<QVector<QString>> idQueueOut; //Lach1exit
    QQueue<int> PackCountQueue;

    QQueue<QVector<QString>> idQueue2; //Lach2

    QVector<QString> TopLayerID_L2;

    QQueue<QVector<QString>> idChange2Queue;
    QQueue<QVector<QString>> idQueue2Out; //Lach2exit
    int LayerN=0;
    QTimer *t1;
    QTimer *t2;

    QTimer * sysStart;

    int currentPackHeight=0;
    int stepsToFocus;
    QDateTime PackVeryfiedDateTime;
    QString pass;
    QString LampLastState ="0";
    QString Plc_IP;
    QString Cam1_serial;
    QString Cam2_serial;
    QString Cam3_serial;



    int svet_port;
    int Ard_port;

    QTimer *sirenStopTimer;

    QTimer *EmptyMoveButton1_Timer;

    QTimer *EmptyMoveButton2_Timer;

    QString curCubeQR;
    bool isDrop;


    int NumPart=0;
    QString V_Ean;
    QString V_QR;

    QString V_Ean1;
    QString V_QR1;


    bool exitcam3Started = false;

    bool exitcam4Started = false;



    bool enbDrop = true;
    std::queue<Produc*> packsToWrite;

    int allCounter=0;
    bool isStarted=false;
    int cube_count =0;
    QFile *file;
    QXmlStreamWriter stream;
    ProductCube *currentCube;
    ProductCube *currentCube1;
    Produc *currentProd;

    QString VerifyQR;
    Ui::MainWindow *ui;
    QThread *video_thread;
    QThread *work_thread;
    QThread *ard_thread;
    QThread *svetofor_thread;

    QThread *video_thread1;
    QThread *work_thread1;

    QThread *video_thread2;
    QThread *work_thread2;

    QThread *video_thread3;
    QThread *work_thread3;

    QThread * plc_thread;

    cv::Mat temp;
    cv::Mat temp1;
    QStringList codes;

    ArduinoSerial *as;
    ArduinoSerial *svetofor;
    int fn=0;

    int tempID =1;
    int tempID1 =1;
    QQueue <int> tempIdQueue;
    std::queue<Produc*> writeQueue;
    QString replaceInd;
    QString replaceInd2;

    int replaceIndH;

    int replaceIndL2;
    int replaceIndHL2;


    QString currentBarScanData;
    QString man_CubeID;


    QString DateTimeW;
    QString idW;
    QString QrW;
    int TempIDW;
    int ccW;
    bool enbDropW;



    int mode;

    int AutoMode;



    QModelIndex lastIndex;

    QString LastEan;


    PLCConnect *plc;

    bool firsttime=true;
    bool firsttimeA=true;

    bool isSystemStarted = false;

signals:
    void Request(bool);
    void Request1(bool);

    void Request2(bool);

    void Request3(bool);

    void Drop();
    void Lamp(QString);
    void BlinkLamp(QString);
    void move(QByteArray);
};

#endif // MAINWINDOW_H
