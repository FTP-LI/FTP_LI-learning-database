import pandas as pd
import numpy as np
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, Dataset
from tqdm import tqdm
from sklearn.preprocessing import MinMaxScaler

# ========== 读取数据 ==========
train_data = pd.read_csv('训练集划分航段.csv')
test_data = pd.read_csv('测试集划分航段.csv') 
port_data = pd.read_csv('港口静态信息数据.csv')

# ========== 数据预处理 ==========
# 填充测试集中缺失的经纬度数据
port_map = dict(zip(port_data['port_code'], zip(port_data['lon'], port_data['lat'])))
test_data['lon'] = test_data['leg_end_port_code'].map(lambda x: port_map.get(x, (np.nan, np.nan))[0])
test_data['lat'] = test_data['leg_end_port_code'].map(lambda x: port_map.get(x, (np.nan, np.nan))[1])

# 规范化数据
scaler = MinMaxScaler()
train_features = scaler.fit_transform(train_data[['lon', 'lat', 'hdg', 'sog', 'cog', 'draught']].values)
test_features = scaler.transform(test_data[['lon', 'lat', 'hdg', 'sog', 'cog', 'draught']].values)

# 增加序列维度，使其符合CNN-BiLSTM输入要求
train_features = np.expand_dims(train_features, axis=1)  # shape: (batch_size, 1, input_size)
test_features = np.expand_dims(test_features, axis=1)  # shape: (batch_size, 1, input_size)

# ========== 自定义数据集 ==========
class ShipDataset(Dataset):
    def __init__(self, features, labels=None):
        self.features = torch.tensor(features, dtype=torch.float32)
        self.labels = torch.tensor(labels, dtype=torch.float32) if labels is not None else None

    def __len__(self):
        return len(self.features)

    def __getitem__(self, idx):
        feature = self.features[idx].clone().detach()
        if self.labels is not None:
            label = self.labels[idx].clone().detach()
            return feature, label
        return (feature,)  # 测试数据集没有标签时，返回单元素元组

# ========== 定义CNN-BiLSTM模型 ==========
class CNNBiLSTM(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(CNNBiLSTM, self).__init__()
        self.conv1 = nn.Conv1d(input_size, 64, kernel_size=3, padding=1)  # input_size 作为通道数
        self.conv2 = nn.Conv1d(64, 128, kernel_size=3, padding=1)
        self.lstm = nn.LSTM(128, hidden_size, bidirectional=True, batch_first=True)
        self.fc = nn.Linear(hidden_size * 2, output_size)

    def forward(self, x):
        x = x.transpose(1, 2)  # 转换为 (batch_size, input_size, seq_len)
        x = torch.relu(self.conv1(x))  # 这里的 conv1 需要输入通道数为 6
        x = torch.relu(self.conv2(x))
        x, _ = self.lstm(x.transpose(1, 2))  # LSTM expects (batch_size, seq_len, input_size)
        x = self.fc(x[:, -1, :])  # 取最后一个时间步的输出
        return x

# ========== 训练参数 ==========
EPOCHS = 1 # 设置训练的迭代次数，可修改
BATCH_SIZE = 64
LEARNING_RATE = 0.001
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# ========== 加载数据 ==========
train_dataset = ShipDataset(train_features, train_data[['lon', 'lat', 'hdg', 'sog', 'cog', 'draught']].values)
train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True)

test_dataset = ShipDataset(test_features)  # 没有标签
test_loader = DataLoader(test_dataset, batch_size=BATCH_SIZE, shuffle=False)

# ========== 初始化模型 ==========
model = CNNBiLSTM(input_size=6, hidden_size=64, output_size=6).to(device)  # 输出6个特征
criterion = nn.MSELoss()
optimizer = torch.optim.Adam(model.parameters(), lr=LEARNING_RATE)

# ========== 训练模型 ==========
model.train()
for epoch in range(EPOCHS):
    total_loss = 0
    with tqdm(total=len(train_loader), desc=f"Epoch {epoch+1}/{EPOCHS} Training Progress") as pbar:
        for inputs, labels in train_loader:
            inputs, labels = inputs.to(device), labels.to(device)

            # 前向传播
            optimizer.zero_grad()
            outputs = model(inputs)

            # 计算损失并反向传播
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()

            total_loss += loss.item()
            pbar.set_postfix(loss=loss.item())
            pbar.update(1)
    
    print(f"Epoch {epoch+1}/{EPOCHS} - Loss: {total_loss / len(train_loader):.4f}")

# ========== 预测 ==========
model.eval()
predictions = []
with torch.no_grad():
    with tqdm(total=len(test_loader), desc="Prediction Progress") as pbar:
        for batch in test_loader:
            inputs = batch[0].to(device)  # 由于 test_loader 只返回 inputs，因此 batch 是一个元组
            outputs = model(inputs)
            predictions.append(outputs.cpu().numpy())
            pbar.update(1)

# ========== 结果填充 ==========
predictions = np.concatenate(predictions, axis=0)

# 获取源数据的小数位数
def get_decimal_places(column):
    return column.apply(lambda x: len(str(x).split('.')[-1]) if pd.notna(x) and '.' in str(x) else 0).max()

# 填充每列时匹配小数位数
decimal_places = {
    'lon': get_decimal_places(test_data['lon']),
    'lat': get_decimal_places(test_data['lat']),
    'hdg': get_decimal_places(test_data['hdg']),
    'sog': get_decimal_places(test_data['sog']),
    'cog': get_decimal_places(test_data['cog']),
    'draught': get_decimal_places(test_data['draught']),
}

# 填充空白数据时，确保匹配预测数据的长度与目标列空白数据的数量一致
for column, places in decimal_places.items():
    missing_data_count = test_data[column].isna().sum()
    
    if missing_data_count > 0:  # 确保有空白数据需要填充
        column_idx = list(test_data.columns).index(column)
        
        # 确保列索引不会超出 predictions 的范围
        if column_idx < predictions.shape[1]:
            prediction_values = np.round(predictions[:missing_data_count, column_idx], places)
            test_data.loc[test_data[column].isna(), column] = prediction_values
        else:
            print(f"Warning: Column index {column_idx} exceeds the number of prediction features (6). Skipping column {column}.")

# ========== 结果保存 ==========
test_data.to_csv('填充后的最终测试集.csv', index=False)
print("预测结果已保存到 '填充后的最终测试集.csv'")

