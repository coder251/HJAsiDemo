//
//  HJDocumentation.h
//  HJAsiDemo
//
//  Created by huangjiong on 16/4/16.
//  Copyright © 2016年 huangjiong123. All rights reserved.
//

#ifndef HJDocumentation_h
#define HJDocumentation_h


#endif /* HJDocumentation_h */


ASIHTTPRequest使用指南---<<翻译稿>>

当第一次使用ASIHTTPRequest进行http请求时，会出现非常多的bug提示.查了一些资料，发现在少倒入了几个资源包：大概是：

CFNetwork.framework;

SystemConfiguration.framework;

MobileCoreServices.framework.



原文：http://allseeing-i.com/ASIHTTPRequest/How-to-use
Creating and running requests
Creating a synchronous request

The simplest way to use ASIHTTPRequest. Sending the startSynchronous message will execute the request in the same thread, and return control when it has completed (successfully or otherwise).

Check for problems by inspecting the error property.

To get the response as a string, call the responseString method. Don’t use this for binary data - use responseData to get an NSData object, or, for larger files, set your request to download to a file with the downloadDestinationPath property.

- (IBAction)grabURL:(id)sender
{
    NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com"];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    [request startSynchronous];
    NSError *error = [request error];
    if (!error) {
        NSString *response = [request responseString];
    }
}

In general, you should use asynchronous requests in preference to synchronous requests. When you use ASIHTTPRequest synchronously from the main thread, your application’s user interface will lock up and become unusable for the duration of the request.

译文：

创建和运行请求

创建一个同步请求

这是最简单的用法，发送 startSynchronous 消息将在相同线程中执行请求，不管是否成功，完成后返回控制。
查看error属性以检测问题。
要以字符串形式得到响应，就调用responseString方法。这个方法不适合二进制数据 － 你应该使用responseData 得到NSData对象，或者如果有更大的文件，你可以设置downloadDestinationPath将请求下载到文件。


- (IBAction)grabURL:(id)sender
{
    NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com"];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    [request startSynchronous];
    NSError *error = [request error];
    if (!error) {
        NSString *response = [request responseString];
    }
}

注意：一般的，你应该优先使用异步请求，如果你在主线程中使用ASIHTTPRequest的同步方法，程序的ui在请求过程中将被锁定而无法响应。





创建一个异步请求

下面的代码做同样的事情，但请求运行于后台。

