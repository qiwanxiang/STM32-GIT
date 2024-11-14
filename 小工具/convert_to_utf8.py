import os
import chardet

def detect_encoding(file_path):
    with open(file_path, 'rb') as f:
        result = chardet.detect(f.read())
        print(f'Detected encoding of {file_path}: {result}')
        return result['encoding']

def convert_to_utf8(file_path):
    # 检测文件的原始编码
    original_encoding = detect_encoding(file_path)
    if original_encoding is None or original_encoding.lower() == 'utf-8':
        print(f'Skipping {file_path} (already UTF-8 or encoding detection failed)')
        return  # 如果无法确定编码或已经是UTF-8，则跳过
    
    try:
        # 读取文件内容并解码为原始编码
        with open(file_path, 'r', encoding=original_encoding, errors='replace') as file:
            content = file.read()
        
        # 将内容以UTF-8格式写回文件
        with open(file_path, 'w', encoding='utf-8') as file:
            file.write(content)
        print(f'Converted {file_path} to UTF-8')
    except Exception as e:
        print(f'Error converting {file_path}: {e}')

def traverse_files(directory):
    for root, dirs, files in os.walk(directory):
        for name in files:
            if name.endswith('.c') or name.endswith('.h')or name.endswith('.txt'):
                file_path = os.path.join(root, name)
                print(f'Processing {file_path}')
                convert_to_utf8(file_path)

if __name__ == '__main__':
    current_directory = os.getcwd()
    traverse_files(current_directory)