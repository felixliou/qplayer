# qplayer
---
## 介绍
这是一款简单的音乐播放器，基于 Qt C++ 开发，开发环境为 QtCreator, 音乐播放器界面 UI 设计较为简陋。

## 功能
* 目前支持常见格式的音频文件的播放(\*.mp3, \*.wav, \*.wma等)
* 加载本地音乐文件
* 播放/暂停
* 调节音乐播放音量
* 调节音乐播放进度
* 切换曲目上一首和下一首
* 四种播放模式: 单曲播放，列表播放，单曲循环，列表循环
* 显示歌词(目前只支持 .lrc 格式的歌词文件)

## 新增功能
* 支持音乐播放器的记忆播放功能，关闭程序后程序将记录此次播放器关闭之前的状态
* 支持本地打开选择歌词文件，下次音乐播放器将调用本次选择的歌词文件
* 联网状态下，能够从网络上下载对应歌曲的歌词(使用的是网易云音乐的 API)

## 演示截图
### Linux (Ubuntu16.04) 下的演示截图
1. ![](https://raw.githubusercontent.com/felixliou/qplayer/master/ScreenShots/1.png)
2. ![](https://raw.githubusercontent.com/felixliou/qplayer/master/ScreenShots/2.png)
### Windows7 下的演示截图
3. ![](https://raw.githubusercontent.com/felixliou/qplayer/master/ScreenShots/3.png)
4. ![](https://raw.githubusercontent.com/felixliou/qplayer/master/ScreenShots/4.png)

## 注意
* 音乐播放器目前需要手动输入搜索音乐文件夹的绝对路径
* 播放器自动调用同文件夹下的歌词文件时，歌词文件名需要与歌曲文件名一致，才可加载歌词文件
* 由于搜索歌词使用的方法欠妥或者网易云音乐的公共 API 返回的 Json 数据的问题，可能导致有些歌词无法正常加载

## 致谢
* 在音乐播放器的编写过程中，[Christopher L](https://github.com/0xE8551CCB) 提出了很多好的修改建议，非常感谢。

## 许可
* Licensed under the [MIT license](./LICENSE.md).
