#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QUrl>
#include<QFileDialog>
#include<QDebug>
#include<QTime>
#include<QMessageBox>
#include<QFile>
#include<QTextCodec>
//#include<QFloat16>
//#include<taglib/tag.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_dir = ui->lineEditSearchPath->text();
    m_flag = false;
    m_playMode = 0;
    m_player = new QMediaPlayer;

    setWindowIcon(QIcon(":myapp.ico"));
    ui->listWidget->setCurrentRow(0);
    m_player->setVolume(0);

    // 设置图标
    ui->pushButton_3->setIcon(QIcon(":/images/pause.ico"));
    ui->pushButtonNextMusic->setIcon(QIcon(":/images/nextMusic.ico"));
    ui->pushButtonPreMusic->setIcon(QIcon(":/images/preMusic.ico"));

    // 关联信号
    connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(updateSlider(qint64)));
    connect(m_player, SIGNAL(durationChanged(qint64)), this, SLOT(setSlider(qint64)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(setPosition(int)));
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this , SLOT(setPushButton(QMediaPlayer::State)));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_3_clicked()
{
    if (!m_flag){
        m_flag = true;
        m_player->play();
        setSlider(m_player->duration());
        ui->pushButton_3->setIcon(QIcon(":/images/start.ico"));
    }else{
        m_flag = false;
        m_player->pause();
        ui->pushButton_3->setIcon(QIcon(":/images/pause.ico"));

    }
}

void MainWindow::on_pushButtonOpen_clicked()
{
    QStringList strList = QFileDialog::getOpenFileNames(this, "选择", m_dir.absolutePath(), "MP3 Files (*.mp3)");
    if (strList.isEmpty()) return;
    m_player->setMedia(QUrl::fromLocalFile(strList[0]));
    for (int i = 0; i < strList.size(); i++){
        qDebug() << strList[i];
    }
    m_player->play();
    m_flag = true;
    ui->pushButton_3->setIcon(QIcon(":/images/start.ico"));
}

void MainWindow::on_pushButtonSearch_clicked()
{
    QDir tmp = QDir(ui->lineEditSearchPath->text());  // 首先检测搜索文件夹是否存在
    if (!tmp.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("指定文件夹不存在！"));
        return;
    }

    m_dir = tmp;   // 搜索文件夹存在

    ui->listWidget->clear();
    QStringList filters;
    filters << "*.mp3";
    QFileInfoList strList =m_dir.entryInfoList(filters, QDir::NoFilter);
    for (int i = 0; i < strList.size(); i++){
        QListWidgetItem *itemMusic = new QListWidgetItem(strList[i].fileName(), NULL);
        ui->listWidget->addItem(itemMusic);
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    playCurrentMusicInListWidget(item);
}

//  设置进度条等控件
 void MainWindow::setSlider(qint64 duration){
    ui->horizontalSlider->setRange(0, duration);
    ui->horizontalSlider->setEnabled(duration>0);
    ui->horizontalSlider->setPageStep(duration/10);
    ui->labelTime->setText(QTime(0, m_player->duration()/60000, (m_player->duration()%60000)/1000).toString("mm:ss"));
 }
 void MainWindow::updateSlider(qint64 position){
     qint64 mu =0, sc = 0, ms = 0;
     QString preText ="", nextText = "";

     ui->horizontalSlider->setValue(position);
     ui->labelTimeFirst->setText(QTime(0, position/60000, position%60000/1000).toString("mm:ss"));

     //  显示歌词
     // 加载歌词文件
     if (!m_openLyric)
         return;
     QListWidgetItem *item = ui->listWidget->currentItem();
     QString strTmp = item->text().split('.')[0] + tr(".lrc");
     QString filename = m_dir.absolutePath() + tr("/") + strTmp;
     QFile file(filename);
     file.open(QIODevice::ReadOnly);
     QTextStream lyric(&file);
    // QTextCodec *codec = QTextCodec::codecForName("GB18030");
     // lyric.setCodec(codec);
     while(mu*60000 + sc * 1000 + ms < position && !lyric.atEnd()){
         QString strLine = lyric.readLine();
         //qDebug() << strLine;
         QRegExp  re("\\[(\\d+):(\\d+)\\.(\\d+)\\](.*)");   //  用于提取时间和歌词信息标签
         int pos = 0;
         if  ((pos = re.indexIn(strLine, pos)) == -1)
             continue;
          //qDebug() << "++++++";
         re.indexIn(strLine);
         QStringList strList = re.capturedTexts();
         preText = nextText;
         mu = strList[1].toInt();  // 分钟
         sc = strList[2].toInt();   // 秒钟
         ms = strList[3].toInt(); // 毫秒
         nextText = strList[4];
         qDebug() << nextText;
     }
     ui->labelMusic->setAlignment(Qt::AlignHCenter);
     if (lyric.atEnd())
        ui->labelMusic->setText(nextText);
     else
         ui->labelMusic->setText(preText);
 }


 //  设置音乐播放进度
 void MainWindow::setPosition(int value){
     if (abs(value - m_player->position()) > 1000)
        m_player->setPosition(value);
 }

 //  播放器状态发生改变
 void MainWindow::setPushButton(QMediaPlayer::State t){
     if (t == QMediaPlayer::StoppedState&&m_player->position() == m_player->duration()){
         if (m_playMode == 0){// 单曲播放
             ui->pushButton_3->setIcon(QIcon(":/images/pause.ico"));
             m_flag  = false;
         }
         else if (m_playMode == 1){ // 列表播放
             qDebug() << m_playMode;
             int nCount = ui->listWidget->count();
             int nowRow = ui->listWidget->row(ui->listWidget->currentItem());
             if (nowRow != nCount-1){
                 ui->listWidget->setCurrentRow(nowRow+1);
                 QListWidgetItem  *item = ui->listWidget->currentItem();
                 playCurrentMusicInListWidget(item);
             }else{
                 ui->pushButton_3->setIcon(QIcon(":/images/pause.ico"));
                 m_flag  = false;
             }
         }
         else if (m_playMode == 2){ // 单曲循环
             m_player->play();
             m_flag = true;
         }
         else{ // 列表循环
             int nCount = ui->listWidget->count();
             int nowRow = ui->listWidget->row(ui->listWidget->currentItem());
             if (nowRow != nCount-1){
                 ui->listWidget->setCurrentRow(nowRow+1);
                 QListWidgetItem  *item = ui->listWidget->currentItem();
                 playCurrentMusicInListWidget(item);
             }else{
                 ui->listWidget->setCurrentRow(0);
                 QListWidgetItem  *item = ui->listWidget->currentItem();
                 playCurrentMusicInListWidget(item);
             }
         }
     }
 }

 // 改变播放器播放状态: 单曲播放，列表播放，单曲循环，列表循环
