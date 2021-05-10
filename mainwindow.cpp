#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv2/highgui.hpp"

#include <QMessageBox>
#include <QProcess>
#include <QSettings>

#include <QFileDialog>
#include <QFile>
#include <QDateTime>
#include <QTextStream>

#include <QDebug>
#include <QInputDialog>
#include <QBuffer>

using namespace cv;
cv::Mat open;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    // должно быть 3 камеры
    Cam1_serial="193.168.1.2";
    Cam2_serial ="194.168.1.2" ;
    Cam3_serial="195.168.1.2";

    Plc_IP = "10.12.61.40";
    Ard_port = 3;
    svet_port = 4;

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC","MyDB");
    db.setDatabaseName("DRIVER={SQL Server Native Client 11.0};SERVER=localhost\\SQLExpress;DATABASE=TECH_LABEL_CONTROL;UID=sa;PWD=1234;WSID=.;");
    db.open();

    dbAgr = QSqlDatabase::addDatabase("QODBC","MyDB_A");
    dbAgr.setDatabaseName("DRIVER={SQL Server Native Client 11.0};SERVER=10.12.61.89\\SQLExpress;DATABASE=TECH_LABEL_CONTROL;UID=sa;PWD=1234;WSID=.;");
    dbAgr.open();

    ui->groupBox_8->setVisible(false);

    ui->pushButton->setVisible(false);

    ard_thread = new QThread();
    as = new ArduinoSerial(0,Ard_port);
    as->PortN=Ard_port;
    as->moveToThread(ard_thread);
    as->serial.moveToThread(ard_thread);
    ard_thread->start();
    connect(this,SIGNAL(move(QByteArray)),as,SLOT(WriteToPort(QByteArray)));


    V_Ean = "nan";
    V_QR = "nan";
    VerifyQR ="";

    plc = new PLCConnect(0,Plc_IP);
    plc_thread = new QThread();
    plc->moveToThread(plc_thread);
    connect(this,SIGNAL(Drop()),plc,SLOT(WriteToPLC()));
    connect (plc,SIGNAL(ConnectError()),this,SLOT(ErrorPLC()));
    connect(plc,SIGNAL(ProductHeight(int)),this,SLOT(ChangePackHeight(int)));

    connect(plc,SIGNAL(LayersCount(int)),this,SLOT(ChangeLayerN(int)));

    connect(plc,SIGNAL(Do_emptyMoveL1()),this,SLOT(EmptyMove1()));

    connect(plc,SIGNAL(Do_emptyMoveL2()),this,SLOT(EmptyMove2()));

    plc_thread->start();

    svetofor_thread = new QThread();
    svetofor = new ArduinoSerial(0,svet_port);
    svetofor->PortN=svet_port;
    svetofor->moveToThread(svetofor_thread);
    svetofor->serial.moveToThread(svetofor_thread);
    connect(this,SIGNAL(Lamp(QString)),svetofor,SLOT(WriteToPort(QString)));
    svetofor_thread->start();

    //ui->Mode->setText("Режим автоматической сборки  куба");
    ui->stackedWidget->setCurrentIndex(0);

    //    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    //    settings.setValue("AddLib.exe", QCoreApplication::applicationFilePath().replace('/','\\'));
    countAll = 0;
    countError = 0;
    countGood =0;
    //    ui->label->setVisible(false);
    // ui->Threshold_slider->setVisible(false);
    qr_found = false;
    ean13_found = false;

    qr_found1 = false;
    ean13_found1 = false;



    currentProd = new Produc();

    mode =2;
    AutoMode =0;

   on_pushButton_clicked();



    ui->enb_lach1->setStyleSheet("QPushButton { background-color : green; }");


    sysStart = new QTimer;
    sysStart->singleShot(15000, this,SLOT(on_pushButton_2_clicked()));
    ui->pushButton_2->setVisible(false);


    EmptyMoveButton1_Timer = new QTimer;
    EmptyMoveButton2_Timer = new QTimer;

    ui->test2->setVisible(false);
    ui->test_cams->setVisible(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReadSettingsFile()
{
    QFile file("settings.txt");
    QTextStream settingsText(&file);

    if(file.open(QIODevice::ReadOnly |QIODevice::Text))
    {
        Plc_IP = settingsText.readLine();
        Cam1_serial=settingsText.readLine();
        Cam2_serial = settingsText.readLine();
        Ard_port = settingsText.readLine().toInt();
        emit Lamp(QString("4"));
        LampLastState = "4";
    }
    else
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Ошибка","Файл Настроек не найден");
        messageBox.setFixedSize(500,200);
        emit Lamp(QString("4"));
        LampLastState = "4";
        return;
    }
}