- (IBAction)grabURLInBackground:(id)sender
{
    NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com"];
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

注意我们设置请求的委托，这样就能在请求完成或失败时得到通知。
这是创建异步请求最简单的方式，他会运行于场景后面的一个全局的NSOperationQueue中，对于更复杂的操作，例如在多个请求中追踪进展，你可能想要创建自己的队列，下面我们谈谈这个。







使用程序块（blocks）
对于v1.8，我们可以在支持程序块的平台使用他们：

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

注意：在我们声明请求时，用到了__block限定语，这很重要！它告诉block不要保留请求，来防止一个保留循环(retain-cycle)，因为请求总会保留block.





使用队列

这个例子做同样的事，不同的是，我们为自己的请求创建了NSOperationQueue。

使用一个NSOperationQueue或者ASINetworkQueue，你能更有效的控制异步请求。我们使用一个请求，只有一定数量的请求能同时运行。如果你添加多于maxConcurrentOperationCount数量的请求，这些请求将等到其他请求完成后才会开始。

- (IBAction)grabURLInTheBackground:(id)sender
{
    if (![self queue]) {
        [self setQueue:[[[NSOperationQueue alloc] init] autorelease]];
    }
    
    NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com"];
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


在上面的例子中，'queue'是控制器的一个保留的NSOperationQueue属性。

我们设置自定义的selector处理成功或失败的回调。如果你不设置这些，默认的requestFinished和requestFailed将被使用，就像在之前的例子中一样。

在委托方法中处理对歌请求的成功和失败

如果你需要处理许多不同类型的请求，你有几个选择：

1. 如果你的请求都属于一个大的范围，你又想区别他们，可以设置请求的userInfo字典属性，填入你的自定义数据，你就可以在委托方法中读取了。在更简单的用例中，你可以简单的设置tag属性。这些属性都是给你自己用的，不会发送到服务器。

2. 如果你处理完全不同的请求，为每个请求设置不同的 setDidFinishSelector / setDidFailSelector。

3. 对更复杂的情况，或者你想在后台解析响应数据，可以为每个请求类型创建一个ASIHTTPRequest的子类，然后重载requestFinished: 和 failWithError: 。

注意：最好避免在委托方法中，用url来区分不同的请求，因为url属性在重定向时会改变。如果你真的真的想用的话，请用[request originalURL] 代替，这个将总是记录第请求连接时的首个url。





关于ASINetworkQueues

ASINetworkQueues是NSOperationQueue，它提供了额外的功能。
它的主要目的是追踪整个队列上传或下载的进度。
ASINetworkQueues提供了一些额外的委托方法selector。
requestDidStartSelector
每当队列中的一个请求开始运行时调用。你可以使用这个作为didStartSelector的替代方法，为你加到队列的请求设置一个委托。

requestDidReceiveResponseHeadersSelector
每当队列中的一个请求从服务器得到响应头时调用。对于大的下载，这个 selector有时在请求实际完成前执行。
你可以使用它作为对didReceiveResponseHeadersSelector的替代。

requestDidFinishSelector
每当队列中的一个请求完成时调用。可以使用它作为对didFinishSelector的替代。

requestDidFailSelector
每当队列中的请求失败时调用，可作为didFailSelector的替代。

queueDidFinishSelector
当队列完成时调用，不管单个请求成功或失败。

使用以上selector，要将队列的委托，而不是请求的委托，设置到实现这些selector所代表的方法的控制器。

ASINetworkQueues工作起来和NSOperationQueues稍有不同，加到其中的请求不会立刻运行，当使用ASINetworkQueue时，添加你想运行的所有请求，然后调用[queue go]。当你启动一个队列，将精确进度(accurate progress)打开时，他会首先为队列中所有的get请求执行一个head请求，来得到将要下载数据的总体尺寸。获得此数据后，它就能准确的显示总体进度，然后真实的请求才会开始。
问题：当你向一个运行中的ASINetworkQueue加入一个请求时发生了什么事情？
回答：如果你使用ASINetworkQueue来追踪几个请求的总体进度，整体进度只会在哪个请求开始执行时，才会将它计算在内。ASINetworkQueue不会在运行中，当加入请求后执行head请求，所以如果你立刻向运行的队列加入许多请求，总体进度不会立刻更新。
如果队列已经运行，你不需要再次调用[queue go]。

当ASINetworkQueue中的一个请求失败，队列默认的将取消所有其他的请求。你可以调用[queue setShouldCancelAllRequestsOnFailure:NO].来关闭这一行为。
ASINetworkQueues只能执行ASIHTTPRequest操作，而不能用于一般操作，尝试添加一个非ASIHTTPRequest的NSOperation将产生一个异常。
提示：这里有一个创建和使用ASINetworkQueue的完整例子：
http://gist.github.com/150447


补充内容 (2011-9-27 16:42):
未完待续。。。





取消一个异步请求
为了取消一个异步请求，可以调用[request cancel]，不管该请求是用[request startAsynchronous] 启动的，还是在你创建的队列中运行。注意你不能取消一个同步请求。
注意，当你取消一个请求，请求会将之视为一个错误，然后会调用你的委托或者队列的失败委托方法。如果你不想这种事发生，在取消之前将委托设为nil，或者使用clearDelegatesAndCancel作为代替。

// Cancels an asynchronous request
[request cancel]

// Cancels an asynchronous request, clearing all delegates and blocks first
[request clearDelegatesAndCancel];

当使用一个ASINetworkQueue时，你取消其中一个请求，所有其他的请求也会被取消，除非队列的shouldCancelAllRequestsOnFailure为no, 默认为yes。

// When a request in this queue fails or is cancelled, other requests will continue to run
[queue setShouldCancelAllRequestsOnFailure:NO];

// Cancel all requests in a queue
[queue cancelAllOperations];





安全处理委托在请求完成前释放的情况

请求不会保留他们的委托，所以如果你的委托有机会在请求运行时释放的话，你能及时清理请求的委托属性是至关重要的。在大多数情况下，如果你的委托将要释放，你大概也想取消请求，因为你不再关心请求的状态。

在下面的例子中，控制器有一个保存于保留实例变量中的ASIHTTPRequest对象。我们在其dealloc实现中调用clearDelegatesAndCancel方法，在我们释放请求的引用之前：

// Ddealloc method for our controller
- (void)dealloc
{
    [request clearDelegatesAndCancel];
    [request release];
    ...
    [super dealloc];
}







put方法和自定义post
如果你想通过put发送数据，或者想要发送post，但是想自己创建post数据体(body)，使用appendPostData: 或者 appendPostDataFromFile:。

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request appendPostData:[@"This is my data" dataUsingEncoding:NSUTF8StringEncoding]];
// Default becomes POST when you use appendPostData: / appendPostDataFromFile: / setPostBody:
[request setRequestMethod:@"PUT"];

如果你想发送大量的数据，并且不用ASIFormDataRequest，参看后面的‘从磁盘以流式post数据’小节。





下载数据

将响应数据直接下载为文件

如果你请求的数据相当大，你可以直接将下载内存直接保存为文件。这样，ASIHTTPRequest不需要一次在内存中保存整个请求。

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setDownloadDestinationPath:@"/Users/ben/Desktop/my_file.txt"];

当使用downloadDestinationPath下载到文件时，下载过程中，数据将被保存在一个临时文件中。这个文件路径保存于temporaryFileDownloadPath。当请求成功完成，下面的事情之一会发生：

如果数据时gzip压缩过的，压缩文件将被解压到downloadDestinationPath，并且临时文件被删除；
如果数据没被压缩，临时文件移动到downloadDestinationPath，覆盖掉任何之前的文件。

