# TinySTL -- 我的 STL 实现


## 目的/笔记
* 理解 C++ 标准中 STL 的设计与实现
* 实现常用的数据结构和算法
* 实践 Google C++ 开源项目风格指南
* 实践 C++11 标准
  * `typename` 替代 `class` 在 template 中的型别形参使用。1. `typename` 比 `class` 更专一。 2. `typename` 名字本身比 `class` 更适合。3. `typename 更通用`（带依赖型别前面必须是 `typename`）。
 
  * `using（Type alias）` 替代 `typedef` 来定义类型。1. `using` 更通用（支持 alias template）。2. `using` 形式上更清晰（`using const_ref = const T &` 与 `typedef const T &const_ref`) 。3. `typedef` 更专一，名字更准确。
  
  * `static_case<type>` 替代 `type()` 来进行类型装换。1.`static_case` 更专一，更方便后期搜索。2. `static_case<type>` 显示更啰嗦
* 实践各种 C++ idiom
  * [Rule of Five](https://en.wikipedia.org/wiki/Rule_of_three_(C%2B%2B_programming)
   : copy * 2 + move * 2 + dtor
  * [copy-and-swap](https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom/3279550?newreg=5e74932504a04ad3b95e89ecf3e4b179)
   : avoiding code duplication and providing a strong exception guarantee.

* 熟悉各种专业名词
  * 资源管理
  * exception safely
  * lvalue/rvalue 相对于等于号，命名为左值（常出现在等号左边）/右值（常出现在等号右边）最大区别是是否能取内存地址。rvalue是特殊的，例如函数返回的值就是rvalue。
  * [ADL](https://en.cppreference.com/w/cpp/language/adl)：Argument-dependent lookup，基于参数的（函数）查找。
  ```C++
  std::cout << "Test\n";         // 全局 namespace 中是没有 << 操作符的，但 ADL 回去 std 中查找，因为，该函数的左参数来自 std
  std::cout std::<< "Test\n";    //
  ```
## 已实现的数据结构
## 项目文件结构说明
## 
