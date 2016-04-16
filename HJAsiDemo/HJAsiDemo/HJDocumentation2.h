//
//  HJDocumentation2.h
//  HJAsiDemo
//
//  Created by huangjiong on 16/4/16.
//  Copyright © 2016年 huangjiong123. All rights reserved.
//

#ifndef HJDocumentation2_h
#define HJDocumentation2_h


#endif /* HJDocumentation2_h */

#pragma mark - iOS开发ASIHttpRequest库简介、配置和安装
使用ASIHTTPRequest可以很方便的进行一下操作：同步/异步方式下载数据,定义下载队列，让队列中的任务按指定的并发数来下载（队列下载必须是异步的),提交表单，文件上传,处理cookie,设置代理,上下载进度条,重定向处理,请求与响应的GZIP,验证与授权。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
使用ASIHTTPRequest可以很方便的进行一下操作：

同步/异步方式下载数据
定义下载队列，让队列中的任务按指定的并发数来下载（队列下载必须是异步的)
提交表单，文件上传
处理cookie
设置代理
上下载进度条
重定向处理
请求与响应的GZIP
验证与授权
等等，只要跟HTTP有关，只有你想不到的，没有她做不到的~

配置方法：

ASIHTTPRequestConfig.h
ASIHTTPRequestDelegate.h
ASIProgressDelegate.h
ASICacheDelegate.h
ASIHTTPRequest.h
ASIHTTPRequest.m
ASIDataCompressor.h
ASIDataCompressor.m
ASIDataDecompressor.h
ASIDataDecompressor.m
ASIFormDataRequest.h
ASIInputStream.h
ASIInputStream.m
ASIFormDataRequest.m
ASINetworkQueue.h
ASINetworkQueue.m
ASIDownloadCache.h
ASIDownloadCache.m
iPhone 工程还需要:

ASIAuthenticationDialog.h
ASIAuthenticationDialog.m
Reachability.h (在External/Reachability 目录下)
Reachability.m (在 External/Reachability 目录下)
库引用：

CFNetwork.framework

SystemConfiguration.framework

MobileCoreServices.framework

CoreGraphics.framework

和libz.dylib

另外，还需要libxml2.dylib(libxml2还需要设置连接选项-lxml2 和头文件搜索路径/usr/include/libxml2)




#pragma mark - 一.iOS开发ASIHttpRequest创建和执行request

本文为大家介绍了iOS开发中ASIHttpRequest如何创建和执行request，其中包括同步请求，异步请求，使用Block，使用队列，取消异步请求等等内容。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
本文为大家介绍了iOS开发中ASIHttpRequest如何创建和执行request，其中包括同步请求，异步请求，使用Block，使用队列，取消异步请求等等内容。

创建NSOperationQueue，这个Cocoa架构的执行任务（NSOperation）的任务队列。我们通过ASIHTTPRequest.h的源码可以看到，此类本身就是一个NSOperation的子类。也就是说它可以直接被放到任务队列中并被执行。

同步请求

同步请求会在当前线程中执行，使用error属性来检查结束状态（要下载大文件，则需要设定downloadDestinationPath来保存文件到本地）：

- (IBAction)grabURL:(id)sender
{
    NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com"];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    [request startSynchronous];
    NSError *error = [request error];
    if (!error) {
        NSString *response = [request responseString];
    }
}
同步请求会阻塞主线程的执行，这导致用户界面不响应用户操作，任何动画都会停止渲染。

异步请求

下面是最简单的异步请求方法，这个request会在全局的NSOperationQueue中执行，若要进行更复杂的操作，我们需要自己创建NSOperationQueue或者ASINetworkQueue，后面会讲到。

- (IBAction)grabURLInBackground:(id)sender
{
    NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com"];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    [request setDelegate:self];
    [request startAsynchronous];
}

- (void)requestFinished:(ASIHTTPRequest *)request
{
    // Use when fetching text data
    NSString *responseString = [request responseString];
    
    // Use when fetching binary data
    NSData *responseData = [request responseData];
}

- (void)requestFailed:(ASIHTTPRequest *)request
{
    NSError *error = [request error];
}
使用block

在平台支持情况下，ASIHTTPRequest1.8以上支持block。

- (IBAction)grabURLInBackground:(id)sender
{
    NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com"];
    __block ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    [request setCompletionBlock:^{
        // Use when fetching text data
        NSString *responseString = [request responseString];
        
        // Use when fetching binary data
        NSData *responseData = [request responseData];
    }];
    [request setFailedBlock:^{
        NSError *error = [request error];
    }];
    [request startAsynchronous];
}
注意，声明request时要使用__block修饰符，这是为了告诉block不要retain request，以免出现retain循环，因为request是会retain block的。

使用队列

创建NSOperationQueue或者ASINetworkQueue队列，我们还可以设定最大并发连接数：maxConcurrentOperationCount

