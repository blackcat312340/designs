import requests  # 导入用于发送HTTP请求的requests库
from bs4 import BeautifulSoup # 导入用于解析HTML的BeautifulSoup库
import pandas as pd # 导入用于处理数据的pandas库
import json
import csv
import time as ti
import random
import re
def start_bilibili(keyword:str,flag:bool,page_label:bool):
    """keyword is what you want to search
    flag=1 means that you want to get subtitles
    flag=0 means that you do not want to get subtitles
    page_label=1 means that you want to get page_label
    page_label=0 means that you do not want to get page_label
    """
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
        ,
        'cookie': ""}
    print("start to get infor about the result of search")
    print("keyword is {}".format(keyword))
    titles = pd.DataFrame(columns=['标题'])
    times = pd.DataFrame(columns=['发布时间'])
    urls = pd.DataFrame(columns=['网址'])
    subtitles_file = pd.DataFrame(columns=['subtitle'])
    PVs = pd.DataFrame(columns=['PV'])
    followers = pd.DataFrame(columns=['follower'])
    ups = pd.DataFrame(columns=['up'])
    pages = pd.DataFrame(columns=['page'])
    page=1
    count=1
    while True:
        try:
            ur = "https://search.bilibili.com/video?vt=53655423&keyword={}app&page={}".format(keyword, page)
            response = requests.get(url=ur, headers=headers)
            if response.status_code != 200:
                print("fail to get page infor,code={}".format(response.status_code))
                return
            soup = BeautifulSoup(response.content, "html.parser")
            # 提取搜索结果
            items = soup.find_all("div", class_="bili-video-card__wrap")
            if len(items)==0:
                print("all pages have been gotten")
                break
            print("page {} have {} items".format(page, len(items)))
            for item in items:
                print("the {} video start to get infor".format(count))
                # 提取视频标题
                title = item.find('h3', class_='bili-video-card__info--tit').text
                title = pd.DataFrame([title], columns=['标题'])
                # 提取发布时间
                if item.find('span', class_='bili-video-card__info--date') == None:
                    continue
                time = item.find('span', class_='bili-video-card__info--date').get_text(strip=True)
                time = pd.DataFrame([time], columns=['发布时间'])
                # 提取视频链接
                url = item.find('a', href=True)['href']
                bvid=url[url.find('BV'):url.find('BV') + 12]
                url = pd.DataFrame([url], columns=['网址'])
                #视频字幕
                if flag==1:
                    print("the {} video start to get subtitle".format(count))
                    try:
                        ur = "https://api.bilibili.com/x/player/pagelist?bvid={}".format(bvid)
                        response = requests.get(url=ur, headers=headers)
                        response.encoding = 'utf-8'  # 修改编码格式
                        data_json = json.loads(response.text)
                        cid = data_json['data'][0]['cid']
                        ur = "https://api.bilibili.com/x/player/wbi/v2?bvid={}&cid={}".format(bvid, cid)
                        response = requests.get(url=ur, headers=headers)
                        response.encoding = 'utf-8'
                        data_json = json.loads(response.text)
                        if data_json['data']['subtitle']['subtitles'] != []:
                            subtitle_url = data_json['data']['subtitle']['subtitles'][0]['subtitle_url']
                            ur = 'https:{}'.format(subtitle_url)
                            response = requests.get(ur, headers=headers)
                            response.encoding = 'utf-8'  # 修改编码格式
                            data_json = json.loads(response.text)  # 通过 json 解析数据
                            subtitle_list = data_json['body']  # 获取 body 列表
                            subtitles = " "
                            for i in range(len(subtitle_list)):
                                subtitles += str(subtitle_list[i]['content']) + "/"
                            subtitles = pd.DataFrame([subtitles], columns=['subtitle'])
                            subtitles_file = pd.concat([subtitles_file, subtitles], ignore_index=True)
                        else:
                            subtitles = pd.DataFrame([None], columns=['subtitle'])
                            subtitles_file = pd.concat([subtitles_file, subtitles], ignore_index=True)
                    except Exception as e:
                        subtitles = pd.DataFrame([None], columns=['subtitle'])
                        subtitles_file = pd.concat([subtitles_file, subtitles], ignore_index=True)
                    ti.sleep(random.randint(10, 15))
                # 提取播放量
                pv = item.find('span', class_='', ).text
                pv = pd.DataFrame([pv], columns=['PV'])
                # 获得up主id
                pattern = r'\d+'
                up_url = item.find('a', class_='bili-video-card__info--owner', href=True)['href']
                # 获得粉丝数量
                response = requests.get("https://api.bilibili.com/x/web-interface/card?mid={}".format(re.findall(pattern, up_url)[0]),headers=headers)
                data = response.json()
                follower = pd.DataFrame([data['data']['card']['fans']], columns=['follower'])
                up = item.find('span', class_='bili-video-card__info--author').get_text(strip=True)
                up = pd.DataFrame([up], columns=['up'])
                # 将提据添加到相应的DataFrame中
                if page_label==1:
                    pa=pd.DataFrame([page], columns=['page'])
                    pages=pd.concat([pages, pa], ignore_index=True)
                titles = pd.concat([titles, title], ignore_index=True)
                times = pd.concat([times, time], ignore_index=True)
                urls = pd.concat([urls, url], ignore_index=True)
                PVs = pd.concat([PVs, pv], ignore_index=True)
                ups = pd.concat([ups, up], ignore_index=True)
                followers = pd.concat([followers, follower], ignore_index=True)
                count = count + 1
                ti.sleep(random.randint(7,10))
            print("page{} have benn gotten".format(page))
            page+=1
        except Exception as e:
            print(e)
            if flag==1:
                bilibili_data = pd.concat([titles, times, urls, PVs, ups, followers, subtitles_file], axis=1)
            else:
                bilibili_data = pd.concat([titles, times, urls, PVs, ups, followers], axis=1)
            if page_label==1:
                bilibili_data = pd.concat([bilibili_data,pages], axis=1)
            bilibili_data.to_csv('{}.csv'.format(keyword), encoding='utf-8', index=False)
            break
    if flag ==1:
        bilibili_data = pd.concat([titles, times, urls, PVs, ups, followers, subtitles_file], axis=1)
    else:
        bilibili_data = pd.concat([titles, times, urls, PVs, ups, followers], axis=1)
    if page_label==1:
        bilibili_data = pd.concat([bilibili_data,pages], axis=1)
    bilibili_data.to_csv('{}.csv'.format(keyword), encoding='utf-8', index=False)
targets=['cursor开发iosapp','xcode开发iosapp','swiftui开发iosapp','claude开发iosapp']
for target in targets:
    start_bilibili(target, 1, 1)