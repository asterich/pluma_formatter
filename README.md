## Pluma Formatter

一个用 CLR(1)写就的笨重的、有 bug 的 formatter, 感觉配不上这个名字（笑

支持大部分 C89 语法，能够应付课设的任务了罢

羽毛笔镇楼

![pluma](img/pluma2.jpg)

### 支持功能

- 大部分 C89 语法

  - 表达式

  - 各种复合语句

  - 部分预处理语句(#include、#define，需要在文件开头)

- LR1 表缓存，加快运行速度（依赖 OpenSSL）

### 构建方法

1. \*nix

   ```bash
   cd pluma_formatter
   mkdir build
   cd build
   cmake ..
   make
   ```

2. Windows

   （应该不支持）

### 使用方法

```bash
# 可执行文件在 build/src 下
cd build/src
./main -o <output_file> <input_file>
```

### 不支持的语法（已知）

1. 类型转换

   ```c
   int *ptr = (int *)src;
   ```

2. 函数体内的自定义类型

   ```c
   int main() {
      Parser parser = yaju + 114514;
   }
   ```

3. 注释

   ```c
   // 不支持的注释
   ```

4. 部分预处理语句

   ```c
   int foo() {
   // 不支持函数内的预处理语句
   #define JUDA "juda"
       printf("%s", JUDA);
   #ifndef JUDA
   #define JUDA "huge"
   }

   // 不支持函数间的预处理语句
   #include <unistd.h>

   int bar() {}
   ```