注意：如果响应body是空的，文件是不会被创建的。所以如果请求可能返回一个空的body，你要确定在尝试操作文件之前，检查它是否存在。





处理收到的响应数据

如果你需要处理收到的响应数据，比如你想使用流解析器(streaming parser)来解析仍在下载的响应数据，就需要在委托中实现request:didReceiveData:方法。注意当你这么做时，ASIHTTPRequest不会产生responseData，也不会将数据写入downloadDestinationPath － 你必须自己搞定保存响应数据的事。

读取HTTP状态码

ASIHTTPRequest 不会鸟大多数的http状态码，重定向和验证的状态码除外。所以，就要靠你检查问题，比如404错误，然后确定你做了恰当的处理。

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request startSynchronous];
int statusCode = [request responseStatusCode];
NSString *statusMessage = [request responseStatusMessage];


读取响应头

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request startSynchronous];
NSString *poweredBy = [[request responseHeaders] objectForKey:@"X-Powered-By"];
NSString *contentType = [[request responseHeaders] objectForKey:@"Content-Type"];


处理文本编码
ASIHTTPRequest会尝试从Content-Type头得到数据的编码。如果它发现了一个编码类型，他会社子responseEncoding为相应的NSStringEncoding。如果它没在头部发现编码信息，就会用defaultResponseEncoding，这个值默认为NSISOLatin1StringEncoding。
当你调用[request responseString]，ASIHTTPRequest将尝试用responseEncoding作为源编码，从收到的数据创建一个字符串。

处理重定向
当ASIHTTPRequest碰到以下http状态码时，会自动重定向到一个新的url，假设Location头已经发送：
301 永久移动
302 发现
303 参看其他
当重定向发生，响应数据(responseHeaders / responseCookies / responseData / responseString 等) 的值将反映从最终位置收到的内容。
在重定向周期中碰到的任何url上设置的Cookie，都倍存储到全局cookie仓库，在恰当的时候会由重定向请求呈现给服务器。
你可以关闭自动重定向，设置shouldRedirect属性为no即可。

注意：默认情况下，自动重定向总是以get方式进行请求(没有body)。这个行为复合大多数的浏览器，除了象301和302这样应该用原有方式重定向的规范。
为了保留301和302的原有方式(包含请求的body)，需要将shouldUseRFC2616RedirectBehaviour设置为yes，在你开始请求之前。

补充内容 (2011-9-27 19:50):
文档真的很长啊。。。才翻译了1/4，今天歇菜，明天继续。。。





跟踪进度

每个ASIHTTPRequest都有两个用于跟踪进度的委托 － downloadProgressDelegate用于下载，uploadProgressDelegate用于上传。
进度委托可以是NSProgressIndicators(Mac OS X)或者UIProgressViews(iPhone)。ASIHTTPRequest会自动适应这两者行为上的差异。你也能够使用自定义的类作为进度委托，只要它响应setProgress:方法。

如果你正执行单个请求，在这个请求上设置一个上传或下载委托。
如果你在队列中执行多个请求，并且想跟踪队列中所有请求的总体进度，可以使用ASINetworkQueue并设置队列的进度委托
如果想同时做上面两件事，也是可行的。

重点注意：如果你正在向需要验证的站点进行上传操作，为了提供有效验证，每当上传失败时，进度会被重设为之前的值。为此，如果你正同验证web服务器通信，建议你仅当useSessionPersistence为yes时使用上传进度委托，并确保你在尝试跟踪大量上传数据前，在另一个请求中验证。

当请求body小于128kb时，目前是不能够跟踪上传进度的。对请求大于128kb的请求，进度委托不会收到第一个128kb的post数据进度的信息。这是因为CFNetwork API的限制造成的。
2009.6.21日更新：apple的好家伙非常友好的定位了我的bug报告！在iphone 2.0 sdk中，貌似这个缓冲尺寸减小到了32kb，这让精确的上传进度跟踪更加可靠了。





跟踪单个请求的下载进度

在这个例子中，myProgressIndicator是一个NSProgressIndicator。

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setDownloadProgressDelegate:myProgressIndicator];
[request startSynchronous];
NSLog(@"Max: %f, Value: %f", [myProgressIndicator maxValue],[myProgressIndicator doubleValue]);

跟踪一组请求的下载进度

这个例子中，myProgressIndicator 是一个UIProgressView，myQueue是一个ASINetworkQueue。
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

注意对于ASINetworkQueues，我们必须调用[myQueue go]来启动队列。

跟踪单个请求的上传进度

本例中，myProgressIndicator是一个UIProgressView。
ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
[request setPostValue:@"Ben" forKey:@"first_name"];
[request setPostValue:@"Copsey" forKey:@"last_name"];
[request setUploadProgressDelegate:myProgressIndicator];
[request startSynchronous];
NSLog(@"Value: %f",[myProgressIndicator progress]);

跟踪一组请求的上传进度

本例中，myProgressIndicator是一个NSProgressUbdicator，myQueue是一个ASINetworkQueue。
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





精确进度vs简单进度

