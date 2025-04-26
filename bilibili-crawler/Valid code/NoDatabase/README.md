# 不使用数据库的爬虫版本

---

## 依赖库（需要在使用前下载的）：

- requests

- bs4

---

## 使用方式：

## 1.命令行参数

> python script.py "搜索关键词" [-s] [-p] [-o OUTPUT] [--max-pages MAX_PAGES] [--start-page START_PAGE]

### 2.首次运行

> $ python main.py "开发iOS App"
> 未检测到有效Cookie，请进行初始化设置
> 请输入B站Cookie（输入q退出）: SESSDATA=xxxxxx; bili_jct=yyyyyyy
> Cookie验证成功并已保存！
> 开始爬取...

## 3.后续运行

> $ python main.py "Swift开发" -s -p
> 检测到有效Cookie，开始爬取...

## 4.更新Cookie

> $ rm ~/.config/bilicrawler/config.ini  # Linux/macOS
> 
>  删除%APPDATA%\BiliCrawler\config.ini #windows

---

[一个结果示例](result.csv)
