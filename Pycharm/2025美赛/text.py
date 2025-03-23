import requests
import pandas as pd
from datetime import datetime, timedelta

# 设置 API 密钥
api_key = "e090972a97163be833b33a71a677b9a0"  # 替换为您的 OpenWeatherMap API 密钥
city = "Juneau,US"

# 获取今天的日期和过去一年的日期
end_date = datetime.utcnow()
start_date = end_date - timedelta(days=365)

# 将日期转换为 UNIX 时间戳
end_timestamp = int(end_date.timestamp())
start_timestamp = int(start_date.timestamp())

# API 请求 URL，获取历史天气数据
url = f"http://api.openweathermap.org/data/2.5/onecall/timemachine"

# 创建一个空列表来存储数据
weather_data = []

# 获取每月的数据，这里简单分成每 30 天获取一次
for days_ago in range(0, 365, 30):
    timestamp = int((end_date - timedelta(days=days_ago)).timestamp())

    # 发送 API 请求
    response = requests.get(url, params={
        'lat': 58.3019,  # 朱诺的纬度
        'lon': -134.4197,  # 朱诺的经度
        'dt': timestamp,  # 时间戳
        'appid': api_key,
        'units': 'metric'  # 使用摄氏度
    })

    if response.status_code == 200:
        data = response.json()

        # 提取并保存每小时的数据
        for hourly_data in data['hourly']:
            weather_data.append({
                "Date": datetime.utcfromtimestamp(hourly_data["dt"]).strftime('%Y-%m-%d %H:%M:%S'),
                "Temperature (°C)": hourly_data["temp"],
                "Humidity (%)": hourly_data["humidity"],
                "Pressure (hPa)": hourly_data["pressure"],
                "Wind Speed (m/s)": hourly_data["wind_speed"]
            })
    else:
        print(f"Failed to retrieve data for timestamp {timestamp}: {response.status_code}")

# 将数据保存为 DataFrame
df = pd.DataFrame(weather_data)

# 保存为 Excel 文件
df.to_excel("Juneau_weather_past_year.xlsx", index=False)
print("Weather data for the past year has been saved to Juneau_weather_past_year.xlsx")