ASIHTTPRequest提供了两种显示进度的途径，简单进度 和 精确进度。他们由ASIHTTPRequests 和 ASINetworkQueues的showAccurateProgress来控制。如果你在一个请求上设置showAccurateProgress，只会影响这个请求。如果你设置了队列，将影响到队列的全部请求。

简单请求
当你使用简单进度，进度仅在请求完成时更新。对于单个进程，你只能得到0％和100％完成。对于一个包括4个请求的队列，你能得到5次进度更新，0％，25％，50％，75％和100％，每一次增量表明有一个请求完成了。
简单进度（showAccurateProgress ＝ NO）是ASINetworkQueues的默认值，它很好的适用于包含大量的轻量级上传/下载请求的队列。

精确进度
使用精确进度时，进度以收发的字节来更新，所以极适合收发大量数据的请求，它会更好的指示一个耗时请求收发了多少数据。
使用精确进度会稍稍降低上传操作的性能，因为进度委托（可能是UIProgressView或者NSProgressIndicator）将更为频繁的重绘。
使用精确进度会对使用队列的下载任务影响更大，因为队列在下载前，会先为其中的get请求执行head请求，来决定将要下载的数据总体尺寸。强烈推荐你在队列中下载大文件时使用精确进度，但是应该避免队列中包含大量小型下载时使用它。
精确进度（showAccurateProgress ＝ YES）是ASIHTTPRequests执行同步任务时的默认值。





自定义进度跟踪

ASIProgressDelegate协议定义了得到请求的更新进度的委托的所有方法。大多数情况下，为NSProgressIndicator或UIProgressView设置uploadProgressDelegate 或 downloadProgressDelegate就足够了。但是，如果你想做更复杂的进度跟踪，你的进度委托应该优先于setProgress: (iOS) 或者setDoubleValue: / setMaxValue: (Mac)实现下列方法。这些方法允许你得到收发的实际字节数，而不是更简单的方法得到的介于0和1之间的数字。
downloadProgressDelegates的方法

request:didReceiveBytes:每当请求下载更多数据时在downloadProgressDelegate上调用。注意这有别于一般会实现的request:didReceiveData:委托。
request:incrementDownloadSizeBy:下载尺寸改变时被调用，传入的参数是下载尺寸的增量。一般发生在请求收到响应头并获得了下载的尺寸时。

uploadProgressDelegates的方法

request:didSendBytes: 每当请求能发送一些数据时在uploadProgressDelegate上调用。重点提醒：这个方法能被小于0的数字调用，当一个请求需要删除上传进度时（一般是当它已经上传了数据，但是验证失败或者由于某种原因需要再次运行时）。
request:incrementUploadSizeBy: 上传尺寸改变时调用。传入的尺寸经常会小于0，由于请求调整了上传尺寸，它将os内部的缓冲尺寸也计算在内了。



处理http验证

如果你正在连接到需要验证的服务器，你大概想要看看这个流程图
http://allseeing-i.com/ASIHTTPRe ... ticationProcess.pdf
，演示了ASIHTTPRequest如何找到和应用请求验证。

在url中指定用户名和密码
NSURL *url = [NSURL URLWithString:@"http://username:password@allseeing-i.com/top_secret/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];

设置请求的用户名和密码
NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com/top_secret/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setUsername:@"username"];
[request setPassword:@"password"];





在键链(keychain)中储存证书
如果你打开了keychainPersistence，任何你提供的有效用户名和密码将被存储到键链中。随后的请求会重用键链中的用户名和密码，即使你退出再重启应用。
NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com/top_secret/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setUseKeychainPersistence:YES];
[request setUsername:@"username"];
[request setPassword:@"password"];

如果你使用键链同时希望自己管理它，你应该能在ASIHTTPRequest.h中找到有助于此与键链相关的类方法。





在会话中保存证书

如果useSessionPersistence打开了，默认是打开的，ASIHTTPRequest将证书保存到内存，并可以在后续请求中重用他们。
NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com/top_secret/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setUsername:@"username"];
[request setPassword:@"password"];
[request setUseSessionPersistence:YES]; //Shouldn't be needed as this is the default

//Should reuse our username and password
request = [ASIHTTPRequest requestWithURL:url];



NTML验证
要通过使用ntml方案的windows服务器进行验证，你还需要指定你验证的域(domain)。
NSURL *url = [NSURL URLWithString:@"http://my.windows.server/top_secret/"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setUsername:@"username"];
[request setPassword:@"password"];
[request setDomain:@"my-domain"];

使用委托来提供证书
相对于提前指定验证证书，你可能更愿意在请求不能从会话验证缓存或键链中得到证书时，向它的委托询问。这可能有助于你希望连到一个服务器，但你不确定它需要哪种类型的验证。
确定你的委托实现了authenticationNeededForRequest，ASIHTTPRequest会暂停一个请求，当它等待一个委托以获得将要使用的证书时。当你有你需要的证书时，只要在请求上设置他们，然后调用[request retryUsingSuppliedCredentials]，如果你想取消，就要调用[request cancelAuthentication]，这也会取消请求。

