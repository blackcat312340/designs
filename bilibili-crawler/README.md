# B站爬虫

### 前言：这个项目也是我接触爬虫技术以来第一个项目，好在github上有B站的api收集项目[bilibili-api-collect]([bilibili-API-collect/docs/video/player.md at master · SocialSisterYi/bilibili-API-collect · GitHub](https://github.com/SocialSisterYi/bilibili-API-collect/blob/master/docs/video/player.md)),让我不必花很多时间去逆向。做这个项目的本意也是有数据收集的需求，顺便也承接了下阶段网络安全的学习。

--- 

## 当前可用版本

[不使用数据库的版本](Valid-code/NoDatabase/README.md)

---

## 项目简介：

- [Dead code](Dead-code)文件夹下是已经失效的代码

- [Valid code](Valid-code)文件夹下是现在可以运行的代码

- 收集视频信息，如播放量，博主粉丝量，视频字幕，视频描述，与搜索关键词的命中项等等。

- 可以与数据库连接进行大量数据存储

- 本项目不支持多线程爬取(主要是很多接口对cookie有要求，你要是多线程爬会被封大约三四天)

- 如果需要爬取其他数据建议去上面连接里的项目看看，请求的返回类型都是json。

---

## 日志:

2025/1/22：快小年了，代码跑通了。很好，可以休息两天。

2025/4/23：当前的爬虫失效了......之前用的是解析搜索界面获取视频信息，看了眼soup发现全是乱码，还是看看那个api收集项目吧。顺便把之前只有一个函数的代码改改，多加几个功能，比如数据库连接？

2025/4/24：代码拆成了好几个函数后确实灵活得多。现在获得视频信息用的是官方api再也不用解析html了。数据库的功能明天再说吧
