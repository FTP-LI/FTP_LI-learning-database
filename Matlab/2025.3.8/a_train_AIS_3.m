%% ===== 船舶特征增强系统 =====
clc; clear; close all;

%% === 数据加载 ===
% 加载港口数据（根据参考资料4-6中的港口位置数据）
portData = readtable('港口静态信息数据.xlsx');
portData = portData(:, {'port_code', 'lon', 'lat'});

% 加载清洗后的训练数据（根据参考资料1的结构）
trainData = readtable('训练集_港口关联完成.csv');
trainData.slice_time = datetime(trainData.slice_time,...
    'InputFormat','yyyy-MM-dd HH:mm:ss');

%% === 特征工程 ===
% 按船舶分组处理（根据参考资料1的ship_name字段）
groups = findgroups(trainData.ship_name);

% -- 基础衍生特征 --
% 航向偏差计算[^1]
trainData.hdg_cog_diff = trainData.cog - trainData.hdg;

% -- 时间序列特征 --
% 速度变化率计算[^1]
trainData = grpstats(trainData, 'ship_name',...
    {@(x) [NaN; diff(x)./hours(diff(trainData.slice_time(groups==x)))],...
    'sog'}, 'DataVars', {'sog','slice_time'});

% 滑动窗口均值（窗口大小3）[^2]
trainData.sog_rolling_mean = splitapply(@(x) movmean(x,[2 0]),...
    trainData.sog, groups);

% 航行状态标记[^1]
trainData.is_sailing = double(trainData.status == 0);

%% === 地理特征计算 ===
% 合并目的港坐标（根据参考资料1的leg_end_port_code字段）
mergedData = innerjoin(trainData, portData,...
    'LeftKeys','leg_end_port_code','RightKeys','port_code');

% 实现Haversine公式计算距离（根据参考资料3的经度纬度数据）
R = 6371; % 地球半径(km)
dlon = deg2rad(mergedData.longitude - mergedData.lon);
dlat = deg2rad(mergedData.latitude - mergedData.lat);
a = sin(dlat/2).^2 + cos(deg2rad(mergedData.latitude)) .* ...
    cos(deg2rad(mergedData.lat)) .* sin(dlon/2).^2;
c = 2 * atan2(sqrt(a), sqrt(1-a));
mergedData.distance_to_port = R * c;

%% === 结果保存 ===
writetable(mergedData, '训练集_特征增强.csv');
disp('特征增强完成，文件已保存为：训练集_特征增强.csv');