void MainWindow::on_pushButton_clicked()
{

    //FIRST CAMERA LAHEN должно быть 2 камеры
    camcon = new CameraConnection(0);  //"40043746""40025598"
    camcon->initSome();
    camcon->setSerial(Cam1_serial);
    camcon->OpenCamera();
    video_thread = new QThread();
    camcon->moveToThread(video_thread);

//    //connect(camcon,SIGNAL(GrabResult(QString)),ui->label,SLOT(setText(QString)));

    connect (this,SIGNAL(Request(bool)),camcon,SLOT(GrabImage(bool)));

    work_thread = new QThread;
    bcp = new BarCodeProcessing();
    bcp->setFolderName("cam1");
    bcp->moveToThread(work_thread);

    v_cam = new VeryCam(nullptr,1);
    connect(bcp,SIGNAL(CodeQr(QString)),v_cam,SLOT(verifyQR(QString )));
    work_thread->start();
    video_thread->start();

    //SECOND CAMERA LAHEN
    camcon1 = new CameraConnection(0);  //"40043746""40025598"
    camcon1->initSome();
    camcon1->setSerial(Cam2_serial);
    camcon1->OpenCamera();
    video_thread1 = new QThread();
    camcon1->moveToThread(video_thread1);
    connect (this,SIGNAL(Request1(bool)),camcon1,SLOT(GrabImage(bool)));

    work_thread1 = new QThread;
    bcp1 = new BarCodeProcessing();
    bcp1->setFolderName("cam2");
    bcp1->moveToThread(work_thread1);



    v_cam1 = new VeryCam(nullptr,1);
    connect(bcp1,SIGNAL(CodeQr(QString)),v_cam1,SLOT(verifyQR(QString )));
    work_thread1->start();
    video_thread1->start();




   // FIRST CAMERA EXIT
    camcon2 = new CameraConnection(0);  //"40043746""40025598"
    camcon2->initSome();
    camcon2->setSerial(Cam3_serial);
    camcon2->OpenCamera();
    video_thread2 = new QThread();
    camcon2->moveToThread(video_thread2);
    connect (this,SIGNAL(Request2(bool)),camcon2,SLOT(GrabImage(bool)));

    work_thread2 = new QThread;
    bcp2 = new BarCodeProcessing();
    bcp2->setFolderName("cam3");
    bcp2->moveToThread(work_thread2);


    v_cam2 = new VeryCam(nullptr,2);
    connect(bcp2,SIGNAL(Code(QString)),v_cam2,SLOT(verifyEAN(QString )));
    connect(bcp2,SIGNAL(CodeQr(QString)),v_cam2,SLOT(verifyQR(QString )));

    connect(v_cam2,SIGNAL(stopCam()),this,SLOT(StopCam2()));

    work_thread2->start();
    video_thread2->start();





    ui->pushButton->setDisabled(true);





    connect(bcp2,SIGNAL(ThreshMat(cv::Mat)),this,SLOT(DrawThresh2(cv::Mat)),Qt::DirectConnection);


    int bits[8];
    bits[0]=0;
    bits[1]=0;
    bits[2]=0;
    bits[3]=0;
    bits[4]=0;
    bits[5]=0;
    bits[6]=0;
    bits[7]=1;

    for (int i = 0;i<8;i++)
    {
        plc->bufferStop[0] = plc->bufferStop[0]<<1;
        plc->bufferStop[0] += bits[i] ;
    }

}

void MainWindow::Draw_Frame(cv::Mat m)
{
}

