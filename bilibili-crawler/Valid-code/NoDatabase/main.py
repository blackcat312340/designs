import argparse
import requests
from bs4 import BeautifulSoup
import pandas as pd
import json
import time
import random
import re
from typing import List, Dict, Optional
from config_manager import load_cookie, setup_cookie_interactive
#数据存储结构
datas = []
# 常量定义
BASE_SEARCH_URL = "https://api.bilibili.com/x/web-interface/search/type"
API_PAGELIST_URL = "https://api.bilibili.com/x/player/pagelist"
API_SUBTITLE_URL = "https://api.bilibili.com/x/player/v2"
API_USER_INFO_URL = "https://api.bilibili.com/x/web-interface/card"

DEFAULT_HEADERS = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36 Edg/135.0.0.0',
    'Referer': 'https://www.bilibili.com/video'
}

def get_subtitle(bvid: str, headers: Dict) -> Optional[str]:
    """获取视频字幕"""
    try:
        # 获取视频分P信息
        pagelist_response = requests.get(
            API_PAGELIST_URL,
            params={'bvid': bvid},
            headers=headers
        )
        pagelist_response.raise_for_status()
        cid = pagelist_response.json()['data'][0]['cid']

        # 获取字幕信息
        subtitle_response = requests.get(
            API_SUBTITLE_URL,
            params={'bvid': bvid, 'cid': cid},
            headers=headers
        )
        subtitle_response.raise_for_status()

        subtitles = subtitle_response.json()['data']['subtitle']['subtitles']
        if not subtitles:
            return None

        subtitle_url = f"https:{subtitles[0]['subtitle_url']}"
        subtitle_content = requests.get(subtitle_url, headers=headers).json()
        return ' '.join([item['content'] for item in subtitle_content['body']])

    except Exception as e:
        print(f"获取字幕失败: {str(e)}")
        return None


def get_user_info(mid: str, headers: Dict) -> Dict:
    """获取用户信息"""
    try:
        response = requests.get(
            API_USER_INFO_URL,
            params={'mid': mid},
            headers=headers
        )
        response.raise_for_status()
        return response.json()['data']['card']
    except Exception as e:
        print(f"获取用户信息失败: {str(e)}")
        return {'fans': 0}


def process_video_item(item, headers: Dict, args) -> Optional[Dict]:
    """处理单个视频项"""
    try:
        # 提取UP主信息
        mid = item['mid']
        user_info = get_user_info(mid, headers)

        # 构造结果字典
        result = {
            'title': item['标题'],
            'pub_date': item['发布时间'],
            'url': item['视频链接'],
            'plays': item['播放量'],
            'up_name': item['UP主'],
            'followers': user_info['fans'],
            'tag':item['标签'],
            'hit': item['命中项'],
            'des':item['描述'],
            'page': args.start_page + args.current_page - 1
        }

        # 获取字幕
        if args.subtitles:
            result['subtitle'] = get_subtitle(item['bvid'], headers)
            time.sleep(random.uniform(5, 7))

        return result

    except Exception as e:
        print(f"处理视频项时发生错误: {str(e)}")
        return None


def crawl_bilibili(args,headers):
    """主爬虫函数"""
    # 初始化数据结构
    items = []
    # 爬取多个页面
    for page_num in range(args.start_page, args.start_page + args.max_pages):
        print(f"正在爬取第 {page_num} 页...")
        time.sleep(1)
        # 构造搜索请求
        try:
            search_params = {
                'keyword': args.keyword,
                'page': page_num,
                'search_type': 'video'
            }
            response = requests.get(
                BASE_SEARCH_URL,
                params=search_params,
                headers=headers
            )
            response.raise_for_status()
            data = response.json()
            if data["code"] == 0:
                for item in data["data"]["result"]:
                    result = {
                        "标题": item.get("title", ""),
                        "UP主": item.get("author", ""),
                        "播放量": item.get("play", 0),
                        "时长": item.get("duration", ""),
                        "发布时间": time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(item.get("pubdate", 0))),
                        "视频链接": f"https://www.bilibili.com/video/{item.get('bvid', '')}",
                        "描述":item.get("description", ""),
                        "标签": item.get("tag", ""),
                        "命中项" :item.get("hit_columns", ""),
                        "mid":item.get("mid", ""),
                        "bvid":item.get("bvid", "")
                    }
                    items.append(result)
            else:
                print(f"API返回错误：{data['message']}")
                break
        except requests.RequestException as e:
            print(f"搜索请求失败: {str(e)}")
            break

        if not items:
            print("没有更多结果")
            break
        # 处理每个视频项
        for item in items:
            video_data = process_video_item(item, headers, args)
            if video_data:
                datas.append(video_data)
                time.sleep(random.uniform(0.5, 1.5))

        # 随机延迟防止被封
        time.sleep(random.uniform(2, 5))

    # 保存结果到CSV
    if datas:
        df = pd.DataFrame(datas)
        keep_columns = ['title','des','tag','hit','pub_date','url','plays','up_name','followers']
        if args.subtitles:
            keep_columns.append('subtitle')
        if args.page_label:
            keep_columns.append('page')

        df[keep_columns].to_csv(args.output, index=False)
        print(f"数据已保存到 {args.output}")


def main():
    # 初始化Cookie检查
    cookie = load_cookie()
    if not cookie:
        print("未检测到有效Cookie，请进行初始化设置")
        setup_cookie_interactive()
        cookie = load_cookie()  # 重新加载保存的Cookie

    # 设置请求头
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
        'Cookie': cookie,
        'Referer': 'https://www.bilibili.com/'
    }
    """命令行入口"""
    parser = argparse.ArgumentParser(description="B站视频信息爬虫")
    parser.add_argument('keyword', type=str, help='搜索关键词')
    parser.add_argument('-s', '--subtitles', action='store_true', help='是否获取字幕')
    parser.add_argument('-p', '--page-label', action='store_true', help='是否包含页码信息')
    parser.add_argument('-o', '--output', type=str, default='result.csv', help='输出文件名')
    parser.add_argument('--max-pages', type=int, default=5, help='最大爬取页数')
    parser.add_argument('--start-page', type=int, default=1, help='起始页码')

    args = parser.parse_args()
    args.current_page = 1  # 用于跟踪当前处理页

    try:
        crawl_bilibili(args,headers)
    except KeyboardInterrupt:
        print("\n用户中断操作，正在保存已获取数据...")
        if datas:
            df = pd.DataFrame(data)
            df.to_csv(args.output, index=False)


if __name__ == "__main__":
    main()