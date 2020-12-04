#include "client.h"

Client::Client(QWidget *parent)
    : QWidget(parent)
    , port(9555)
    , outputDevice(QAudioDeviceInfo::defaultOutputDevice())
    , isConnected(false)
{
    ipLabel = new QLabel("Ip:", this);
    portLabel = new QLabel(":", this);

    ipEdit = new QLineEdit(this);
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegularExpression ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(ipRegex, ipEdit);
    ipEdit->setValidator(ipValidator);

    portEdit = new QLineEdit(QString::number(port), this);
    portEdit->setFixedSize(100, 25);
    portEdit->setValidator(new QIntValidator(portEdit));

    outputDeviceLabel = new QLabel("Output device:", this);
    outputDeviceSelect = new QComboBox(this);
    for (auto lDevice : QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        outputDeviceSelect->addItem(lDevice.deviceName());

    connectButton = new QPushButton("Connect", this);
    connect(connectButton, SIGNAL(clicked()), this, SLOT(processButton()));

    QHBoxLayout* addressLayout = new QHBoxLayout();
    addressLayout->addWidget(ipLabel);
    addressLayout->addWidget(ipEdit);
    addressLayout->addWidget(portLabel);
    addressLayout->addWidget(portEdit);

    QHBoxLayout* outputDeviceLayout = new QHBoxLayout();
    outputDeviceLayout->addWidget(outputDeviceLabel);
    outputDeviceLayout->addWidget(outputDeviceSelect);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(addressLayout);
    mainLayout->addLayout(outputDeviceLayout);
    mainLayout->addWidget(connectButton);
}

Client::~Client()
{
}

void Client::connectToServer()
{
    updateServerIp();
    updateServerPort();

    clientSocket = new QUdpSocket(this);
    clientSocket->bind(QHostAddress(ip), port);

    for (auto lDevice : QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        if (lDevice.deviceName() == outputDeviceSelect->currentText()) {
            outputDevice = lDevice;
            break;
        }

    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    if (!outputDevice.isFormatSupported(format))
        format = outputDevice.nearestFormat(format);

    outputAudio = new QAudioOutput(format, this);
    device = outputAudio->start();
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(playVoice()));

    connectButton->setText("Disconnect");
    isConnected = true;
}

void Client::disconnectFromServer()
{
    disconnect(clientSocket, SIGNAL(readyRead()), this, SLOT(playVoice()));

    delete outputAudio;
    delete clientSocket;

    connectButton->setText("Connect");
    isConnected = false;
}

void Client::playVoice()
{
    while (clientSocket->hasPendingDatagrams()) {
        QByteArray data;
        data.resize(clientSocket->pendingDatagramSize());
        clientSocket->readDatagram(data.data(), data.size());
        device->write(data.data(), data.size());
    }
}

void Client::updateServerIp()
{
    ip = ipEdit->text();
}

void Client::updateServerPort()
{
    port = portEdit->text().toInt();
}


void Client::processButton()
{
    if (isConnected)
        disconnectFromServer();
    else
        connectToServer();
}