void MainWindow::on_pushButton_2_clicked()
{


       connect(camcon,SIGNAL(FrameReady(cv::Mat)),bcp,SLOT(ProcessFrame(cv::Mat)));
       connect(camcon1,SIGNAL(FrameReady(cv::Mat)),bcp1,SLOT(ProcessFrame(cv::Mat)));
       connect(camcon2,SIGNAL(FrameReady(cv::Mat)),bcp2,SLOT(ProcessFrame(cv::Mat)));



    connect(plc,SIGNAL(RecivedData(QString)), this,SLOT(RecieveCommand(QString)));
    connect(plc,SIGNAL(RecivedDataL1(QString)), this,SLOT(RecieveCommandL1(QString)));
    connect(plc,SIGNAL(RecivedDataL2(QString)), this,SLOT(RecieveCommandL2(QString)));
    ui->pushButton_2->setDisabled(true);
}

void MainWindow::Check_serial(QString str)
{
    if(str == "another cam")
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Ошибка","Подключите оригинальную камеру.");
        messageBox.setFixedSize(500,200);
    }
    else
        ui->statusBar->showMessage("Камера подключена.");
}


void MainWindow::DrawThresh(Mat a)
{
    try{

        QImage * qi = new QImage(a.data , a.cols, a.rows,a.step, QImage::Format_RGB888);
        ui->Cam1->setPixmap(QPixmap::fromImage( *qi).scaled(ui->Cam1->size()));
        ui->Cam1->update();
        delete qi;
    }
    catch(std::exception& e)
    {

    }
}

void MainWindow::DrawThresh1(Mat a)
{
    try{

        QImage * qi = new QImage(a.data , a.cols, a.rows,a.step, QImage::Format_RGB888);
        ui->Cam2->setPixmap(QPixmap::fromImage( *qi).scaled(ui->Cam2->size()));
        ui->Cam2->update();
        delete qi;
    }
    catch(std::exception& e)
    {

    }

}

void MainWindow::DrawThresh2(Mat a)
{
    try{

        if(!a.empty())
        {
            QImage * qi = new QImage(a.data , a.cols, a.rows,a.step, QImage::Format_RGB888);
            ui->Cam3->setPixmap(QPixmap::fromImage( *qi).scaled(ui->Cam3->size()));
            ui->Cam3->update();
            delete qi;
        }
    }
    catch(std::exception& e)
    {

    }
}

void MainWindow::DrawThresh3(Mat a)
{


}

void MainWindow::StopCam()
{
    disconnect(bcp,SIGNAL(ProcessingRequest(bool)),camcon,SLOT(GrabImage(bool)));
}

void MainWindow::StopCam1()
{
    disconnect(bcp1,SIGNAL(ProcessingRequest(bool)),camcon1,SLOT(GrabImage(bool)));
}

void MainWindow::StopCam2()
{
    disconnect(bcp2,SIGNAL(ProcessingRequest(bool)),camcon2,SLOT(GrabImage(bool)));

    readLach=0;
    int bits[8];
    bits[0]=0;
    bits[1]=0;
    bits[2]=0;
    bits[3]=0;
    bits[4]=0;  //        133    // 139
    bits[5]=0;  //data  //  0
    bits[6]=readLach;//   0   //   data
    bits[7]=1;         //  1  //    1


    for (int i = 0;i<8;i++)
    {
        plc->bufferStop[0] = plc->bufferStop[0]<<1;
        plc->bufferStop[0] += bits[i] ;
    }

}

void MainWindow::StopCam3()
{


}