对于v1.0.8，请求的委托一次只会收到一个authenticationNeededForRequest或者proxyAuthenticationNeededForRequest。当委托处理第一个请求时，其他需要验证的请求会暂停执行。如果证书通过验证，任何当前进程的其他请求都会尝试重用他们，假设他们对url有效。如果委托取消了验证，且队列的shouldCancelAllRequestsOnFailure为yes，所有其他的请求不再尝试查询证书而会取消。

在使用同步请求时，你不能使用代理模式进行验证。
在老版本中这会导致应用挂起，对于v1.0.8代理方法不再被调用。





使用内置验证对话框(目前仅ios可用)

在v1.0.8中新增了ASIAuthenticationDialog类。它主要用来和验证代理一同工作，但是它可以被用来为验证web服务器询问用户的证书。
为了最好的用户体验，多数连到单个服务的应用应该在其委托中实现authenticationNeededForRequest:，或者避免完全的使用委托样式(delegate-style)验证。但是有时为常规验证而使用ASIHTTPRequest的标准验证对话框有些好处：

你不想创建你自己的登录表单
你大概需要从外部来源获得数据，而不太确定它需不需要验证。

为此，在请求中设置shouldPresentAuthenticationDialog为true，如果你的委托没有实现authenticationNeededForRequest，用户将看到此对话框。

验证对话框不会在同步请求时出现。

对话框有点模仿了iphone上的safari，并包括：
一条消息指明这个认证是为web服务器做的(而不是代理)
你连接到的服务器的主机名或ip
验证域(authentication realm)，如果支持的话
输入用户名和密码的文本框
当连接到一个使用ntlm方案的服务器时，对话框也包括一个输入域(domain)的文本框
关于验证是否以明文(plain text)发送的提示(仅在用无ssl的基本验证时才被明文发送)。

如果你想改变对话框的外观，子类化ASIHTTPRequest，重载showAuthenticationDialog来显示你自己的自定义对话框或者子类化ASIAuthenticationDialog。





在服务器要求之前提供证书

非常重要：在v1.0.8中，使用基本验证的请求时，这个特征改变了，你可能需要更新你的代码。

ASIHTTPRequest可以在首次请求时就为服务器提供证书，而不是等到服务器要求证书。结果是使用验证时获得更好的性能，因为它避免了额外的请求。

使用基本验证来触发此行为时，你应该手动设置请求的authenticationScheme：
[request setAuthenticationScheme:(NSString *)kCFHTTPAuthenticationSchemeBasic];

使用其他验证方案时，证书可以在服务器要求之前提供，但只有在另一个请求成功通过此服务器验证之后。

你可能希望禁用此特征，如果：
你的应用可能使用多组证书来同时和同一个服务器对话。
你的应用是安全至上的，使用这个特性本身不太安全，因为在你有机会验证你连接到你希望连接的服务器之前，证书已经被发送了。

为了禁用此特征，使用以下代码：
[request setShouldPresentCredentialsBeforeChallenge:NO];



Cookies

持久化cookies

ASIHTTPRequest允许你使用全局存储，全局存储为所有max os上使用CFNetwork或NSURLRequest API的应用所共享。如果useCookiePersistence为on（默认如此），cookies将被保存到共享的NSHTTPCookieStorage容器中，并自动被其他请求自动重用。如果cookie仅对特定请求有效，ASIHTTPRequest提供其他程序创建的cookie是没有价值的。
你可以象这样清除在会话中创建的所有cookie：
[ASIHTTPRequest setSessionCookies:nil];

这里，'会话cookie'指的一个会话中创建的所有cookie，而不是程序退出时被删除的无过期时间的cookie(经常被称为会话cookie)。

另外，类的方便(convenience)方法clearSession将清除所有会话中创建的cookie，以及任何缓存的验证数据。


手工处理cookie
如果你喜欢，你可以关掉useCookiePersistence，然后手动管理特定请求的一组cookie。

//Create a cookie
NSDictionary *properties = [[[NSMutableDictionary alloc] init] autorelease];
[properties setValue:[@"Test Value" encodedCookieValue] forKey:NSHTTPCookieValue];
[properties setValue:@"ASIHTTPRequestTestCookie" forKey:NSHTTPCookieName];
[properties setValue:@".allseeing-i.com" forKey:NSHTTPCookieDomain];
[properties setValue:[NSDate dateWithTimeIntervalSinceNow:60*60] forKey:NSHTTPCookieExpires];
[properties setValue:@"/asi-http-request/tests" forKey:NSHTTPCookiePath];
NSHTTPCookie *cookie = [[[NSHTTPCookie alloc] initWithProperties:properties] autorelease];

//This url will return the value of the 'ASIHTTPRequestTestCookie' cookie
url = [NSURL URLWithString:@"http://allseeing-i.com/ASIHTTPRequest/tests/read_cookie"];
request = [ASIHTTPRequest requestWithURL:url];
[request setUseCookiePersistence:NO];
[request setRequestCookies:[NSMutableArray arrayWithObject:cookie]];
[request startSynchronous];