- (IBAction)grabURLInTheBackground:(id)sender
{
    if (![self queue]) {
        [self setQueue:[[[NSOperationQueue alloc] init] autorelease]];
        [self queue].maxConcurrentOperationCount = 4;
    }
    
    NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com"];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    [request setDelegate:self];
    [request setDidFinishSelector:@selector(requestDone:)];
    [request setDidFailSelector:@selector(requestWentWrong:)];
    [[self queue] addOperation:request]; //queue is an NSOperationQueue
}

- (void)requestDone:(ASIHTTPRequest *)request
{
    NSString *response = [request responseString];
}

- (void)requestWentWrong:(ASIHTTPRequest *)request
{
    NSError *error = [request error];
}
如果不设定selector，那么系统会使用默认的requestFinished: 和 requestFailed:方法

如果需要对队列里面的每个request进行区分，那么可以设定request的userInfo属性，它是个NSDictionary，或者更简单的方法是设定每个request的tag属性，这两个属性都不会被发送到服务器。

不要使用request的URL来区分每个request，因为URL可能会改变（例如重定向），如果需要使用request的URL，使用[request originalURL]，这个将永远返回第一个url。

对于ASINetworkQueue

ASINetworkQueue是NSOperationQueue的子类，提供更高级的特性（ASINetworkQueue的代理函数）：

requestDidStartSelector
当一个request开始执行时，这个代理函数会被调用。
requestDidReceiveResponseHeadersSelector
当队列中的request收到服务器返回的头信息时，这个代理函数会被调用。对于下载很大的文件，这个通常比整个request的完成要早。
requestDidFinishSelector
当每个request完成时，这个代理函数会被调用。
requestDidFailSelector
当每个request失败时，这个代理函数会被调用。
queueDidFinishSelector
当队列完成（无论request失败还是成功）时，这个代理函数会被调用。
ASINetworkQueues与NSOperationQueues稍有不同，加入队列的request不会立即开始执行。如果队列打开了进度开关，那么队列开始时，会先对所有GET型request进行一次HEAD请求，获得总下载大小，然后真正的request才被执行。

向一个已经开始进行的ASINetworkQueue 加入request会怎样？

如果你使用ASINetworkQueue来跟踪若干request的进度，只有当新的request开始执行时，总进度才会进行自适应调整（向后移动）。ASINetworkQueue不会为队列开始后才加入的request进行HEAD请求，所以如果你一次向一个正在执行的队列加入很多request，那么总进度不会立即被更新。

如果队列已经开始了，不需要再次调用[queue go]。

当ASINetworkQueue中的一个request失败时，默认情况下，ASINetworkQueue会取消所有其他的request。要禁用这个特性，设置 [queue setShouldCancelAllRequestsOnFailure:NO]。

ASINetworkQueues只可以执行ASIHTTPRequest操作，二不可以用于通用操作。试图加入一个不是ASIHTTPRequest的NSOperation将会导致抛出错误。

取消异步请求

取消一个异步请求（无论request是由[request startAsynchronous]开始的还是从你创建的队列中开始的），使用[request cancel]即可。注意同步请求不可以被取消。

注意，如果你取消了一个request，那么这个request将会被视为请求失败，并且request的代理或者队列的代理的失败代理函数将被调用。如果你不想让代理函数被调用，那么将delegate设置为nil，或者使用clearDelegatesAndCancel方法来取消request。

clearDelegatesAndCancel 将会首先清除所有的代理和block。

当使用ASINetworkQueue时，如果取消了队列中的一个request，那么队列中其他所有request都会被取消，可以设置shouldCancelAllRequestsOnFailure的值为NO来避免这个现象。

安全地控制delegate防止request完成之前代理被释放

request并不retain它们的代理，所以有可能你已经释放了代理，而之后request完成了，这将会引起崩溃。大多数情况下，如果你的代理即将被释放，你一定也希望取消所有request，因为你已经不再关心它们的返回情况了。如此做：

// 代理类的dealloc函数
- (void)dealloc
{
    [request clearDelegatesAndCancel];
    [request release]; 
    ... 
    [super dealloc]; 
}



#pragma mark - 二.iOS开发ASIHttpRequest发送数据

本文为大家介绍了iOS开发ASIHttpRequest发送数据的内容，其中包括设定request头，使用ASIFormDataRequest POST表单，PUT请求、自定义POST请求等等内容。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
本文为大家介绍了iOS开发ASIHttpRequest发送数据的内容，其中包括设定request头，使用ASIFormDataRequest POST表单，PUT请求、自定义POST请求等等内容。

设定request头

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request addRequestHeader:@"Referer" value:@"http://www.dreamingwish.com/"];
使用ASIFormDataRequest POST表单

通常数据是以’application/x-www-form-urlencoded’格式发送的，如果上传了二进制数据或者文件，那么格式将自动变为‘multipart/form-data’ 。

