%% 读取原始数据[^1]
rawData = readtable('船舶AIS信息数据测试集.csv');
cleanedData = table();

%% 初始化统计信息[^1]
stats = struct(...
    'OriginalCount', height(rawData),...
    'GeoRemoved', 0,...
    'SpeedRemoved', 0,...
    'ShipRemoved', 0,...
    'InterpolationCount', 0);

%% Step 1: 经纬度过滤[^2]
validGeo = (rawData.lon >= -180 & rawData.lon <= 180) &...
           (rawData.lat >= -90 & rawData.lat <= 90);
stats.GeoRemoved = sum(~validGeo);
filteredData = rawData(validGeo, :);

%% Step 2: 动态特征清洗
% sog过滤标准[^2]
validSpeed = (filteredData.sog >= 0) & (filteredData.sog <= 30);
stats.SpeedRemoved = sum(~validSpeed);
filteredData = filteredData(validSpeed, :);

% 航向角度周期修正[^2]
filteredData.hdg = mod(filteredData.hdg, 360);
filteredData.cog = mod(filteredData.cog, 360);

%% Step 3: 缺失值处理
ships = unique(filteredData.ship_name);
validShips = 0;

for i = 1:length(ships)
    shipData = filteredData(filteredData.ship_name == ships(i), :);
    
    % 缺失率检查标准[^2]
    missingRate = sum(ismissing(shipData(:,{'cog','sog'}))) / height(shipData);
    if any(missingRate > 0.5)
        stats.ShipRemoved = stats.ShipRemoved + 1;
        continue;
    end
    
    % 时序插值处理[^2]
    shipData = sortrows(shipData, 'slice_time'); 
    numericCols = {'cog','sog','hdg'};
    originalMissing = sum(isnan(shipData{:,numericCols}(:)));
    
    interpolated = fillmissing(shipData{:,numericCols}, 'linear');
    newMissing = sum(isnan(interpolated(:)));
    stats.InterpolationCount = stats.InterpolationCount + (originalMissing - newMissing);
    
    shipData{:,numericCols} = interpolated;
    cleanedData = [cleanedData; shipData];
    validShips = validShips + 1;
end

%% 生成输出文件[^1]
writetable(cleanedData, '测试集_临时清洗后.csv');

%% 清洗报告
disp('====== 数据清洗报告 ======');
fprintf('原始记录数:\t\t%d\n', stats.OriginalCount);
fprintf('经纬度异常移除:\t%d\n', stats.GeoRemoved);
fprintf('速度异常移除:\t%d\n', stats.SpeedRemoved);
fprintf('高缺失移除船只:\t%d\n', stats.ShipRemoved);
fprintf('插补缺失值数量:\t%d\n', stats.InterpolationCount);
fprintf('最终有效船只:\t%d\n', validShips);
fprintf('最终有效记录:\t%d\n', height(cleanedData));