void MainWindow::RecieveCommand(QString command) // камеры на выходе смотрят QR и CODE128
{
try
    {
    
    if (command=="star" && enb_lach1&&!exitcam3Started) // начало съемки камеры на выходе
    {
        if(idChangeQueue.size()>0)
        {

                    QSqlQuery querygetData(QSqlDatabase::database("MyDB"));
                    querygetData.prepare("select countQR from L1_vis where pos = 1");
                    querygetData.exec();
                    int count =0;
                    if(querygetData.next())
                    {
                        count = querygetData.value(0).toInt(); //Дата-время последней записи на сервере(по шкафу №2)
                    }

                    QSqlQuery querysetData(QSqlDatabase::database("MyDB"));
                    querysetData.prepare("update L1_vis set countQR = :count where pos = 2");
                    querysetData.bindValue(":count",count);
                    querysetData.exec();

                    QSqlQuery querysetData1(QSqlDatabase::database("MyDB"));
                    querysetData1.prepare("update L1_vis set countQR = :count where pos = 1");
                    querysetData1.bindValue(":count",0);
                    querysetData1.exec();

//            QSqlQuery querysetData1(QSqlDatabase::database("MyDB"));
//            querysetData1.prepare("update L1_vis set countQR = :count where pos = 2");
//            querysetData1.bindValue(":count",0);
//            querysetData1.exec();

            exitcam3Started = true;
            nextId1 = idChangeQueue.dequeue();
            ui->L1_QueueChange->setNum(idChangeQueue.size());
            bcp2->setLastGoodFrame(1);
            bcp2->mv.clear();


            connect(bcp2,SIGNAL(ProcessingRequest(bool)),camcon2,SLOT(GrabImage(bool)));
            emit Request2(true);
            v_cam2->ean13_found=false;
            v_cam2->qr_found=false;

            v_cam2->QR = "nan";
            v_cam2->EAN = "nan";
            firsttime = false;
            bcp2->setQrFound(false);
            bcp2->setEanFound(false);

            readLach=1;

            int bits[8];
            bits[0]=0;
            bits[1]=0;
            bits[2]=0;
            bits[3]=0;
            bits[4]=0;
            bits[5]=0;
            bits[6]=readLach;
            bits[7]=1;

            for (int i = 0;i<8;i++)
            {
                plc->bufferStop[0] = plc->bufferStop[0]<<1;
                plc->bufferStop[0] += bits[i] ;
            }
        }
    }

    if (command == "stop" && exitcam3Started && enb_lach1)
    {
        exitcam3Started = false;

        StopCam2();
        if (firsttime)
        {
            firsttime = false;
            return;
        }
        //emit Request2(false);



        if(!EmptyEx1)
        {

            QSqlQuery querysetData(QSqlDatabase::database("MyDB"));
            querysetData.prepare("update L1_vis set countQR = :count where pos = 2");
            querysetData.bindValue(":count",0);
            querysetData.exec();
            if(v_cam2->EAN!="nan"&&v_cam2->QR!="nan")
            {

                QSqlQuery InsertCube(QSqlDatabase::database("MyDB"));
                InsertCube.prepare("insert into Cube(ID,datetimeCube,codeproduct, LachID) values(:id,:datet,:qr,:l_id)");
                InsertCube.bindValue(":id",v_cam2->QR);
                InsertCube.bindValue(":datet",QDateTime::currentDateTime());
                InsertCube.bindValue(":qr",v_cam2->EAN);
                InsertCube.bindValue(":l_id",1);
                qDebug()<<InsertCube.exec()<<endl;


                for (int i = 0; i<nextId1.size();i++)
                {
                QSqlQuery InsertLINK(QSqlDatabase::database("MyDB"));
                InsertLINK.prepare("insert into Link(ID,QR,datetimeLink,LachID) values(:id,:qr,:dt,:la)");
                InsertLINK.bindValue(":id",nextId1[i]);
                InsertLINK.bindValue(":qr",v_cam2->QR);
                InsertLINK.bindValue(":dt",QDateTime::currentDateTime());
                InsertLINK.bindValue(":la",1);

                qDebug()<<InsertLINK.exec()<<endl;
                }

            }
            if(v_cam2->EAN=="nan"&&v_cam2->QR=="nan")
            {
                bcp2->writemat();

                QSqlQuery InsertCube(QSqlDatabase::database("MyDB"));
                InsertCube.prepare("insert into Cube(ID,datetimeCube,codeproduct, LachID) values(:id,:datet,:qr,:l_id)");
                InsertCube.bindValue(":id",v_cam2->QR);
                InsertCube.bindValue(":datet",QDateTime::currentDateTime());
                InsertCube.bindValue(":qr",v_cam2->EAN);
                InsertCube.bindValue(":l_id",1);
                qDebug()<<InsertCube.exec()<<endl;


                for (int i = 0; i<nextId1.size();i++)
                {
                    QSqlQuery InsertLINK(QSqlDatabase::database("MyDB"));
                    InsertLINK.prepare("insert into Link(ID,QR,datetimeLink,LachID) values(:id,:qr,:dt,:la)");
                    InsertLINK.bindValue(":id",nextId1[i]);
                    InsertLINK.bindValue(":qr",v_cam2->QR);
                    InsertLINK.bindValue(":dt",QDateTime::currentDateTime());
                    InsertLINK.bindValue(":la",1);

                qDebug()<<InsertLINK.exec()<<endl;
                }

            }


        }
        EmptyEx1=false;

    }



    if(command=="change" && enb_lach1)
    {
        if(idQueueOut.size()>0)
            idChangeQueue.enqueue(idQueueOut.dequeue());

        ui->L1_QueueOut->setNum(idQueueOut.size());
        ui->L1_QueueChange->setNum(idChangeQueue.size());

        QSqlQuery querysetData(QSqlDatabase::database("MyDB"));
        querysetData.prepare("update L1_vis set countQR = :count where pos = 1");
        querysetData.bindValue(":count",countOnConv);
        querysetData.exec();



    }




    }
    catch(std::exception& e)
    {

    }
}

