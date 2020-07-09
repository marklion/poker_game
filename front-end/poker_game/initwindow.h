#ifndef INITWINDOW_H
#define INITWINDOW_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressBar>

#define INIT_FONT_FILE_URL "/resource/msyhl.ttc"
#define RESOURCE_HOST_NAME "59.110.64.232"


class InitWindow : public QWidget
{
    Q_OBJECT

public:
    InitWindow(QWidget *parent = nullptr);
    ~InitWindow();
    void win_resouce_request();
private:

    QNetworkReply *m_poReply = NULL;
    QProgressBar *m_pgbar = NULL;
    QNetworkAccessManager nam;
private slots:
    void stop_init();
    void win_resouce_recv();
    void updateDataReadProgress(qint64,qint64);


};
#endif // INITWINDOW_H
