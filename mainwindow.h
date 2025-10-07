#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPortChanged(const QString &portName);
    void readData();
    void onResponseTimeout();



private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QByteArray serialBuffer;
    QTimer *responseTimer;
    QTimer *continuousTimer;

    int count=1;
    int series=0;
};
#endif // MAINWINDOW_H