void MainWindow::on_pushButtonPlayMode_clicked()
{
    m_playMode++;
    if (m_playMode == 4){
        m_playMode = 0;
        ui->pushButtonPlayMode->setText(tr("单曲播放"));
    }
    else if (m_playMode == 1){
        ui->pushButtonPlayMode->setText(tr("列表播放"));
    }
    else if (m_playMode == 2){
        ui->pushButtonPlayMode->setText(tr("单曲循环"));
    }
    else{
        ui->pushButtonPlayMode->setText(tr("列表循环"));
    }
}

// 播放下一首歌曲
void MainWindow::on_pushButtonNextMusic_clicked()
{
    int nCount = ui->listWidget->count();
    if (nCount == 0)
        return;
    int nowRow = ui->listWidget->currentRow();
    if (nowRow != nCount - 1){
        ui->listWidget->setCurrentRow(nowRow+1);
        QListWidgetItem *item  = ui->listWidget->currentItem();
        playCurrentMusicInListWidget(item);
    }else{
        ui->listWidget->setCurrentRow(0);
        QListWidgetItem *item  = ui->listWidget->currentItem();
        playCurrentMusicInListWidget(item);
    }
}
// 播放上一首歌曲
void MainWindow::on_pushButtonPreMusic_clicked()
{
    int nCount = ui->listWidget->count();
    if (nCount == 0)
        return;
    int nowRow = ui->listWidget->currentRow();
    if (nowRow != 0){
        ui->listWidget->setCurrentRow(nowRow-1);
        QListWidgetItem *item  = ui->listWidget->currentItem();
        playCurrentMusicInListWidget(item);
    }else{
        ui->listWidget->setCurrentRow(0);
        QListWidgetItem *item  = ui->listWidget->currentItem();
        playCurrentMusicInListWidget(item);
    }
}

// 播放当前QListWidget选中的音乐条目
void MainWindow::playCurrentMusicInListWidget(QListWidgetItem *item){
    QString str = m_dir.absolutePath() + "/" + item->text();
    m_player->setMedia(QUrl::fromLocalFile(str));
    m_player->play();
    m_flag = true;
    ui->pushButton_3->setIcon(QIcon(":/images/start.ico"));
    // 加载歌词文件
    loadMusicLyric(item);
}
// 设置音量大小
void MainWindow::on_horizontalSliderVolume_valueChanged(int value)
{
    m_player->setVolume(value);
}

// 加载歌词文件
void MainWindow::loadMusicLyric(QListWidgetItem *item){
    QString strTmp = item->text().split('.')[0] + tr(".lrc");
    QString filename = m_dir.absolutePath() + '/' + strTmp;
    QFile file(filename);
    if ( !file.open(QIODevice::ReadOnly)){
        // 打开歌词文件失败
        m_openLyric = false;
        return;
    }else
        m_openLyric = true;
}