文件中的数据是需要时才从磁盘加载，所以只要web server能处理，那么上传大文件是没有问题的。

ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
[request setPostValue:@"Ben" forKey:@"first_name"];
[request setPostValue:@"Copsey" forKey:@"last_name"];
[request setFile:@"/Users/ben/Desktop/ben.jpg" forKey:@"photo"];
数据的mime头是自动判定的，但是如果你想自定义mime头，那么这样：

ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];

// Upload a file on disk
[request setFile:@"/Users/ben/Desktop/ben.jpg" withFileName:@"myphoto.jpg" andContentType:@"image/jpeg"
          forKey:@"photo"];

// Upload an NSData instance
[request setData:imageData withFileName:@"myphoto.jpg" andContentType:@"image/jpeg" forKey:@"photo"];
你可以使用addPostValue方法来发送相同name的多个数据（梦维：服务端会以数组方式呈现）：

ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
[request addPostValue:@"Ben" forKey:@"names"];
[request addPostValue:@"George" forKey:@"names"];
[request addFile:@"/Users/ben/Desktop/ben.jpg" forKey:@"photos"];
[request addData:imageData withFileName:@"george.jpg" andContentType:@"image/jpeg" forKey:@"photos"];
PUT请求、自定义POST请求

如果你想发送PUT请求，或者你想自定义POST请求，使用appendPostData: 或者 appendPostDataFromFile:

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request appendPostData:[@"This is my data" dataUsingEncoding:NSUTF8StringEncoding]];
// Default becomes POST when you use appendPostData: / appendPostDataFromFile: / setPostBody:
[request setRequestMethod:@"PUT"];




#pragma mark - 三.iOS开发ASIHTTPRequest进度追踪

本文为大家介绍了发ASIHTTPRequest进度追踪的内容，其中包括追踪单个request的下载进度,追踪一系列request的下载进度,追踪单个request的上传进度,追踪一系列request的上传进度,精确进度条vs简单进度条,自定义进度追踪等等内容。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
本文为大家介绍了发ASIHTTPRequest进度追踪的内容，其中包括追踪单个request的下载进度,追踪一系列request的下载进度,追踪单个request的上传进度,追踪一系列request的上传进度,精确进度条vs简单进度条,自定义进度追踪等等内容。

每个ASIHTTPRequest有两个delegate用来追踪进度：

downloadProgressDelegate (下载) 和 uploadProgressDelegate (上载)。

进度delegate可以是NSProgressIndicators (Mac OS X) 或者 UIProgressViews (iPhone).ASIHTTPRequest会自适应这两个class的行为。你也可以使用自定义class作为进度delegate，只要它响应setProgress:函数。

如果你执行单个request，那么你需要为该request设定upload/download进度delegate
如果你在进行多个请求，并且你想要追踪整个队列中的进度，你必须使用ASINetworkQueue并设置队列的进度delegate
如果上述两者你想同时拥有，恭喜你，0.97版以后的ASIHTTPRequest，这个可以有 ^ ^
IMPORTANT:如果你向一个要求身份验证的网站上传数据，那么每次授权失败，上传进度条就会被重置为上一次的进度值。因此，当与需要授权的web服务器交互时，建议仅当useSessionPersistence为YES时才使用上传进度条，并且确保你在追踪大量数据的上传进度之前，先使用另外的request来进行授权。

追踪小于128KB的数据上传进度目前无法做到，而对于大于128kb的数据，进度delegate不会收到第一个128kb数据块的进度信息。这是因为CFNetwork库API的限制。我们曾向apple提交过bug报告（bug id 6596016），希望apple能修改CFNetwork库以便实现上述功能。

2009-6-21：Apple的哥们儿们真棒！iPhone 3.0 SDK里，buffer大小已经被减小到32KB了，我们的上传进度条可以更精确了。

追踪单个request的下载进度
这个例子中， myProgressIndicator是个 NSProgressIndicator.

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setDownloadProgressDelegate:myProgressIndicator];
[request startSynchronous];
NSLog(@"Max: %f, Value: %f", [myProgressIndicator maxValue],[myProgressIndicator doubleValue]);
追踪一系列request的下载进度
在这个例子中, myProgressIndicator 是个 UIProgressView, myQueue是个 ASINetworkQueue.

- (void)fetchThisURLFiveTimes:(NSURL *)url
{
    [myQueue cancelAllOperations];
    [myQueue setDownloadProgressDelegate:myProgressIndicator];
    [myQueue setDelegate:self];
    [myQueue setRequestDidFinishSelector:@selector(queueComplete:)];
    int i;
    for (i=0; i<5; i++) {
        ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
        [myQueue addOperation:request];
    }
    [myQueue go];
}

- (void)queueComplete:(ASINetworkQueue *)queue
{
    NSLog(@"Value: %f", [myProgressIndicator progress]);
}
这个例子中，我们已经为ASINetworkQueues调用过[myQueue go]了。

