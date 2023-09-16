
#!/bin/bash

# 获取当前目录中的所有dot文件
dot_files=$(find . -maxdepth 1 -type f -name "*.dot")

echo $dot_files

# 循环遍历每个dot文件
for file in $dot_files; do
    # 使用dot命令操作dot文件
    file=$(echo $file | cut -d. -f2)
    echo $file
    dot -Tpng -o ./$file.png ./$file.dot
    rm ./$file.dot
done

png_files=$(find . -maxdepth 1 -type f -name "*.png")

echo $png_files

for file in $png_files; do
    # 使用dot命令操作dot文件
    mv $file /mnt/c/Users/14094/Downloads/$file
done