//Should be: I have 'Test Value' as the value of 'ASIHTTPRequestTestCookie'
NSLog(@"%@",[request responseString]);





处理压缩的响应，以及压缩请求body

使用gzip来处理压缩的响应数据

对于v0.9，ASIHTTPRequest不会提醒服务器它可以接受gzip压缩格式数据。如果你在现有项目中升级ASIHTTPRequest，参看设置指令地址
http://allseeing-i.com/ASIHTTPRequest/Setup-instructions
了解如何链接到zlib。

许多web服务器可以在发送数据前进行压缩 － 这样可以加快下载和降低使用带宽，代价是服务器(用以压缩数据)和客户端(解压数据)增加了额外的cpu时间。一般说来，只有某些类型的数据会被压缩 - 许多二进制格式例如jpeg,gif,png,swf和pdf已经压缩过他们的数据，所以gzip压缩不会用于发送他们到客户端。文本文件如网页和xml文档是gzip压缩的完美候选人，因为他们经常包含大量重复信息。

如何设置apache使用mod_deflate来压缩数据
apatche 2.x 之后带来了mod_deflate扩展允许透明的压缩某些类型的数据。要打开他，你需要在apatche配置文件中启用mode_deflate，并添加mod_deflate指令到你的虚拟主机配置中，或者到你的.htaccess文件中。

在ASIHTTPRequest中使用gzip

- (IBAction)grabURL:(id)sender
{
    NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com"];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    // YES is the default, you can turn off gzip compression by setting this to NO
    [request setAllowCompressedResponse:YES];
    [request startSynchronous];
    BOOL *dataWasCompressed = [request isResponseCompressed]; // Was the response gzip compressed?
    NSData *compressedResponse = [request rawResponseData]; // Compressed data
    NSData *uncompressedData = [request responseData]; // Uncompressed data
    NSString *response = [request responseString]; // Uncompressed data as a string
}


当allowCompressedResponse为真，ASIHTTPRequest将添加Accept-Encoding头到请求中，表明我们可以接受gzip压缩的响应数据。如果响应头包含一个content-encoding头指明了数据已被压缩，调用responseData 或者 responseString将在返回前解压数据。你可以通过调用rawResponseData得到原始压缩数据。

联机解压gzip响应包
默认的，ASIHTTPRequest会一直等到请求完成对gzip响应包的解压缩。设置shouldWaitToInflateCompressedResponses属性为no，你可以告诉ASIHTTPRequest在收到数据同时进行解压。有时候，这会提升一点速度，因为当一个请求等待更多响应时，数据可以同时被处理。
这个特性对于需要使用流解析器(streaming parser)如xml或json解析器时尤其有用，打开这个选项，你可以直接给你的解析器输送解压过的数据，他们来自你委托实现的request:didReceiveData: 方法。

注意当shouldWaitToInflateCompressedResponses为no时，原始(压缩过的)数据将被丢弃，参看ASIHTTPRequest.h的注释获得更多信息。


使用gzip来压缩请求body
v1.0.3新增了请求body的gzip压缩。使用这个特性，你的应用可以压缩post/put操作的内容，只要设置shouldCompressRequestBody为yes即可。shouldCompressRequestBody默认为no。

当你配置了SetInputFilter DEFLATE 后，apatche的mod_deflate可以自动的解压gzip请求体。这个方法为cgi内容工作，但是如果你使用apatche模块构建成了一个RESOURCE过滤器(如mod PHP)则不能工作，这时你需要自己搞定数据解压。

注意：ASIHTTPRequest不能检查到服务器是否接受gzip请求体。只有当你确定服务器能搞定gzip body时使用这项特性。
避免对压缩格式文件如jpeg/png/gif/pdf/swf使用gzip，你会发现gzip版本会比原始文件大。



恢复被打断的下载
自v0.94开始，ASIHTTPRequest能够恢复不完整的下载
- (IBAction)resumeInterruptedDownload:(id)sender
{
    NSURL *url = [NSURL URLWithString:
                  @"http://allseeing-i.com/ASIHTTPRequest/Tests/the_great_american_novel.txt"];
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    
    NSString *downloadPath = @"/Users/ben/Desktop/my_work_in_progress.txt";
    
    // The full file will be moved here if and when the request completes successfully
    [request setDownloadDestinationPath:downloadPath];
    
    // This file has part of the download in it already
    [request setTemporaryFileDownloadPath:@"/Users/ben/Desktop/my_work_in_progress.txt.download"];
    [request setAllowResumeForFileDownloads:YES];
    [request startSynchronous];
    
    //The whole file should be here now.
    NSString *theContent = [NSString stringWithContentsOfFile:downloadPath];
}


这只在下载数据到文件时有用，你必须设置allowResumeForFileDownloads为yes，为了：
任何你将来可能想恢复的下载(或者ASIHTTPRequest会在取消或释放时删除的临时下载)
任何你想恢复的下载