追踪单个request的上传进度
在这个例子中, myProgressIndicator 是个 UIProgressView。

ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
[request setPostValue:@"Ben" forKey:@"first_name"];
[request setPostValue:@"Copsey" forKey:@"last_name"];
[request setUploadProgressDelegate:myProgressIndicator];
[request startSynchronous];
NSLog(@"Value: %f",[myProgressIndicator progress]);
追踪一系列request的上传进度
这个例子中, myProgressIndicator是个 NSProgressIndicator, myQueue是个ASINetworkQueue.

- (void)uploadSomethingFiveTimes:(NSURL *)url
{
    [myQueue cancelAllOperations];
    [myQueue setUploadProgressDelegate:myProgressIndicator];
    [myQueue setDelegate:self];
    [myQueue setRequestDidFinishSelector:@selector(queueComplete:)];
    int i;
    for (i=0; i<5; i++) {
        ASIHTTPRequest *request = [ASIFormDataRequest requestWithURL:url];
        [request setPostBody:[@"Some data" dataUsingEncoding:NSUTF8StringEncoding]];
        [myQueue addOperation:request];
    }
    [myQueue go];
}

- (void)queueComplete:(ASINetworkQueue *)queue
{
    NSLog(@"Max: %f, Value: %f", [myProgressIndicator maxValue],[myProgressIndicator doubleValue]);
}
精确进度条vs简单进度条
ASIHTTPRequest提供两种进度条显示，简单进度条和精确进度条，使用ASIHTTPRequests 和ASINetworkQueues的showAccurateProgress 来控制。为一个request设置showAccurateProgress只会对该request有效。如果你为一个队列设置showAccurateProgress，那么会影响队列里所有的request。

简单进度条
当使用简单进度条时，进度条只会在一个request完成时才更新。对于单个request，这意味着你只有两个进度状态：0%和100%。对于一个有5个request的队列来说，有五个状态：0%，25%，50%，75%，100%，每个request完成时，进度条增长一次。

简单进度条（showAccurateProgress = NO）是ASINetworkQueue的默认值，适用于大量小数据请求。

精确进度条
当使用精确进度条时，每当字节被上传或下载时，进度条都会更新。它适用于上传/下载大块数据的请求，并且会更好的显示已经发送/接收的数据量。

使用精确进度条追踪上传会轻微降低界面效率，因为进度delegate（一般是UIProgressViews 或NSProgressIndicators）会更频繁地重绘。

使用精确进度条追踪下载会更影响界面效率，因为队列会先为每个GET型request进行HEAD请求，以便统计总下载量。强烈推荐对下载大文件的队列使用精确进度条，但是要避免对大量小数据请求使用精确进度条。

精确进度条（showAccurateProgress = YES）是以同步方式执行的ASIHTTPRequest的默认值。

自定义进度追踪
ASIProgressDelegate 协议定义了所有能更新一个request进度的方法。多数情况下，设置你的uploadProgressDelegate或者 downloadProgressDelegate为NSProgressIndicator或者UIProgressView会很好。但是，如果你想进行更复杂的追踪，你的进度delegate实现下列函数要比 setProgress: (iOS) 或者 setDoubleValue: / setMaxValue: (Mac)好：

这些函数允许你在实际量的数据被上传或下载时更新进度，而非简单方法的0到1之间的数字。

downloadProgressDelegates方法
request:didReceiveBytes: 每次request下载了更多数据时，这个函数会被调用（注意，这个函数与一般的代理实现的 request:didReceiveData:函数不同）。
request:incrementDownloadSizeBy: 当下载的大小发生改变时，这个函数会被调用，传入的参数是你需要增加的大小。这通常发生在request收到响应头并且找到下载大小时。
uploadProgressDelegates方法
request:didSendBytes: 每次request可以发送更多数据时，这个函数会被调用。注意：当一个request需要消除上传进度时（通常是该request发送了一段数据，但是因为授权失败或者其他什么原因导致这段数据需要重发）这个函数会被传入一个小于零的数字。



#pragma mark - 四.iOS开发ASIHTTPRequest身份验证

本文为大家介绍了iOS开发ASIHTTPRequest身份验证的内容，其中包括为URL指定要使用的用户名和密码,为request指定要使用的用户名和密码,将凭据存储到keychain,将凭据存储到session中,NTLM授权,使用代理来提供凭据,使用内建的授权对话框（目前只对iOS有效）,在服务器请求凭据前向服务器发送凭据等等内容。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
本文为大家介绍了iOS开发ASIHTTPRequest身份验证的内容，其中包括为URL指定要使用的用户名和密码,为request指定要使用的用户名和密码,将凭据存储到keychain,将凭据存储到session中,NTLM授权,使用代理来提供凭据,使用内建的授权对话框（目前只对iOS有效）,在服务器请求凭据前向服务器发送凭据等等内容。

ASIHTTPRequest是简单易用的，它封装了CFNetwork API。使得与Web服务器通信变得更简单。它是用Objective-C编写的，可以在MAC OS X和iPhone应用中使用。

你可以查阅下图ASIHTTPRequest授权流程图来了解ASIHTTPRequest如何找到授权凭据，并将授权凭据应用到request上。



