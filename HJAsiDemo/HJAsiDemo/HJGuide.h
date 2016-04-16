//
//  HJGuide.h
//  HJAsiDemo
//
//  Created by huangjiong on 16/4/15.
//  Copyright © 2016年 huangjiong123. All rights reserved.
//

#ifndef HJGuide_h
#define HJGuide_h


#endif /* HJGuide_h */

官方文档翻译:
http://www.cnblogs.com/pengyingh/articles/2360528.html


1.添加库:
CFNetwork.framework
MobileCoreServices.framework
SystemConfiguration.framework
libz.tbd

2.-fno-objc-arc


如果需要处理许多不同类型的请求，可以这么处理：

1. 如果你的请求都属于一个大的范围，你又想区别他们，可以设置请求的userInfo字典属性，填入你的自定义数据，你就可以在代理方法中读取了。你也可以更简单的设置tag属性来区分不同请求。这些属性都是给你自己用的，不会发送到服务器。

2. 如果你处理完全不同的请求，可以为每个请求设置不同的 setDidFinishSelector / setDidFailSelector 来覆盖默认的代理方法。

3. 对更复杂的情况，或者你想在后台解析响应数据，可以为每个请求类型创建一个ASIHTTPRequest的子类，然后重载requestFinished: 和 failWithError: 。

4. 注意：最好避免在代理方法中，用url来区分不同的请求，因为url属性在重定向时会改变。如果你真的真的想用的话，请用[request originalURL] 代替，这个将总是记录第请求连接时的首个url。
