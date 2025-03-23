%% 全局参数配置（基于赛题文档定义[^1]）
VALID_STATUS = ["5", "1", "0"];       % 有效状态编码
THRESHOLD_KM_PER_3H = 50;            % 航段分割阈值
INPUT_FILE = '船舶AIS信息数据测试集.csv';
OUTPUT_FILE = '轨迹预测结果.csv';

%% 数据加载修正方案
try
    % 自动检测并解决列名差异
    opts = detectImportOptions(INPUT_FILE);
    opts = setvartype(opts, {'status'}, 'string');  
    
    % 字段重命名容错处理
    if any(strcmp(opts.VariableNames, 'longitude'))
        opts = renamevars(opts, 'longitude', 'lon');
    end
    if any(strcmp(opts.VariableNames, 'latitude'))
        opts = renamevars(opts, 'latitude', 'lat'); 
    end
    
    rawData = readtable(INPUT_FILE, opts);
catch ME
    error('文件读取失败: %s\n检查字段名: %s', ME.message, strjoin(opts.VariableNames, ', '));
end

%% 时区处理增强（解析标准时区格式）
if isdatetime(rawData.slice_time)
    % 已正确转换为datetime类型
    rawData.timezone = timezone(rawData.slice_time);
else
    % 从字符串解析时区偏移（原始数据格式[^1]）
    timezoneOffset = regexp(rawData.slice_time, '(?<=\+|-)\d{2}$', 'match');
    rawData.timezone = cellfun(@(x) duration(str2double(x)/24,0,0), ...
                              vertcat(timezoneOffset{:}), 'UniformOutput', false);
    rawData.timezone = cell2mat(rawData.timezone);
    rawData.slice_time = datetime(rawData.slice_time, 'InputFormat', 'yyyy-MM-dd HH:mm:ssXXX');
end

%% 核心处理逻辑（保持不变）
results = table();
shipList = unique(rawData.ship_name);

for shipID = 1:length(shipList)
    currentShip = shipList(shipID);
    shipData = rawData(rawData.ship_name == currentShip, :);
    shipData = sortrows(shipData, 'slice_time');
    
    currentSegment = [];
    for rowIdx = 1:height(shipData)
        currentRow = shipData(rowIdx,:);
        
        if isempty(currentSegment)
            currentSegment = currentRow;
            continue;
        end
        
        % 航距计算（根据实际字段名[^1]）
        prevLat = currentSegment.lat(end);
        prevLon = currentSegment.lon(end);
        currLat = currentRow.lat;
        currLon = currentRow.lon;
        R = 6371;
        dLat = deg2rad(currLat - prevLat);
        dLon = deg2rad(currLon - prevLon);
        a = sin(dLat/2)^2 + cos(deg2rad(prevLat)) * cos(deg2rad(currLat)) * sin(dLon/2)^2;
        distance = 2 * R * atan2(sqrt(a), sqrt(1-a));
        
        % 状态转移判断（基于官方定义[^1]）
        statusChange = false;
        if strcmp(currentSegment.status(end), '5') && strcmp(currentRow.status, '1')
            statusChange = true;
        elseif strcmp(currentSegment.status(end), '1') && strcmp(currentRow.status, '0')
            statusChange = true;
        end
        
        if statusChange || (distance > THRESHOLD_KM_PER_3H)
            segmentResult = processSegment(currentSegment);
            results = [results; segmentResult];
            currentSegment = [];
        end
        
        currentSegment = [currentSegment; currentRow];
    end
    
    if ~isempty(currentSegment)
        segmentResult = processSegment(currentSegment);
        results = [results; segmentResult];
    end
end

writetable(results, OUTPUT_FILE);
disp(['生成成功: ' OUTPUT_FILE]);

%% 辅助函数修正
function output = processSegment(segment)
    % 生成三小时步长时间序列（基于测试集规则[^1]）
    startTime = dateshift(segment.slice_time(1), 'start', 'hour');
    endTime = dateshift(segment.slice_time(end), 'start', 'hour');
    timeVector = (startTime:hours(3):endTime)';
    
    % 线性插值保持数据连续性
    interpLon = interp1(datenum(segment.slice_time), segment.lon, datenum(timeVector));
    interpLat = interp1(datenum(segment.slice_time), segment.lat, datenum(timeVector));
    
    output = table();
    output.ship_name = repmat(segment.ship_name(1), length(timeVector), 1);
    output.slice_time = timeVector;
    output.lon = interpLon;
    output.lat = interpLat;
end