为URL指定要使用的用户名和密码

NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
为request指定要使用的用户名和密码

NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setUsername:@"username"];
[request setPassword:@"password"];
将凭据存储到keychain

如果打开了keychainPersistence，所有提供的可用的用户名和密码将被存储到keychain中，以后的request将会重用这些用户名密码，即使你关闭程序后重新打开也不影响。

NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setUseKeychainPersistence:YES];
[request setUsername:@"username"];
[request setPassword:@"password"];
如果你使用keychain但是想要自己管理它，你可以在ASIHTTPRequest.h文件里找到相关的类方法。

将凭据存储到session中

如果打开了useSessionPersistence（默认即是如此），ASIHTTPRequest会把凭据存储到内存中，后来的request将会重用这些凭据。

NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setUsername:@"username"];
[request setPassword:@"password"];
[request setUseSessionPersistence:YES]; //这一项是默认的，所以并不必要

//将会重用我们的 username 和 password
request = [ASIHTTPRequest requestWithURL:url];
NTLM授权

要使用NTLM授权的Windows服务器，你还需要指定你要进行授权域。

NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setUsername:@"username"];
[request setPassword:@"password"];
[request setDomain:@"my-domain"];
使用代理来提供凭据

你不一定非要提前指定授权凭据，你还可以让每个request在无法从session或keychain中找到凭据时向它们的代理请求凭据。如果你要连接到一个你并不清楚授权类型的服务器时，这是很有用的。

你的delegate必须实现authenticationNeededForRequest:方法，当request等待凭据时，ASIHTTPRequest将会暂停这个request。如果你持有你需要的凭据，那么先为request设定凭据，然后调用[request retryUsingSuppliedCredentials]即可。如果你想取消授权，调用[request cancelAuthentication]，此时，这个request也会被取消。

从1.0.8版开始，一次只能有一个request的delegate收到authenticationNeededForRequest: 或者 proxyAuthenticationNeededForRequest:。当delegate处理第一个request时，其他需要授权的request将会被暂停。如果提供了一个凭据，当前进程中所有其他的request将会假定这个凭据对这个URL有效，并尝试重用这个凭据。如果delegate取消了授权，并且队列的shouldCancelAllRequestsOnFailure值为YES，所有其他的request都将被取消（它们也不会尝试请求凭据）。

当进行同步请求时，你不可以使用代理模式来授权。

在较老的版本中，这么做会导致程序假死，从1.0.8开始，即使你这么做了，代理函数也不会被调用。

使用内建的授权对话框（目前只对iOS有效）

这个特性归功于1.0.8版本的新类ASIAuthenticationDialog 。这个特性主要是用于授权代理（后面会介绍到），但是它也可以用来向用户取得授权凭据。

为了更好的用户体验，大多数（连接单一服务的）app必须为request的delegate实现authenticationNeededForRequest:方法，或者避免同时使用代理式授权。

most apps that connect to a single service should implement authenticationNeededForRequest: in their request delegates, or avoid the use of delegation-style authentication altogether.

但是，会有一些情况下，为普通的授权使用ASIHTTPRequest的标准授权对话框更好：

你不想创建你自己的登录表单
你可能需要从外部资源获取数据，但是你不清楚你需不需要进行授权
对于这些情况，为request设置shouldPresentAuthenticationDialog为YES，此时，如果你的代理没有实现



authenticationNeededForRequest:方法，那么用户将会看到这个对话框。

一次同时只有一个对话框可以显示出来，所以当一个对话框显示时，所有其他需要授权的request将会暂停。如果提供了一个凭据，当前进程中所有其他的request将会假定这个凭据对这个URL有效，并尝试重用这个凭据。如果delegate取消了授权，并且队列的shouldCancelAllRequestsOnFailure值为YES，所有其他的request都将被取消（它们也不会尝试请求凭据）。

对于同步请求的request，授权对话框不会显示出来。

这个对话框部分模仿了iPhone上Safari使用的授权对话框，它包含以下内容：

一段信息来说明这些凭据是用于websever（而非一个proxy）
你将要连接到服务器的主机名或者IP
授权域（如果提供的话）
填写用户名和密码的区域
当连接到NTLM授权模式的服务器时，还会包含一个填写domain的区域
一个说明信息，指明凭据是否将会被以明文方式发送（例如：“只有当使用基于非SSL的基本授权模式时才会以明文方式发送”）
如果你想改变它的外观，你必须继承ASIHTTPRequest，并重写showAuthenticationDialog来显示你自己的对话框或ASIAuthenticationDialog子类。

在服务器请求凭据前向服务器发送凭据

IMPORTANT

从1.8.1开始，使用基本授权模式的request时，这个特性的行为改变了。你可能需要修改你的代码。

在第一次生成request时，ASIHTTPRequest可以先向服务器发送凭据（如果有的话），而不是等服务器要求提供凭据时才提供凭据。这个特性可以提高使用授权的程序的执行效率，因为这个特性避免了多余的request。

