# ZnExpress
表达式解释器 (调试环境：VS2010 SP1)

说明：

    标准库的正则库不支持(?<=)和(?!=) 
    
    这导致在取加减号和正负数符号时我不知道怎么做了，所以这时候就要用boost库的正则库
    
    如果你不需要区分正负号或者能够写出提取正负号的正则表达式也可以直接用标准库的正则库
    
    通过_ZNEXPRESS_USE_BOOST 宏业控制是否使用boost的正则库
    
    如要扩展，新建自己的value、factory、operator类
    
  一个四则运算的使用方法示例：
  
   包含头文件前使用下面这个宏，使用boost的正则库来支持(?<=)
   
 #define _ZNEXPRESS_USE_BOOST
 
   要计算的表达式
   
 const ZnExpress::t_zn_char sExp[] = _T("(5+(+3--2)*-3)/2");
 
   声明默认的整数表达式转换对象，指定提取元素的正则表达式，表达式元素为: ( ) + - * / 和带符号的整数
   
 ZnExpress::its_integer its(_T("\\(|\\)|(?<=\\d)\\+|(?<=\\d)-|\\*|/|[-+]?\\d+"));
 
   将表达式转换为后缀表达式
   
 ZnExpress::its_integer::t_sp_suffix spSuffix = its.to_suffix_sptr(sExp);
 
   得到结果元素
   
 ZnExpress::its_integer::t_sp sp = spSuffix->result();
 
   输出值
   
 std::cout << sExp << " = " << (ZnExpress::its_integer::t_value*)(sp.get())->result() << std::endl;
 
 
 
 
 
 
 
  浮点数四则运算的方法
  
 ZnExpress::its_float its(_T("\\(|\\)|(?<=\\d)\\+|(?<=\\d)-|\\*|/|[-+]?\\d+\\.?\\d*"););
 
 const ZnExpress::t_zn_char sExp[] = _T("5.5+4.2/2.5*3.9-5.2");
 
 ZnExpress::its_float::t_sp_suffix spSuffix = its.to_suffix_sptr(sExp);
 
 ZnExpress::its_float::t_sp sp = spSuffix->result();
 
 std::cout << sExp << " = " << (ZnExpress::its_float::t_value*)(sp.get())->result() << std::endl;

