import os
import platform
import requests
from typing import Optional

CONFIG_DIR = {
    'Windows': os.path.join(os.environ['APPDATA'], 'BiliCrawler'),
    'Darwin': os.path.expanduser('~/Library/Application Support/BiliCrawler'),
    'Linux': os.path.expanduser('~/.config/bilicrawler')
}

CONFIG_FILE = os.path.join(CONFIG_DIR[platform.system()], 'config.ini')


def get_config_path() -> str:
    """获取跨平台配置文件路径"""
    system = platform.system()
    config_dir = CONFIG_DIR.get(system, CONFIG_DIR['Linux'])
    os.makedirs(config_dir, exist_ok=True)
    return os.path.join(config_dir, 'config.ini')


def save_cookie(cookie: str):
    """保存Cookie到配置文件"""
    config_path = get_config_path()
    with open(config_path, 'w', encoding='utf-8') as f:
        f.write(f"[Auth]\ncookie = {cookie}")


def load_cookie() -> Optional[str]:
    """从配置文件加载Cookie"""
    config_path = get_config_path()
    if not os.path.exists(config_path):
        return None

    try:
        with open(config_path, 'r', encoding='utf-8') as f:
            for line in f:
                if line.startswith('cookie'):
                    return line.split('=')[1].strip()
    except Exception as e:
        print(f"读取配置文件失败: {str(e)}")
    return None


def validate_cookie(cookie: str) -> bool:
    """验证Cookie有效性"""
    try:
        response = requests.get(
            'https://api.bilibili.com/x/web-interface/nav',
            headers={
                'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
                'Cookie': cookie
            },
            timeout=10
        )
        return response.json().get('code') == 0
    except Exception as e:
        print(f"Cookie验证失败: {str(e)}")
        return False


def setup_cookie_interactive():
    """交互式Cookie设置"""
    while True:
        cookie = input("请输入B站Cookie（输入q退出）: ").strip()
        if cookie.lower() == 'q':
            exit(0)

        if validate_cookie(cookie):
            save_cookie(cookie)
            print("Cookie验证成功并已保存！")
            return
        print("无效的Cookie，请重新输入（需包含SESSDATA和bili_jct）")
