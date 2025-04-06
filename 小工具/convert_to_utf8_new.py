import os
import chardet
import tempfile
import shutil


def detect_encoding(file_path):
    try:
        # 处理空文件
        if os.path.getsize(file_path) == 0:
            return None, 0.0

        with open(file_path, 'rb') as f:
            raw_data = f.read(100000)  # 限制检测数据量以提高性能

        result = chardet.detect(raw_data)
        print(f'Detected encoding: {result["encoding"]} (confidence: {result["confidence"]:.2f})')
        return result['encoding'].lower(), result['confidence']
    except Exception as e:
        print(f'Detection error: {e}')
        return None, 0.0


def safe_convert(file_path, original_encoding):
    # 创建临时文件
    temp_fd, temp_path = tempfile.mkstemp()
    os.close(temp_fd)

    try:
        # 保留原始换行符
        with open(file_path, 'r', encoding=original_encoding, errors='replace', newline='') as infile:
            content = infile.read()

        # 写入UTF-8（不带BOM）
        with open(temp_path, 'w', encoding='utf-8', newline='', errors='replace') as outfile:
            outfile.write(content)

        # 替换原文件
        shutil.move(temp_path, file_path)
        print(f'Successfully converted: {file_path}')
    except Exception as e:
        print(f'Conversion failed: {e}')
        if os.path.exists(temp_path):
            os.remove(temp_path)


def convert_to_utf8(file_path):
    # 跳过符号链接
    if os.path.islink(file_path):
        print(f'Skipping symbolic link: {file_path}')
        return

    original_encoding, confidence = detect_encoding(file_path)

    # 有效性检查
    if not original_encoding:
        print(f'Skipping: {file_path} (empty or detection failed)')
        return
    if confidence < 0.8:  # 置信度阈值
        print(f'Skipping: {file_path} (low confidence: {confidence:.2f})')
        return

    # 排除不需要转换的情况
    if original_encoding in {'utf-8', 'utf_8', 'utf8', 'ascii'}:
        print(f'Skipping: {file_path} (already compatible)')
        return

    try:
        safe_convert(file_path, original_encoding)
    except PermissionError:
        print(f'Permission denied: {file_path}')
    except Exception as e:
        print(f'Unexpected error: {e}')


def traverse_files(directory):
    for root, dirs, files in os.walk(directory, followlinks=False):  # 不跟随符号链接
        # 过滤系统目录（可选）
        dirs[:] = [d for d in dirs if not d.startswith('.')]

        for name in files:
            # 大小写不敏感的扩展名检查
            if name.lower().endswith(('.c', '.h', '.txt')):
                file_path = os.path.join(root, name)
                print('-' * 50)
                print(f'Processing: {file_path}')
                convert_to_utf8(file_path)


if __name__ == '__main__':
    current_directory = os.getcwd()
    print(f'Starting conversion in: {current_directory}')
    traverse_files(current_directory)
    print('Conversion process completed')