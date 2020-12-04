#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QUdpSocket>
#include <QAudioOutput>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QLayout>

class Client : public QWidget
{
    Q_OBJECT

private:
    QUdpSocket* clientSocket;
    QAudioOutput* outputAudio;
    QIODevice* device;

    QString ip;
    int port;

    QLabel* ipLabel;
    QLabel* portLabel;

    QLineEdit* ipEdit;
    QLineEdit* portEdit;

    QAudioDeviceInfo outputDevice;
    QLabel* outputDeviceLabel;
    QComboBox* outputDeviceSelect;

    QPushButton* connectButton;

    bool isConnected;

    void connectToServer();
    void disconnectFromServer();

    void updateServerIp();
    void updateServerPort();

public:
    Client(QWidget *parent = nullptr);
    ~Client();

private slots:
    void playVoice();
    void processButton();
};
#endif // CLIENT_H
