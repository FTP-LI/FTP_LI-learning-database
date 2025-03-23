function combine_svg_images()
    % 读取四张SVG图片，移除XML声明和DOCTYPE
    svg1 = clean_svg_content(fileread('1.svg'));
    svg2 = clean_svg_content(fileread('2.svg'));
    svg3 = clean_svg_content(fileread('3.svg'));
    svg4 = clean_svg_content(fileread('4.svg'));
    
    % 从每张SVG文件中提取出宽度和高度
    [w1, h1] = get_svg_dimensions(svg1);
    [w2, h2] = get_svg_dimensions(svg2);
    [w3, h3] = get_svg_dimensions(svg3);
    [w4, h4] = get_svg_dimensions(svg4);
    
    % 计算拼接后的总宽度和高度
    total_width = max(w1 + w2, w3 + w4);
    total_height = h1 + h3;
    
    % 创建新的SVG文档
    svg_combined = sprintf(['<?xml version="1.0" encoding="UTF-8"?>\n' ...
        '<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" version="1.1">\n' ...
        '<g>%s</g>\n' ...  % 第一张图片
        '<g transform="translate(%d,0)">%s</g>\n' ... % 第二张图片
        '<g transform="translate(0,%d)">%s</g>\n' ... % 第三张图片
        '<g transform="translate(%d,%d)">%s</g>\n' ... % 第四张图片
        '</svg>'], ...
        total_width, total_height, ...
        extract_svg_content(svg1), ...
        w1, extract_svg_content(svg2), ...
        h1, extract_svg_content(svg3), ...
        w3, h1, extract_svg_content(svg4));
    
    % 保存拼接后的SVG文件
    fid = fopen('combined_image.svg', 'w');
    if fid == -1
        error('无法创建输出文件');
    end
    fprintf(fid, svg_combined);
    fclose(fid);
    disp('拼图完成，保存为 combined_image.svg');
end

function [width, height] = get_svg_dimensions(svg_str)
    % 提取SVG尺寸
    width_pattern = 'width="(\d+)"';
    height_pattern = 'height="(\d+)"';
    
    width_match = regexp(svg_str, width_pattern, 'tokens');
    height_match = regexp(svg_str, height_pattern, 'tokens');
    
    width = str2double(width_match{1}{1});
    height = str2double(height_match{1}{1});
end

function cleaned_content = clean_svg_content(content)
    % 移除XML声明和DOCTYPE
    content = regexprep(content, '<\?xml.*?\?>\s*', '');
    content = regexprep(content, '<!DOCTYPE.*?>\s*', '');
    cleaned_content = strtrim(content);
end

function svg_content = extract_svg_content(svg_str)
    % 提取SVG标签内的内容
    pattern = '<svg[^>]*>(.*)</svg>';
    tokens = regexp(svg_str, pattern, 'tokens', 'dotall');
    if ~isempty(tokens)
        svg_content = tokens{1}{1};
    else
        svg_content = '';
    end
end