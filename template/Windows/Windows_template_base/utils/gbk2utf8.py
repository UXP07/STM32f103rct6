import os
import argparse

def is_gbk(file_path):
    """检测文件是否为 GBK 编码"""
    try:
        with open(file_path, 'rb') as f:
            content = f.read()
            content.decode('gbk')
        return True
    except UnicodeDecodeError:
        return False

def convert_file(file_path, backup=True):
    """将单个文件从 GBK 转换为 UTF-8"""
    if not is_gbk(file_path):
        print(f"[跳过] 非 GBK 文件: {file_path}")
        return False

    try:
        with open(file_path, 'rb') as f:
            content = f.read().decode('gbk')

        if backup:
            backup_path = file_path + '.bak'
            os.rename(file_path, backup_path)
            print(f"[备份] 原文件已备份至: {backup_path}")

        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)

        print(f"[成功] 已转换: {file_path}")
        return True

    except Exception as e:
        print(f"[失败] 转换出错: {file_path} | 错误: {e}")
        return False

def batch_convert(directory, extensions=None, backup=True):
    """批量转换目录下所有指定扩展名的文件"""
    extensions = set(ext.lower() for ext in extensions) if extensions else None
    count = 0

    for root, _, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            ext = os.path.splitext(file)[1].lower()

            if extensions and ext not in extensions:
                continue

            if convert_file(file_path, backup):
                count += 1

    print(f"\n✅ 转换完成，共处理 {count} 个文件。")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="批量将 GBK 编码文件转换为 UTF-8 编码")
    parser.add_argument("path", help="要转换的目录路径")
    parser.add_argument("--ext", nargs="*", default=[".txt", ".c", ".h"], help="要转换的文件扩展名（默认 .txt .c .h）")
    parser.add_argument("--no-backup", action="store_true", help="不备份原文件")
    args = parser.parse_args()

    if not os.path.isdir(args.path):
        print(f"❌ 错误：目录不存在: {args.path}")
        exit(1)

    batch_convert(args.path, args.ext, backup=not args.no_backup)