对于基本授权模式，要触发这个行为，你必须手动设置request的authenticationScheme为kCFHTTPAuthenticationSchemeBasic：

[request setAuthenticationScheme:(NSString *)kCFHTTPAuthenticationSchemeBasic];
对于其他授权方案，凭据也可以在服务器要求之前被发送，但是仅当有另一个request成功授权之后才行。

在以下情况下，你也许想要禁用这个特性：

你的程序可能会一次使用一系列凭据来与服务器对话
安全性对于你的程序来说非常重要。使用这个特性是相对不安全的，因为你不能在凭据被发送前验证你是否连接到了正确的服务器。
要禁用这个特性，这样做：

[request setShouldPresentCredentialsBeforeChallenge:NO];




#pragma mark - 五.iOS开发ASIHTTPRequest中Cookie的使用

本文为大家介绍了iOS开发ASIHTTPRequest中Cookie的使用的内容，其中包括持久化cookie，自己处理cookie等等内容，希望对大家有所帮助。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
本文为大家介绍了iOS开发ASIHTTPRequest中Cookie的使用的内容，其中包括持久化cookie，自己处理cookie等等内容，希望对大家有所帮助。

持久化cookie

ASIHTTPRequest允许你使用全局存储来和所有使用CFNetwork或者NSURLRequest接口的程序共享cookie。

如果设置useCookiePersistence为YES（默认值），cookie会被存储在共享的 NSHTTPCookieStorage 容器中，并且会自动被其他request重用。值得一提的是，ASIHTTPRequest会向服务器发送其他程序创建的cookie（如果这些cookie对特定request有效的话）。

你可以清空session期间创建的所有cookie：

[ASIHTTPRequest setSessionCookies:nil];
这里的‘session cookies’指的是一个session中创建的所有cookie，而非没有过期时间的cookie（即通常所指的会话cookie，这种cookie会在程序结束时被清除）。

另外，有个方便的函数 clearSession可以清除session期间产生的所有的cookie和缓存的授权数据。

自己处理cookie

如果你愿意，你大可以关闭useCookiePersistence，自己来管理某个request的一系列cookie：

//创建一个cookie
NSDictionary *properties = [[[NSMutableDictionary alloc] init] autorelease];
[properties setValue:[@"Test Value" encodedCookieValue] forKey:NSHTTPCookieValue];
[properties setValue:@"ASIHTTPRequestTestCookie" forKey:NSHTTPCookieName];
[properties setValue:@".dreamingwish.com" forKey:NSHTTPCookieDomain];
[properties setValue:[NSDate dateWithTimeIntervalSinceNow:60*60] forKey:NSHTTPCookieExpires];
[properties setValue:@"/asi-http-request/tests" forKey:NSHTTPCookiePath];
NSHTTPCookie *cookie = [[[NSHTTPCookie alloc] initWithProperties:properties] autorelease];

//这个url会返回名为'ASIHTTPRequestTestCookie'的cookie的值
url = [NSURL URLWithString:@"http://www.dreamingwish.com/"];
request = [ASIHTTPRequest requestWithURL:url];
[request setUseCookiePersistence:NO];
[request setRequestCookies:[NSMutableArray arrayWithObject:cookie]];
[request startSynchronous];

//将会打印: I have 'Test Value' as the value of 'ASIHTTPRequestTestCookie'
NSLog(@"%@",[request responseString]);


#pragma mark - 六.iOS开发ASIHTTPRequest数据压缩

本文为大家介绍了iOS开发ASIHTTPRequest数据压缩的内容，其中包括使用gzip来处理压缩的响应数据，怎样设置apache的mod_deflate来使用gzip压缩数据，在ASIHTTPRequest中使用gzip，相应数据的实时解压缩，使用gzip压缩request数据等等内容。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
使用gzip来处理压缩的响应数据

从0.9版本开始，ASIHTTPRequest会提示服务器它可以接收gzip压缩过的数据。许多web服务器可以在数据被发送之前压缩这些数据——这可以加快下载速度减少流量使用，但会让服务器的cpu（压缩数据）和客户端（解压数据）付出代价。总的来说，只有特定的几种数据会被压缩——许多二进制格式的文件像jpeg，gif，png，swf和pdf已经压缩过他们的数据了，所以向客户端发送这些数据时不会进行gzip压缩。文本文件例如网页和xml文件会被压缩，因为它们通常有大量的数据冗余。

怎样设置apache的mod_deflate来使用gzip压缩数据

apache 2.x以上版本已经配备了mod_deflate扩展，这使得apache可以透明地压缩特定种类的数据。要开启这个特性，你需要在apache的配置文件中启用mod_deflate。并将mod_deflate命令添加到你的虚拟主机配置或者.htaccess文件中。

在ASIHTTPRequest中使用gzip

