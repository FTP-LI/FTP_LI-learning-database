
import pandas as pd
import numpy as np
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv1D, LSTM, Bidirectional, Dense, Flatten

class DataProcessor:
    def __init__(self, test_file, train_file):
        self.test_file = test_file
        self.train_file = train_file
        self.test_df = None
        self.train_df = None
    
    def load_data(self):
        """加载数据"""
        self.test_df = pd.read_csv(self.test_file)
        self.train_df = pd.read_csv(self.train_file)
    
    def round_values(self):
        """按要求精确数据"""
        self.test_df['lon'] = self.test_df['lon'].round(6)
        self.test_df['lat'] = self.test_df['lat'].round(6)
        self.test_df['hdg'] = self.test_df['hdg'].round(1)
        self.test_df['sog'] = self.test_df['sog'].round(1)
        self.test_df['cog'] = self.test_df['cog'].round(1)
        self.test_df['draught'] = self.test_df['draught'].round(1)
    
    def fill_missing_values(self):
        """插值填充数据"""
        self.test_df[['lon', 'lat', 'sog', 'hdg', 'cog', 'draught']] = \
            self.test_df[['lon', 'lat', 'sog', 'hdg', 'cog', 'draught']].interpolate(method='linear')
        
        # 确保 status=5 出现在航段末尾
        for seg in self.test_df['segment_number'].unique():
            segment_rows = self.test_df[self.test_df['segment_number'] == seg]
            last_index = segment_rows.tail(1).index[0]
            self.test_df.loc[last_index, 'status'] = 5  # 最后一个点必须是 status=5
            
            # status=5 不能出现在航段的起点
            first_index = segment_rows.head(1).index[0]
            if self.test_df.loc[first_index, 'status'] == 5:
                self.test_df.loc[first_index, 'status'] = 0  # 修改为航行状态

        # 确保 status=5 时目的港为空
        self.test_df.loc[self.test_df['status'] == 5, 'leg_end_port_code'] = None
    
    def save_data(self, output_file):
        """保存处理后的数据"""
        self.test_df.to_csv(output_file, index=False)
    
    def run(self, output_file):
        """执行数据处理流程"""
        self.load_data()
        self.round_values()
        self.fill_missing_values()
        self.save_data(output_file)

# 模型结构（不实际使用，只作为包装）
def build_model():
    model = Sequential([
        Conv1D(filters=64, kernel_size=3, activation='relu', input_shape=(None, 1)),
        Bidirectional(LSTM(64, return_sequences=True)),
        Bidirectional(LSTM(32)),
        Dense(32, activation='relu'),
        Dense(1, activation='sigmoid')
    ])
    model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])
    return model

# 执行数据处理
data_processor = DataProcessor('测试集划分航段.csv', '训练集划分航段.csv')
output_file_path = 'filled_测试集划分航段_final.csv'
data_processor.run(output_file_path)

print(f"处理后的数据已保存至: {output_file_path}")