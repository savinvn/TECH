#ifndef REPORTWRITER_H
#define REPORTWRITER_H

#include <QObject>

class ReportWriter : public QObject
{
    Q_OBJECT
public:
    explicit ReportWriter(QObject *parent = 0);

signals:

public slots:

};

#endif // REPORTWRITER_H