- (IBAction)grabURL:(id)sender
{
    NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com"];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    // 默认为YES, 你可以设定它为NO来禁用gzip压缩
    [request setAllowCompressedResponse:YES];
    [request startSynchronous];
    BOOL *dataWasCompressed = [request isResponseCompressed]; // 响应是否被gzip压缩过？
    NSData *compressedResponse = [request rawResponseData]; // 压缩的数据
    NSData *uncompressedData = [request responseData]; // 解压缩后的数据
    NSString *response = [request responseString]; // 解压缩后的字符串
}
当allowCompressedResponse 设置为YES时，ASIHTTPRequest将向request中增加一个Accept-Encoding头，表示我们可以接收gzip压缩过的数据。如果响应头中包含一个Content-Encoding头指明数据是压缩过的，那么调用responseData 或者responseString 将会得到解压缩后的数据。你也可以通过调用rawResponseData来获得原始未压缩的数据。

相应数据的实时解压缩

默认情况下，ASIHTTPRequest会等到request完成时才解压缩返回的数据。若设置request的shouldWaitToInflateCompressedResponses 属性为NO，ASIHTTPRequest将会对收到的数据进行实时解压缩。 在某些情况下，这会稍稍提升速度，因为数据可以在reqeust等待网络数据时进行处理。

如果你需要对响应数据流进行流处理（例如XML和JSON解析），这个特性会很有用。如果启用了这个选项，你可以通过实现代理函数request:didReceiveData:来将返回的网络数据一点一点喂给解析器。

注意，如果shouldWaitToInflateCompressedResponses 被设置为NO，那么原始（未解压）的数据会被抛弃。具体情况请查阅ASIHTTPRequest.h的代码注释。

使用gzip压缩request数据

1.0.3版本的新特性就是gzip压缩request数据。使用这个特性，你可以通过设置shouldCompressRequestBody 为YES来使你的程序压缩POST/PUT的内容，默认值为NO。

apache的mod_deflate可以自动解压缩gzip压缩的请求体（通过合适的设置）。这个方法适用于CGI内容，但不适用于内容过滤器式的模块（例如mod PHP），这种情况下，你就必须自己解压缩数据。

ASIHTTPRequest 无法检测一个服务器是否能接收压缩过的请求体。当你确定服务器可以解压缩gzip包时，再使用这个特性。

请避免对已经压缩过的格式（例如jpeg/png/gif/pdf/swf）进行压缩，你会发现压缩后的数据比原数据更大。（梦维：因为压缩包都有头信息）


#pragma mark - 七.iOS开发ASIHTTPRequest断点续传(下载)
2013-07-22 14:02 佚名 dreamingwish 字号：T | T
一键收藏，随时查看，分享好友！
本文为大家介绍了iOS开发ASIHTTPRequest断点续传(下载)的内容，其中包括ASIHTTPRequest可以恢复中断的下载，设置一个临时下载路径，断点续传的工作原理等等内容。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
本文为大家介绍了iOS开发ASIHTTPRequest断点续传(下载)的内容，其中包括ASIHTTPRequest可以恢复中断的下载，设置一个临时下载路径，断点续传的工作原理等等内容。

从0.94版本开始，ASIHTTPRequest可以恢复中断的下载。

这个特性只对下载数据到文件中有效，你必须为一下情况的request设置allowResumeForFileDownloads 为YES：

任何你希望将来可以断点续传的下载（否则，ASIHTTPRequest会在取消或者释放内存时将临时文件删除）
任何你要进行断点续传的下载
另外，你必须自己设置一个临时下载路径（setTemporaryFileDownloadPath），这个路径是未完成的数据的路径。新的数据将会被添加到这个文件，当下载完成时，这个文件将被移动到downloadDestinationPath 。

- (IBAction)resumeInterruptedDownload:(id)sender
{
    NSURL *url = [NSURL URLWithString:
                  @"http://www.dreamingwish.com/wp-content/uploads/2011/10/asihttprequest-auth.png"];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    
    NSString *downloadPath = @"/Users/ben/Desktop/asi.png";
    
    //当request完成时，整个文件会被移动到这里
    [request setDownloadDestinationPath:downloadPath];
    
    //这个文件已经被下载了一部分
    [request setTemporaryFileDownloadPath:@"/Users/ben/Desktop/asi.png.download"];
    [request setAllowResumeForFileDownloads:YES];
    [request startSynchronous];
    
    //整个文件将会在这里
    NSString *theContent = [NSString stringWithContentsOfFile:downloadPath];
}
断点续传的工作原理是读取temporaryFileDownloadPath的文件的大小，并使用Range: bytes=x HTTP头来请求剩余的文件内容。

ASIHTTPRequest并不检测是否存在Accept-Ranges头（因为额外的HEAD头请求会消耗额外的资源），所以只有确定服务器支持断点续传下载时，再使用这个特性。