void MainWindow::RecieveCommandL1(QString command) //чтение этикеток верхнего слоя лахен1
{
    try {

        if (command=="star" && enb_lach1 && !blockL1Start) // Начало съемки на лахене
        {
            bcp->setLastGoodFrame(1);
            bcp->mv.clear();

            connect(bcp,SIGNAL(ProcessingRequest(bool)),camcon,SLOT(GrabImage(bool)));
            emit Request(true);
            v_cam->qr_found=false;
            v_cam->ean13_found = true;
            v_cam->QR = "nan";
            firsttime = false;

            v_cam->QRs.clear();
            v_cam->QRs.resize(0);            

            bcp1->setLastGoodFrame(1);
            bcp1->mv.clear();

            connect(bcp1,SIGNAL(ProcessingRequest(bool)),camcon1,SLOT(GrabImage(bool)));
            emit Request1(true);
            v_cam1->qr_found=false;
            v_cam1->ean13_found = true;
            v_cam1->QR = "nan";


            v_cam1->QRs.clear();
            v_cam1->QRs.resize(0);




        }

        if (command == "stop"&& enb_lach1&&!blockL1Start)
        {
            StopCam();
            StopCam1();
            if (firsttime)
            {
                firsttime = false;
                return;
            }

            QVector<QString> resultQR;

            resultQR=v_cam->QRs+v_cam1->QRs;

            if(resultQR.size()>0)
            {
                ui->label->setText(resultQR[0]);

                emit Lamp(QString("3"));
                if(idQueue.size()< 1)
                    idQueue.enqueue(resultQR);
                else
                {
                    idQueue.dequeue();
                    idQueue.enqueue(resultQR);
                }


                QSqlQuery querygetCount(QSqlDatabase::database("MyDB_A"));
                querygetCount.prepare("select  count(id) from pack where  qr !='nan' and id in (select id from pack where qr = :qr)  ");
                querygetCount.bindValue(":qr",resultQR[0]);

                qDebug()<<querygetCount.exec()<<endl;



                int count =0;
                if(querygetCount.next())
                {
                    count = querygetCount.value(0).toInt(); //Дата-время последней записи на сервере(по шкафу №2)
                }

                QSqlQuery querygetData(QSqlDatabase::database("MyDB"));
                querygetData.prepare("update L1_vis set countQR = :count where pos = 0");
                querygetData.bindValue(":count",count);
                querygetData.exec();


                emit Lamp(QString("3"));
                ui->L1_Queue->setNum(idQueue.size());
            }
            else
            {
                QSqlQuery querygetData(QSqlDatabase::database("MyDB"));
                querygetData.prepare("update L1_vis set countQR = :count where pos = 0");
                querygetData.bindValue(":count",0);
                querygetData.exec();

                bcp->writemat();
                bcp1->writemat();

                emit Lamp(QString("1"));
                QVector<QString> nullV;
                nullV.push_back("null1");
                 ui->label->setText(nullV[0]);
                if(idQueue.size()< 1)
                idQueue.enqueue(nullV);
                else
                {
                    idQueue.dequeue();
                    idQueue.enqueue(nullV);
                }
                 ui->L1_Queue->setNum(idQueue.size());
            }
        }
        if(command=="exit"&& enb_lach1)
        {
            if(blockL1Start)
                blockL1Start = false;
            if(idQueue.size()>0)
                idQueueOut.enqueue(idQueue.dequeue());
             ui->L1_Queue->setNum(idQueue.size());
             ui->L1_QueueOut->setNum(idQueueOut.size());


             QSqlQuery querygetData(QSqlDatabase::database("MyDB"));
             querygetData.prepare("select countQR from L1_vis where pos = 0");
             querygetData.exec();
             //int count =0;
             if(querygetData.next())
             {
                 countOnConv = querygetData.value(0).toInt(); //Дата-время последней записи на сервере(по шкафу №2)
             }



             QSqlQuery querysetData1(QSqlDatabase::database("MyDB"));
             querysetData1.prepare("update L1_vis set countQR = :count where pos = 0");
             querysetData1.bindValue(":count",0);
             querysetData1.exec();


        }
    }
    catch(std::exception& e)
    {

    }

}

