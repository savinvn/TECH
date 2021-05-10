#ifndef BARCODEPROCESSING_H
#define BARCODEPROCESSING_H
#include "BarCodeProcessing_global.h"
#include <QObject>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/objdetect.hpp>


class BARCODEPROCESSING_EXPORT BarCodeProcessing :public QObject
{
    Q_OBJECT
public:
    cv::QRCodeDetector d;

    BarCodeProcessing();
    std::vector<cv::Mat> mv;
    double alph=0;
    int addthres=0;
    int getLastGoodFrame() const;
    void setLastGoodFrame(int value);

    int getFrameToSave() const;
    void setFrameToSave(int value);

    QString getFolderName() const;
    void setFolderName(const QString &value);

    bool getQrFound() const;
    void setQrFound(bool value);
    int currentweek=0;
    int correctCode =0;
    void setNeedScale(bool value);

    bool getEanFound() const;
    void setEanFound(bool value);

private:
    bool needScale = false;
    int MainThresh;
    int lastGoodFrame=0;
      int frameToSave=15;

      cv::Mat m;
       cv::Mat mToSave;
       int nam=1;

       QString FolderName;

       bool qrFound = false;
       bool eanFound = false;
public slots:
    void ProcessFrame(cv::Mat frame);
    int getMainThresh() const;
    void setMainThresh(int value);
    void writemat();
    void UpdateDB();

signals:
     void ProcessingRequest(bool request);
     void CodeDetected(bool isFind);
     void ElapsedTime(int msec);
     void Code(QString code);
     void CodeQr(QString code);
     void SearchResult(QString res);
     void BarCodeROI(cv::Rect BC_ROI);
     void ThreshMat(cv::Mat);
};

#endif // BARCODEPROCESSING_H
