#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QUrl>
#include<QFileDialog>
#include<QDebug>
#include<QTime>
#include<QMessageBox>
#include<QFile>
#include<QTextCodec>
#include<QCloseEvent>
#include<QRegExp>

//#include<QFloat16>
//#include<taglib/tag.h>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_flag = false;
    m_player = new QMediaPlayer;

    // 网络部分
    network_Manager = new QNetworkAccessManager();
    network_request = new QNetworkRequest();

    setWindowIcon(QIcon(":myapp.ico"));
    // 设置图标
    ui->pushButtonStEd->setIcon(QIcon(":/images/pause.ico"));
    ui->pushButtonNextMusic->setIcon(QIcon(":/images/nextMusic.ico"));
    ui->pushButtonPreMusic->setIcon(QIcon(":/images/preMusic.ico"));

    // 关联信号
    connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(updateSlider(qint64)));
    connect(m_player, SIGNAL(durationChanged(qint64)), this, SLOT(setSlider(qint64)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(setPosition(int)));
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this , SLOT(setPushButton(QMediaPlayer::State)));


    if (!QDir(".").exists("config.json")){
        m_dir = ui->lineEditSearchPath->text();
        m_playMode = 0;
        m_player->setVolume(0);
    }else{
        loadMusicState();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonStEd_clicked()
{
    if (!m_flag){
        m_flag = true;
        m_player->play();
        setSlider(m_player->duration());
        ui->pushButtonStEd->setIcon(QIcon(":/images/start.ico"));
    }else{
        m_flag = false;
        m_player->pause();
        ui->pushButtonStEd->setIcon(QIcon(":/images/pause.ico"));

    }
}

void MainWindow::on_pushButtonOpen_clicked()
{
    /*
    QString strTemp = QFileDialog::getOpenFileName(this, "选择", m_dir.absolutePath(), "Music Files (*.mp3 *.wav *.wma *.ape *.acc *.ogg)");
    if (strTemp.isEmpty()) return;
    m_player->setMedia(QUrl::fromLocalFile(strTemp));
    m_player->play();
    m_flag = true;
    ui->pushButton_3->setIcon(QIcon(":/images/start.ico"));
    */
    QString strMusicFile = QFileDialog::getOpenFileName(this, "选择歌词文件", m_dir.absolutePath(), "lyric Files(*.lrc)");
    QString currentMusic = m_dir.absolutePath() + "/" + ui->listWidget->currentItem()->text();
    if (strMusicFile.isEmpty())   // 如果没有选中文件就立刻返回
        return;
    if (m_jsonObject.contains(currentMusic)){
        m_jsonObject[currentMusic] = strMusicFile;
    }else{
        m_jsonObject.insert(currentMusic, strMusicFile );
    }
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
    filters << "*.mp3" << "*.wav" << "*.wma" << "*.ape" << "*.acc" << "*.ogg";
    QFileInfoList strList =m_dir.entryInfoList(filters, QDir::NoFilter);
    for (int i = 0; i < strList.size(); i++){
        QListWidgetItem *itemMusic = new QListWidgetItem(strList[i].fileName(), NULL);
        ui->listWidget->addItem(itemMusic);
    }
    if (ui->listWidget->count() > 0)
         ui->listWidget->setCurrentRow(0);  // 设置默认选中条目
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

     // 加载歌词文件
    QListWidgetItem *item = ui->listWidget->currentItem();
     // 首先判断配置类型中是否存在已经设置好的歌词文件
     QString currentMusicLyric;
     QString currentMusic = m_dir.absolutePath() + "/" +item->text();
     if (m_jsonObject.contains(currentMusic)){
         currentMusicLyric = m_jsonObject[currentMusic].toString();
     }else{ // 在当前目录下搜索
         QString strTmp = item->text().split('.')[0] + tr(".lrc");
         currentMusicLyric = m_dir.absolutePath() + "/" + strTmp;
     }
     QFile file(currentMusicLyric);
    if ( !file.open(QIODevice::ReadOnly))
        return;
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
     file.close();
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
             ui->pushButtonStEd->setIcon(QIcon(":/images/pause.ico"));
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
                 ui->pushButtonStEd->setIcon(QIcon(":/images/pause.ico"));
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
    ui->pushButtonStEd->setIcon(QIcon(":/images/start.ico"));
    // 加载歌词文件
    ui->labelMusic->setText("");
    loadMusicLyric(item);
}
// 设置音量大小
void MainWindow::on_horizontalSliderVolume_valueChanged(int value)
{
    m_player->setVolume(value);
}

// 加载歌词文件
void MainWindow::loadMusicLyric(QListWidgetItem *item){
    // 首先判断配置类型中是否存在已经设置好的歌词文件
    QString currentMusicLyric;
    QString currentMusic = m_dir.absolutePath() + "/" +item->text();
    if (m_jsonObject.contains(currentMusic)){
        currentMusicLyric = m_jsonObject[currentMusic].toString();
    }else{ // 在当前目录下搜索
        QString strTmp = item->text().split('.')[0] + tr(".lrc");
        currentMusicLyric = m_dir.absolutePath() + "/" + strTmp;
    }
    QFile file(currentMusicLyric);
    if ( !file.open(QIODevice::ReadOnly)){
        // 打开歌词文件失败
        m_openLyric = false;
        return;
    }else
        m_openLyric = true;
}

//  关闭窗口之前保存当前播放器的状态
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton rt = QMessageBox::information(this, tr("关闭程序"), tr("确定关闭程序"),QMessageBox::Yes|QMessageBox::No);
    if (rt == QMessageBox::Yes){
        // 关闭前保存音乐播放器的当前状态
        saveMusicState();
        event->accept();
    }else{
        event->ignore();
    }

}