void MainWindow::RecieveCommandL2(QString command) //чтение этикеток верхнего слоя лахен 2 УБРАТЬ
{

}


void MainWindow::RecieveCommandBA(QByteArray bytearr)
{
    QString str = QString(bytearr);
    ui->statusBar->showMessage(str);
}

void MainWindow::on_pushButton_3_clicked()
{
}

void MainWindow::on_action_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_Settings_OK_clicked()
{
    //ui->Mode->setText("Режим автоматической сборки  куба");
    ui->stackedWidget->setCurrentIndex(0);
}



void MainWindow::FullCubeReady()
{
}

void MainWindow::FullCubeReady1()
{

}

void MainWindow::on_pushButton_6_clicked()
{
    // ui->Mode->setText("Режим ручной сборки  куба");
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_11_clicked()
{
    // ui->Mode->setText("Режим автоматической сборки  куба");
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_7_clicked()
{
}


void MainWindow::on_pushButton_exit_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Введите пароль"),tr("Введите:"), QLineEdit::Password,QString(), &ok);
    if (ok && text=="roadmin")
    {
        this->close();
    }
    else
    {
        QMessageBox message ;
        message.setWindowTitle("Неправаильный пароль");
        message.setText("Неправаильный пароль");
        message.exec();
        return;
    }
}

void MainWindow::on_eanQrMode_clicked()
{
    connect(bcp,SIGNAL(CodeQr(QString)),this,SLOT(VerifyCodeQR(QString)));
}

void MainWindow::on_eanMode_clicked()
{
    disconnect(bcp,SIGNAL(CodeQr(QString)),this,SLOT(VerifyCodeQR(QString)));
}

void MainWindow::delLastPack()
{

    if(packsToWrite.size()>1)
    {
        packsToWrite.pop();
        packsToWrite.pop();
    }
}

void MainWindow::on_dropControl_clicked()
{
}

void MainWindow::on_video_ENB_clicked()
{
    connect(bcp,SIGNAL(ThreshMat(cv::Mat)),this,SLOT(DrawThresh(cv::Mat)),Qt::DirectConnection);
    connect(bcp1,SIGNAL(ThreshMat(cv::Mat)),this,SLOT(DrawThresh1(cv::Mat)),Qt::DirectConnection);


    //  ui->Mode->setText("Режим просмотра камер");
    ui->stackedWidget->setCurrentIndex(2);

}

void MainWindow::on_videoClose_clicked()
{
    disconnect(bcp,SIGNAL(ThreshMat(cv::Mat)),this,SLOT(DrawThresh(cv::Mat)));
    disconnect(bcp1,SIGNAL(ThreshMat(cv::Mat)),this,SLOT(DrawThresh1(cv::Mat)));


    //  ui->Mode->setText("Режим автоматической сборки  куба");
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::CameraError()
{
    this->close();
}

void MainWindow::on_del_cur_cube_clicked()
{


}

void MainWindow::ErrorPLC(QString pN)
{
    QMessageBox messageBox;
    messageBox.critical(0,"Ошибка","Отсутствует соединение с контролером."+pN+"\nПроверьте подключение. Перезапустите программу");
    messageBox.setFixedSize(500,200);
    emit Lamp(QString("4"));
    LampLastState = "4";

    return;
}

void MainWindow::on_cam1exp_valueChanged(int arg1)
{
    // camcon->setExpos(arg1);
}

void MainWindow::on_cam2exp_valueChanged(int arg1)
{

}

void MainWindow::StartCube(int num)
{

}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
}

void MainWindow::SirenStop()
{
    emit Lamp("1");
    LampLastState = "1";


}

void MainWindow::on_del_selected_Pack_clicked()
{
   }

void MainWindow::on_applyPartNum_clicked()
{
    // NumPart = ui->partNum->text().toInt();
    //ui->curParty->setNum(ui->partNum->text().toInt());
}



void MainWindow::on_strt_clicked()
{
    RecieveCommand("star");
}

void MainWindow::on_ProdQuantity_valueChanged(int arg1)
{

}

void MainWindow::on_foc_clicked()
{
    QByteArray ba;
    ba[0]= 0x01;
    ba[1] = ui->spinBox->value();
    emit move(ba);
}

void MainWindow::on_foc1_clicked()
{
    QByteArray ba;
    ba[0]= 0x03;
    ba[1] = ui->spinBox->value();
    emit move(ba);
}



void MainWindow::ChangePackHeight(int h)
{
    //    if(currentPackHeight!=h)
    //    {
    //        bcp->setNeedScale(false);
    //        bcp1->setNeedScale(false);

    //        currentPackHeight = h;
    //        if(h>=200&&h<250)
    //        {
    //            stepsToFocus=230 ;
    //        }
    //        if(h>=250&&h<300)
    //        {
    //            stepsToFocus=225 ;
    //        }
    //        if(h>=300&&h<350)
    //        {
    //            stepsToFocus=220 ;
    //        }
    //        if(h>=350&&h<400)
    //        {
    //            stepsToFocus=215 ;
    //        }
    //        if(h>=400&&h<450)
    //        {
    //            stepsToFocus=210;
    //        }
    //        if(h>=450&&h<500)
    //        {
    //            stepsToFocus=203;
    //        }
    //        if(h>=500&&h<550)
    //        {
    //            bcp->setNeedScale(true);
    //            bcp1->setNeedScale(true);
    //            stepsToFocus=205;
    //        }
    //        if(h>=550&&h<600)
    //        {
    //            bcp->setNeedScale(true);
    //            bcp1->setNeedScale(true);
    //            stepsToFocus=202;
    //        }
    //        if(h>=600)
    //        {
    //            bcp->setNeedScale(true);
    //            bcp1->setNeedScale(true);

    //            stepsToFocus=200;
    //        }
    //        t1 = new QTimer;
    //        t1->singleShot(100,this,SLOT(move1obj()));


    //        t2 = new QTimer;
    //        t2->singleShot(15000,this,SLOT(move2obj()));




    //    }
}

void MainWindow::ChangeLayerN(int n)
{
    LayerN = n;

}
void MainWindow::move1obj()
{
    QByteArray ba;
    ba[0]= 0x01;
    ba[1] = stepsToFocus;
    emit move(ba);

}

void MainWindow::move2obj()
{
    QByteArray ba;
    ba[0]= 0x03;
    ba[1] = stepsToFocus;
    emit move(ba);

}

void MainWindow::on_testdb_clicked()
{
    //RecieveCommandL1("star");
}

void MainWindow::on_testCube_clicked()
{
//    v_cam->QR="https://qrc.ai/3AfrZMsdCRRo";
//    v_cam->qr_found = true;
//  RecieveCommandL1("stop");
}

void MainWindow::on_exitLach1_clicked()
{
//    RecieveCommandL1("exit");
}

void MainWindow::on_StartId_clicked()
{
//    RecieveCommand("star");
}


void MainWindow::on_stopID_clicked()
{
//    v_cam2->qr_found=true;
//    v_cam->ean13_found=true;
//    v_cam2->QR = "!!!!test.qrc.ai//TESTID";
//    v_cam2->EAN = "00001111100000";
//    RecieveCommand("stop");

}

void MainWindow::on_enb_lach1_clicked()
{


if(!enb_lach1)
{
    enb_lach1=true;
    ui->enb_lach1->setStyleSheet("QPushButton { background-color : green; }");
    ui->Lach1OFF->setStyleSheet("QPushButton { background-color : grey; }");


    QSqlQuery queryLOG(QSqlDatabase::database("MyDB"));
    queryLOG.prepare("insert into ActionLog(PCID,DateTime,Action,N_Line) values(:pcid,:datetime,:act,:Nl)");
    queryLOG.bindValue(":datetime",QDateTime::currentDateTime());
    queryLOG.bindValue(":pcid","Шкаф Привязки");
    queryLOG.bindValue(":act","Контроль Включен");
    queryLOG.bindValue(":Nl",1);
    qDebug()<<queryLOG.exec()<<endl;

}
}

void MainWindow::on_Lach1OFF_clicked()
{
    if(enb_lach1)
    {
    enb_lach1=false;
    idQueue.clear();
    idQueueOut.clear();
    idChangeQueue.clear();

    ui->enb_lach1->setStyleSheet("QPushButton { background-color : grey; }");
    ui->Lach1OFF->setStyleSheet("QPushButton { background-color : red; }");

    QSqlQuery queryLOG(QSqlDatabase::database("MyDB"));
    queryLOG.prepare("insert into ActionLog(PCID,DateTime,Action,N_Line) values(:pcid,:datetime,:act,:Nl)");
    queryLOG.bindValue(":datetime",QDateTime::currentDateTime());
    queryLOG.bindValue(":pcid","Шкаф Привязки");
    queryLOG.bindValue(":act","Контроль Выключен");
    queryLOG.bindValue(":Nl",1);
    qDebug()<<queryLOG.exec()<<endl;

    }
}


void MainWindow::on_enbLach2_clicked()
{


}


void MainWindow::on_Lach2OFF_clicked()
{


}

void MainWindow::delayEmptyMove1()
{
    ui->emptyExit1->setDisabled(false);
    ui->emptyExit1->setStyleSheet("QPushButton {}");
}

void MainWindow::delayEmptyMove2()
{

}

void MainWindow::EmptyMove1()
{
   // blockL1Start = true;
}

void MainWindow::EmptyMove2()
{
//    blockL2Start = true;
}

void MainWindow::on_emptyExit1_clicked()
{
    if(exitcam3Started)
    {
        EmptyEx1=true;
        StopCam2();

        ui->emptyExit1->setStyleSheet("QPushButton { background-color :rgb(48, 255, 55); }");
        ui->emptyExit1->setDisabled(true);
        EmptyMoveButton1_Timer->singleShot(10000,this,SLOT(delayEmptyMove1()));


        QSqlQuery queryLOG(QSqlDatabase::database("MyDB"));
        queryLOG.prepare("insert into ActionLog(PCID,DateTime,Action,N_Line) values(:pcid,:datetime,:act,:Nl)");
        queryLOG.bindValue(":datetime",QDateTime::currentDateTime());
        queryLOG.bindValue(":pcid","Шкаф Привязки");
        queryLOG.bindValue(":act","Нажата кнопка Выгнать Поддон");
        queryLOG.bindValue(":Nl",1);
        qDebug()<<queryLOG.exec()<<endl;

    }
}

void MainWindow::on_emptyExit2_clicked()
{

}



void MainWindow::on_test_cams_clicked()
{
    QVector<QString> tvec;
    tvec.push_back("11");
    idChangeQueue.enqueue(tvec);
   RecieveCommand("star");





}

void MainWindow::on_test2_clicked()
{

}
