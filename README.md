## Pluma Formatter

一个用 CLR(1)写就的笨重的、有 bug 的 formatter, 感觉配不上这个名字（笑

支持大部分 C89 语法，能够应付课设的任务了罢

- 构建方法:

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

- 使用方法:

  ```bash
  # 可执行文件在 build/src 下
  cd build/src
  ./main -o <output_file> <input_file>
  ```

- 不支持的语法（已知）

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

  3. 注释和预处理