而且，你必须设置一个临时下载路径(setTemporaryFileDownloadPath)，用不完整数据的路径，新数据将被追加到这个文件，当下载成功，这个文件会被移到downloadDestinationPath。

ASIHTTPRequest不会检查accept-range头（因为额外head请求的负担），所以仅在确定服务器能够支持不完整下载时才使用此特征。

补充内容 (2011-9-28 18:30):
stopped here tody...



直接从磁盘流式请求body

自v0.96开始，ASIHTTPRequest能够使用磁盘文件作为请求body。这意味着将请求body保持在内存不再必要，这样一来，大型的post/put操作的将极大的减少内存使用量。

你可以以几种方法来使用这一特征：

ASIFormDataRequests

NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com/ignore"];
ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
[request setPostValue:@"foo" forKey:@"post_var"];
[request setFile:@"/Users/ben/Desktop/bigfile.txt" forKey:@"file"];
[request startSynchronous];

ASIFormDataRequests在你使用setFile:forKey:，自动使用这一特性。请求将创建临时文件，包含完整的请求body。文件一次写入一些body的相关部分。请求被CFReadStreamCreateForStreamedHTTPRequest创建，使用此文件上的读取流作为来源。

常规的ASIHTTPRequest
如果你知道你的请求将会很大，可以在请求上打开磁盘流（streaming from disk）：
[request setShouldStreamPostDataFromDisk:YES];

在下面的例子中，我们每次添加一个NSData对象。有两个方法做这个事 － 从内存添加数据(appendPostData:)，或者使用appendPostDataFromFile从文件添加内容。

NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com/ignore"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setShouldStreamPostDataFromDisk:YES];
[request appendPostData:myBigNSData];
[request appendPostDataFromFile:@"/Users/ben/Desktop/bigfile.txt"];
[request startSynchronous];


在本例中，我们想要直接put一个大文件。我们自己设置setPostBodyFilePath，ASIHTTPRequest将使用这个文件作为post body。
NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com/ignore"];
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setRequestMethod:@"PUT"];
[request setPostBodyFilePath:@"/Users/ben/Desktop/another-big-one.txt"];
[request setShouldStreamPostDataFromDisk:YES];
[request startSynchronous];

非常重要：你不应该在上面任何方法的同一个请求中使用setPostBody － 他们是互斥的，setPostBody只能用于你自己构建请求body，并计划将请求body保留到内存的情型。





使用下载缓存

ASIDownloadCache 以及 ASICacheDelegate 的API在v1.8中已经改变，如果从v1.7升级的话，你需要更新你的代码。
特别是，缓存策略的可用选项是不同的，并且你现在可以将多个缓存策略合并到单个请求中。

ASIHTTPRequest可以自动保存下载数据到一个缓存，以便以后使用。许多情况下这会很有帮助：
你想要访问数据，在没有因特网连接不能重新下载时；
你想下载些东西，仅在你上次下载后它有了变化时；
你用的内容永不改变，所以你只想下载它一次；

在之前的ASIHTTPRequest版本中，处理以上情况意味着你自己手动保存这些数据。使用一个下载缓存可以在一些情况下减少你自己编写本地存储机制的需求。

ASIDownloadCache是一个简单的url缓存，可以被用于缓存get请求的响应。为了符合响应缓存的条件，请求必须成功(没有错误)，服务器必须返回一个200 ok的http响应码，或者从v1.8.1开始，支持301,302,303,307重定向的状态码。

开启响应缓存很简单：
[ASIHTTPRequest setDefaultCache:[ASIDownloadCache sharedCache]];

开启之后，所有的请求会自动使用缓存。如果你喜欢，你可以为独立的请求设置共享缓存：
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setDownloadCache:[ASIDownloadCache sharedCache]];

缓存不限于单个，你可以创建任意多个缓存。当你自己创建缓存时，必须设置缓存的存储路径 －这应该是一个可写的文件夹：

ASIDownloadCache *cache = [[[ASIDownloadCache alloc] init] autorelease];
[cache setStoragePath:@"/Users/ben/Documents/Cached-Downloads"];

// Don't forget - you are responsible for retaining your cache!
[self setMyCache:cache];

ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setDownloadCache:[self myCache]];



关于缓存策略
缓存策略是在信息存储于缓存中时你主要的控制方法，以及何时优先使用缓存的数据，而不是重新下载数据。

独立请求的缓存策略可以使用它的cachePolicy属性。缓存策略使用位掩码(bitmask)定义，所以你可以组合多个选项来创建想要的策略：
// Always ask the server if there is new content available,
// If the request fails, use data from the cache even if it should have expired.
[request setCachePolicy:ASIAskServerIfModifiedCachePolicy|ASIFallbackToCacheIfLoadFailsCachePolicy];

你可以使用下列选项来定义一个请求的缓存策略：

ASIUseDefaultCachePolicy
默认缓存策略，当你对请求应用此策略时，它会使用缓存的defaultCachePolicy，ASIDownloadCache的默认缓存策略是ASIAskServerIfModifiedWhenStaleCachePolicy，你不应该将此策略和其他策略组合使用。

