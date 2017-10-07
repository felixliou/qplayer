#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include<QMediaPlayer>
#include<QListWidgetItem>
#include<QDir>

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


private slots:

    void on_pushButton_3_clicked();

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
    // 改变播放器播放状态
     void on_pushButtonPlayMode_clicked();

     void on_pushButtonNextMusic_clicked();

     void on_pushButtonPreMusic_clicked();

     void on_horizontalSliderVolume_valueChanged(int value);

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

};

#endif // MAINWINDOW_H
