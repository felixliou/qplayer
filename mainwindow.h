#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include<QMediaPlayer>
#include<QListWidgetItem>
#include<QDir>
#include<QJsonValue>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonDocument>
// 网络部分
#include<QNetworkAccessManager>
#include<QNetworkRequest>
#include<QNetworkReply>
// 使用网易云音乐的接口 Api
#define MP3ID_URL "http://s.music.163.com/api/search/get/?"// 获取歌曲信息id的 url
#define LYRIC_URL "http://music.163.com/api/song/lyric?"   // 获取歌词信息的 url

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // 播放当前清单选中的音乐条目
    void playCurrentMusicInListWidget(QListWidgetItem* item);
    // 加载歌词文件
    void loadMusicLyric(QListWidgetItem *item);


    // 打开程序时加载音乐播放器的关闭时的状态
    void loadMusicState();
    // 关闭程序前保存音乐播放器的当前状态
    void saveMusicState();
    // 处理获取歌曲 id
    void parseJsonSongId(QString);
    // 下载歌词文件
    void downLoadLyric(int);
    // 处理获取歌曲歌词并写入文件
    void parseJsonSongLyric(QString);


private slots:

    void on_pushButtonStEd_clicked();

    void on_pushButtonOpen_clicked();

    void on_pushButtonSearch_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

     void updateSlider(qint64 duration);
    //  设置Slider的进度
     void setSlider(qint64 position);
     //  设置音乐播放器进度
     void setPosition(int);
     //  播放器状态发生改变
     void setPushButton(QMediaPlayer::State t);
     // 网络歌曲id响应完成时的槽函数
     void replyMusicIDFinished(QNetworkReply *);
     // 网络歌曲lyric响应完成时的槽函数
     void replyMusicLyricFinished(QNetworkReply *);

     void on_pushButtonPlayMode_clicked();

     void on_pushButtonNextMusic_clicked();

     void on_pushButtonPreMusic_clicked();

     void on_horizontalSliderVolume_valueChanged(int value);

     void on_pushButtonDwloadLyric_clicked();

private:
    Ui::MainWindow *ui;
    // 音频播放器
    QMediaPlayer *m_player;
    // 存储搜索路径
    QDir m_dir;
    // 播放flag
    bool m_flag;

    // 播放模式
    int m_playMode;

    // 能否加载歌词文件
    bool m_openLyric;

    //  配置文件 QJsonObject 对象
    QJsonObject m_jsonObject;

    // 网络部分
    QNetworkAccessManager *network_Manager;
    QNetworkRequest *network_request;
    // 当前歌曲的 id
    int m_songId;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