#pragma mark - 八.iOS开发ASIHTTPRequest直接读取磁盘数据流请求体
2013-07-22 14:10 佚名 dreamingwish 字号：T | T
一键收藏，随时查看，分享好友！
本文为大家介绍了iOS开发ASIHTTPRequest直接读取磁盘数据流的请求体的内容，其中包括ASIFormDataRequests，普通ASIHTTPRequest等等内容。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
本文为大家介绍了iOS开发ASIHTTPRequest直接读取磁盘数据流的请求体的内容，其中包括ASIFormDataRequests，普通ASIHTTPRequest等等内容。

从0.96版本开始，ASIHTTPRequest可以使用磁盘上的数据来作为请求体。这意味着不需要将文件完全读入内存中，这就避免的当使用大文件时的严重内存消耗。使用这个特性的方法有好几种：ASIFormDataRequests和普通ASIHTTPRequest等等，下面来具体介绍。

ASIFormDataRequests

当使用setFile:forKey:时，ASIFormDataRequests自动使用这个特性。request将会创建一个包含整个post体的临时文件。文件会一点一点写入post体。这样的request是由 CFReadStreamCreateForStreamedHTTPRequest创建的，它使用文件读取流来作为资源。

NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com/"];
ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
[request setPostValue:@"foo" forKey:@"post_var"];
[request setFile:@"/Users/ben/Desktop/bigfile.txt" forKey:@"file"];
[request startSynchronous];
普通ASIHTTPRequest

如果你明白自己的request体会很大，那么为这个request设置流式读取模式。

[request setShouldStreamPostDataFromDisk:YES];
下面的例子中，我们将一个NSData对象添加到post体。这有两个方法：从内存中添加（appendPostData:），或者从文件中添加（appendPostDataFromFile:）；

NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setShouldStreamPostDataFromDisk:YES];
[request appendPostData:myBigNSData];
[request appendPostDataFromFile:@"/Users/ben/Desktop/bigfile.txt"];
[request startSynchronous];
这个例子中，我们想直接PUT一个大文件。我们得自己设置setPostBodyFilePath ，ASIHTTPRequest将使用这个文件来作为post体。

NSURL *url = [NSURL URLWithString:@"http://www.dreamingwish.com"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setRequestMethod:@"PUT"];
[request setPostBodyFilePath:@"/Users/ben/Desktop/another-big-one.txt"];
[request setShouldStreamPostDataFromDisk:YES];
[request startSynchronous];
IMPORTANT:切勿对使用上述函数的request使用setPostBody——他们是互斥的。只有在你要自己建立request的请求体，并且还准备在内存中保持这个请求体时，才应该使用setPostBody。

#pragma mark - 九.iOS开发ASIHTTPRequest使用download cache
2013-07-22 14:15 佚名 dreamingwish 字号：T | T
一键收藏，随时查看，分享好友！
本文为大家介绍了iOS开发ASIHTTPRequest使用download cache的内容，其中包括cache策略，存储策略，其他cache相关的特性，编写自己的cache等等内容。
AD：网+线下沙龙 | 移动APP模式创新：给你一个做APP的理由>>
本文为大家介绍了iOS开发ASIHTTPRequest使用download cache的内容，其中包括cache策略，存储策略，其他cache相关的特性，编写自己的cache等等内容。

从1.8版本开始，ASIDownloadCache和ASICacheDelegate的API改变了，你可能需要修改你的代码。

尤其是，cache策略的可用选项发生了改变，你现在可以对单一request使用结合的cache策略

ASIHTTPRequest可以自动缓存下载的数据，在很多情况下这很有用。

当你离线时，你无法再次下载数据，而你又需要访问这些数据
从上次下载这些数据后，你只想在数据更新后才下载新的数据
你处理的数据永远不会发生改变，所以你只想下载一次数据
在之前版本的ASIHTTPRequest里，遇到上述情况，你只能自己处理这些策略。在一些情况下，使用download cache可以让你不用再写本地缓存机制。

ASIDownloadCache 是个简单的URL cache，可以用来缓存GET请求的相应数据。一个request要被缓存，它首先必须请求成功（没有发送错误），服务器必须返回200HTTP状态值。或者，从1.8.1版本开始，301,302,303,307重定向状态码都可以。

要打开响应值的cache机制很简单：

[ASIHTTPRequest setDefaultCache:[ASIDownloadCache sharedCache]];
这样做以后，所有的request都会自动使用cache。如果你愿意，你可以让不同的request使用共享的cache：

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setDownloadCache:[ASIDownloadCache sharedCache]];
你不会被局限于使用单一的cache，你可以想创建多少cache就创建多少cache，只要你喜欢 ^ ^。当你自己创建一个cache，你必须设定cache的路径——这路径必须是一个你拥有写权限的目录。

ASIDownloadCache *cache = [[[ASIDownloadCache alloc] init] autorelease];
[cache setStoragePath:@"/Users/ben/Documents/Cached-Downloads"];

//别忘了 - 你必须自己retaining你自己的cache!
[self setMyCache:cache];

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setDownloadCache:[self myCache]];