//  保存关闭前音乐播放器的状态
void MainWindow::saveMusicState(){
    m_jsonObject.insert("music_dir", m_dir.absolutePath());
    m_jsonObject.insert("music_volume", m_player->volume());
    m_jsonObject.insert("music_currentMusic", ui->listWidget->currentItem()->text());
    m_jsonObject.insert("music_progress", ui->horizontalSlider->value());
    m_jsonObject.insert("music_playMode", m_playMode);
    // 写入配置文件
    QJsonDocument jsonDoc(m_jsonObject);
    QByteArray ba = jsonDoc.toJson();
    QFile file(QDir(".").absolutePath() + "/" + "config.json");
    file.open(QIODevice::WriteOnly);
    file.write(ba);
    file.close();
}

// 加载配置文件
void MainWindow::loadMusicState(){
    QFile file(QDir(".").absolutePath()+"/"+"config.json");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(ba);
    m_jsonObject = jsonDoc.object();
    m_dir = QDir(m_jsonObject["music_dir"].toString());
    ui->lineEditSearchPath->setText(m_jsonObject["music_dir"].toString());
    on_pushButtonSearch_clicked();  // 手动执行搜索文件夹

    QFileInfo currentMusicFile(m_jsonObject["music_currentMusic"].toString());
    QString musicName = currentMusicFile.fileName();
    QListWidgetItem* item = ui->listWidget->findItems(musicName, Qt::MatchExactly)[0];
    ui->listWidget->setCurrentItem(item);  // 设置选中条目

    m_player->setVolume(m_jsonObject["music_volume"].toInt());
    ui->horizontalSliderVolume->setValue(m_jsonObject["music_volume"].toInt());
    m_player->setMedia(QUrl::fromLocalFile(m_dir.absolutePath() + "/" +m_jsonObject["music_currentMusic"].toString()));
    emit m_player->durationChanged(m_player->duration());
    m_player->setPosition(m_jsonObject["music_progress"].toInt());
    ui->horizontalSlider->setValue(m_jsonObject["music_progress"].toInt());  // 不能显示当前进度是什么原因呢？


    m_playMode = m_jsonObject["music_playMode"].toInt();
    if (m_playMode == 0){
        m_playMode = 3;
    }else{
        m_playMode--;
    }
    on_pushButtonPlayMode_clicked(); // 手动执行播放模式改变
}

// 下载歌词文件
void MainWindow::on_pushButtonDwloadLyric_clicked()
{
    if (ui->listWidget->count() < 1) return;
    QListWidgetItem *item = ui->listWidget->currentItem();  // 当前选中条目
    QRegExp re("\\-|\\.");
    QString musicName = item->text().split(re)[1].trimmed();  // 当前选中歌曲的名称

    // 设置发送数据
    QByteArray bsend;
    bsend.append("s="+musicName+"&");
    bsend.append("limit=100&");
    bsend.append("type=1");

    QString url = MP3ID_URL + QString(bsend);


    connect(network_Manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyMusicIDFinished(QNetworkReply*)));
    network_request->setUrl(url);
    network_Manager->get(*network_request);
}

