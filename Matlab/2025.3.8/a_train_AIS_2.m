%% 修正后的可并行处理方案
% 数据参数配置[^3]
missing_port_flag = "N/A";  % 标准缺失标识符
coord_decimals = 6;         % 坐标系精度要求
%% 数据加载优化
% AIS训练集稳健读取
opts = detectImportOptions('训练集_临时清洗后.csv');
opts = setvartype(opts, {'leg_end_port_code'}, 'string');
trainData = readtable('训练集_临时清洗后.csv', opts);
% 港口数据预加载
portOpts = detectImportOptions('港口静态信息数据.xlsx');
portOpts = setvartype(portOpts, {'port_code'}, 'string');
portData = readtable('港口静态信息数据.xlsx', portOpts);
%% 内存优化型港口映射构建
portMap = containers.Map('KeyType','char','ValueType','any');
for p = 1:height(portData)
    code = char(portData{p,'port_code'});  % 类型统一转换
    portMap(code) = [...
        round(portData{p,'lon'}, coord_decimals),...
        round(portData{p,'lat'}, coord_decimals)...
    ];
end
%% 并行安全型匹配逻辑
% 预处理空值
tf_missing = ismissing(trainData.leg_end_port_code) | ...
    strlength(trainData.leg_end_port_code) == 0;
trainData.leg_end_port_code(tf_missing) = missing_port_flag;
num_rows = height(trainData);
processed_coords = zeros(num_rows,2);
parfor i = 1:num_rows  % 内存安全型并行
    raw_code = char(trainData.leg_end_port_code(i));
    code = strtrim(raw_code);
    
    % 直接查询港口映射（消除共享状态）[^1]
    if ~strcmp(code, char(missing_port_flag)) && portMap.isKey(code)
        processed_coords(i,:) = portMap(code);
    end
end
%% 结果集成与验证
trainData.port_lon = processed_coords(:,1);
trainData.port_lat = processed_coords(:,2);
% 错误代码分析
invalid_mask = sum(processed_coords,2) == 0;
fprintf('未识别港口数量: %d\n', sum(invalid_mask));
writetable(trainData, '训练集_港口关联完成.csv');
