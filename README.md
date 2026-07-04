# rename_tool

批量文件重命名工具。

## 功能

- 递归遍历目录
- `fnmatch` 通配符匹配文件名
- 字符串替换重命名
- 目标文件存在时跳过并报错
- `rename` 失败时输出 `errno` 诊断

## 编译

```bash
make
make clean
```

## 使用方法

```bash
./rename_tool <directory> --match <pattern> --replace <replacement>
./rename_tool -h
```

## 示例

```bash
# 普通重命名
./rename_tool test_dir --match "hello.txt" --replace "world.txt"
# [OK] test_dir/hello.txt -> test_dir/world.txt

# 覆盖保护
./rename_tool test_dir --match "hello.txt" --replace "world.txt"
# [ERR] test_dir/hello.txt -> test_dir/world.txt: target exists

# 通配符匹配
./rename_tool test_dir --match "*.txt" --replace ".bak"

# 递归目录
./rename_tool test_dir --match "file2.txt" --replace "doc2.txt"
# [OK] test_dir/subdir/file2.txt -> test_dir/subdir/doc2.txt
```

## 测试

```bash
chmod +x test.sh
./test.sh
git status
```

## 技术栈

C99、POSIX API、`getopt_long`、`opendir`/`readdir`、`rename`
