% 建模高铁站
stationHeight = 10; % 高铁站的高度
stationWidth = 100; % 高铁站的宽度
stationDepth = 50; % 高铁站的深度
stationColor = 'red'; % 高铁站的颜色

% 创建高铁站的长方体
stationVertices = [0 0 0; stationWidth 0 0; stationWidth stationDepth 0; 0 stationDepth 0;0 0 stationHeight; stationWidth 0 stationHeight; stationWidth stationDepth stationHeight; 0 stationDepth stationHeight];
stationFaces = [1 2 6 5; 2 3 7 6; 3 4 8 7; 4 1 5 8; 1 2 3 4; 5 6 7 8];
station = patch('Vertices', stationVertices, 'Faces', stationFaces, 'FaceColor', stationColor);

% 建模高铁列车
trainLength = 30; % 高铁列车的长度
trainWidth = 10; % 高铁列车的宽度
trainHeight = 5; % 高铁列车的高度
trainColor = 'blue'; % 高铁列车的颜色

% 创建高铁列车的长方体
trainVertices = [0 0 0; trainLength 0 0; trainLength trainWidth 0; 0 trainWidth 0;
                 0 0 trainHeight; trainLength 0 trainHeight; trainLength trainWidth trainHeight; 0 trainWidth trainHeight];
trainFaces = [1 2 6 5; 2 3 7 6; 3 4 8 7; 4 1 5 8; 1 2 3 4; 5 6 7 8];
train = patch('Vertices', trainVertices, 'Faces', trainFaces, 'FaceColor', trainColor);

% 建模站台上的圆柱体
cylinderRadius = 5; % 圆柱体的半径
cylinderHeight = 20; % 圆柱体的高度
cylinderColor = 'green'; % 圆柱体的颜色
cylinderX = stationWidth / 2; % 圆柱体的X坐标

% 创建站台上的圆柱体
numCylinders = 4; % 圆柱体的数量
cylinders = gobjects(numCylinders, 1);
for i = 1:numCylinders
    cylinderY = (i-1) * (stationDepth / (numCylinders+1));
    cylinderZ = 0;
    [cylinderXMesh, cylinderYMesh, cylinderZMesh] = cylinder(cylinderRadius);
    cylinderZMesh = cylinderZMesh * cylinderHeight;
    cylinders(i) = surf(cylinderXMesh+cylinderX, cylinderYMesh+cylinderY, cylinderZMesh+cylinderZ, 'FaceColor', cylinderColor);
end

% 创建三维场景
figure;
hold on;
axis equal;
view(3);
grid on;

% 添加高铁站、高铁列车和站台上的圆柱体到图形窗口
addpatch(station);
addpatch(train);
for i = 1:numCylinders
    addpatch(cylinders(i));
end

% 标题和轴标签
title('高铁站建模');
xlabel('X轴');
ylabel('Y轴');
zlabel('Z轴');