void MainWindow::replyMusicIDFinished(QNetworkReply *reply){
    // 解除关联
     disconnect(network_Manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyMusicIDFinished(QNetworkReply*)));
    //获取响应的信息，状态码为200表示正常

    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串
        parseJsonSongId(result);  //自定义方法，解析歌曲数据
        downLoadLyric(m_songId);
    }
    else
    {
        QMessageBox::information(this, tr("失败"), tr("下载歌词失败"));
    }
}

void MainWindow::parseJsonSongId(QString result){
    QByteArray ba;
    QJsonParseError jsonError;
    ba.append(result);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(ba, &jsonError);
    if (jsonError.error == QJsonParseError::NoError){
        if (jsonDoc.isObject()){
            QJsonObject jsonObj = jsonDoc.object();
            QJsonValue result = jsonObj["result"];
            if  (result.isObject()){
                QJsonObject jsonObjResult = result.toObject();
                if (jsonObjResult.contains("songs")){
                    QJsonValue songs = jsonObjResult["songs"];
                    if (songs.isArray()){
                        QJsonArray songsArray =  songs.toArray();

                        int size = songsArray.size();  // 统计数组大小
                        QListWidgetItem *item = ui->listWidget->currentItem();  // 当前选中条目
                        QRegExp re("\\-|\\.");
                        QString singerName = item->text().split(re)[0].trimmed();  // 当前选中歌手的名称
                        QStringList singerNameList = singerName.split(',');               // 歌手名称列表

                        for (int i = 0; i< size; i++){
                            QJsonValue songsValue = songsArray[i];
                            if (songsValue.isObject()){
                                QJsonObject songObj = songsValue.toObject();
                                if (songObj.contains("artists")){
                                    QJsonValue artistsValue = songObj["artists"];
                                    if  (artistsValue.isArray()){
                                        QJsonArray artistsArray = artistsValue.toArray();
                                        if (singerNameList.size() != artistsArray.size())
                                            continue;
                                        for (int j  = 0; j < singerNameList.size(); j++){
                                            if (singerNameList[j] != artistsArray[j].toObject()["name"].toString())
                                                break;
                                            m_songId = songObj["id"].toInt();
                                            qDebug() << m_songId;
                                        }
                                    }

                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// 下载歌词文件
void MainWindow::downLoadLyric(int songId){
    // 设置发送数据
    qDebug() << songId << " from 下载歌词文件";
    QByteArray bsend;
    //bsend.append("os=pc&");
    bsend.append("id="+QString::number(songId, 10)+"&");
    bsend.append("lv=-1&");
    bsend.append("kv=-1&");
    bsend.append("tv=-1");

    QString url = LYRIC_URL + QString(bsend);
    qDebug() << url;

    connect(network_Manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyMusicLyricFinished(QNetworkReply*)));
    network_request->setUrl(url);
    network_Manager->get(*network_request);
}

// 歌词信息请求完成
void MainWindow::replyMusicLyricFinished(QNetworkReply *reply){
    // 解除关联
     disconnect(network_Manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyMusicLyricFinished(QNetworkReply*)));
    //获取响应的信息，状态码为200表示正常

    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串
        parseJsonSongLyric(result);  //自定义方法，解析歌曲数据
    }
    else
    {
        QMessageBox::information(this, tr("失败"), tr("获取歌词文件失败！"));
    }
}

// 解析下载歌词文件
void MainWindow::parseJsonSongLyric(QString result){
    QByteArray ba;
    QJsonParseError jsonError;
    ba.append(result);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(ba, &jsonError);
    if (jsonError.error == QJsonParseError::NoError){
        if (jsonDoc.isObject()){
            QJsonObject jsonObj = jsonDoc.object();
            if (jsonObj.contains("lrc")){
                QJsonValue lrc = jsonObj["lrc"];
                if (lrc.isObject()){
                    QJsonObject lrcObj = lrc.toObject();
                    if (lrcObj.contains("lyric")){
                        QJsonValue lyric = lrcObj["lyric"];

                        QString musicLyric = lyric.toString();  //  歌词信息
                        // 下载歌词文件
                        QString musicName = ui->listWidget->currentItem()->text();
                        musicName.truncate(musicName.lastIndexOf('.'));
                        QString filePath = m_dir.absolutePath() + "/" + musicName + ".lrc";
                        QFile file(filePath);
                        file.open(QIODevice::WriteOnly);
                        QByteArray ba;
                        ba.append(musicLyric);
                        file.write(ba);
                        file.close();
                    }
                }
            }
        }
    }
}