ASIDoNotReadFromCacheCachePolicy
请求不会从缓存读取数据

ASIDoNotWriteToCacheCachePolicy
请求不会保存到缓存

ASIAskServerIfModifiedWhenStaleCachePolicy
这是ASIDownloadCaches的默认缓存策略。使用了它，请求会首先查看缓存中是否有可用的缓存响应数据。如果没有，请求会照常进行。
如果有没有过期的缓存数据，请求会使用它而不去访问服务器。如果缓存数据过期了，请求将执行一个有条件的get去获取是否有可用的升级版本。如果服务器表示缓存的数据就是最新的，那么缓存的数据将被使用，新的数据不会被下载。这时，缓存的过期时间(expiry date)将根据服务器新的过期时间而被更新。如果服务器提供了更新内容，则将被下载，新的数据和过期时间将被写入缓存。

ASIAskServerIfModifiedCachePolicy
这个策略和ASIAskServerIfModifiedWhenStaleCachePolicy相同，只是请求每次都会询问服务器是否有新的数据

ASIOnlyLoadIfNotCachedCachePolicy
只要存在缓存数据，总是会被使用，即使它已经过期。

ASIDontLoadCachePolicy
请求仅在响应已经被缓存时成功。如果请求没有任何响应被缓存，请求会终止，并且也不会为此请求设置错误。

ASIFallbackToCacheIfLoadFailsCachePolicy
如果请求失败，将会退回到缓存数据。如果失败后使用了缓存的数据，请求将会成功而不报错。你一般会将它和其他策略混合使用，因为此策略仅在发生问题时用于指定行为。


当你为缓存设置了defaultCachePolicy属性，所有的请求将会使用这个缓存策略，除非他们自己设置了自定义的策略。





关于存储策略
存储策略允许你定义特定响应的缓存将被保存多久，ASIHTTPRequest目前支持两种存储策略：
ASICacheForSessionDurationCacheStoragePolicy 是默认值。响应仅在会话期间被保存，并将在缓存首次使用后被删除，或者当调用[ASIHTTPRequest clearSession] 时被删除。
使用ASICachePermanentlyCacheStoragePolicy，缓存数据将被永久保存，要用这个存储策略，可将它设置到一个请求上：
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setCacheStoragePolicy:ASICachePermanentlyCacheStoragePolicy];

要手动清除缓存，调用clearCachedResponsesForStoragePolicy，传入你希望清除的缓存数据的存储类型：
[[ASIDownloadCache sharedCache] clearCachedResponsesForStoragePolicy:ASICachePermanentlyCacheStoragePolicy];





其他缓存特性
// 当你关掉shouldRespectCacheControlHeaders, 响应数据将被保存，即使服务器明确要求不要缓存他们
// (eg with a cache-control or pragma: no-cache header)
[[ASIDownloadCache sharedCache] setShouldRespectCacheControlHeaders:NO];

为请求设置secondsToCache，覆盖了由服务器设置的任何过期时间，一直保存响应数据直到secondsToCache秒到期。
ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
[request setSecondsToCache:60*60*24*30]; // Cache for 30 days


在请求运行后，didUseCachedResponse将在响应由缓存返回时，返回yes
[request didUseCachedResponse];


询问缓存请求数据的保存路径，这是使用下载缓存最高效的方式，因为数据不必在请求完成后被拷贝到缓存
[request setDownloadDestinationPath:
 [[ASIDownloadCache sharedCache] pathToStoreCachedResponseDataForRequest:request]];


写你自己的缓存

如果你已经有一个下载缓存，并想将它接入到ASIHTTPRequest，或者你想要写自己的缓存，可以让你的缓存实现ASICacheDelegate协议。






调试选项

ASIHTTPRequest提供了一些有助于调试请求行为的标志。这些标志可以在ASITHHPRequestConfig.h中找到。
当你打开这些标志后，请求会把他们干的事打印到控制台。

DEBUG_REQUEST_STATUS
打印总体请求的生命周期信息 -- 开始，结束上传，结束下载 等等。

DEBUG_THROTTLING
打印（粗略的）有多少带宽被使用的信息，如果请求被限流，还包括这如何发生的信息。同DEBUG_REQUEST_STATUS联合使用，可能对调试超时有帮助，因为你可以看到在哪一点请求停止了收发数据。

DEBUG_PERSISTENT_CONNECTIONS
打印请求如何重用持续连接的信息，如果你看到输出下面的信息：
Request attempted to use connection #1, but it has been closed - will retry with a new connection

…这表示你设置到persistentConnectionTimeoutSeconds的值可能过高，参看“配置持续连接”小节获得更多信息。

DEBUG_HTTP_AUTHENTICATION
从v1.8.1后添加，这会打印出关于请求如何处理http验证(基本/摘要/ntml)的信息。

DEBUG_FORM_DATA_REQUEST
打印ASIFormDataRequest将发送的请求body的概要。仅在使用ASIFormDataRequest有用。

（全文完） 
====================================================================
© Ben Copsey, All-Seeing Interactive 2008-2011.
