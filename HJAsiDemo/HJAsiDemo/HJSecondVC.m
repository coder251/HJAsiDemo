//
//  HJSecondVC.m
//  HJAsiDemo
//
//  Created by huangjiong on 16/4/15.
//  Copyright © 2016年 huangjiong123. All rights reserved.
//

#import "HJSecondVC.h"
#import "ASIHTTPRequest.h"
#import "ASINetworkQueue.h"

@interface HJSecondVC ()<ASIHTTPRequestDelegate>
@property(nonatomic,strong)ASINetworkQueue *queue;

@end

@implementation HJSecondVC
{
  
}


- (void)viewDidLoad {
    [super viewDidLoad];
    
    //队列
    self.queue = [[ASINetworkQueue alloc] init];
    //url
    NSURL *url = [NSURL URLWithString:@""];
    //请求对象
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    //设置代理
    request.delegate = self;
    //自定义请求完成及失败时的 代理回调方法,
    request.didFinishSelector = @selector(requestDidFinish:);
    request.didFailSelector = @selector(requestDidFail:);
    
    //设置tag,userInfo属性来区别不同的请求,这些属性不会发送给服务器
    request.tag = 33;
    request.userInfo = @{@"hjKey":@"hjValue"};
    
    //将请求加入队列
    [self.queue addOperation:request];
    
}

/**
 *  请求完成
 */
-(void)requestDidFinish:(ASIHTTPRequest *)request
{
    request.responseString;
    request.responseData;
}

/**
 *  请求失败
 */
-(void)requestDidFail:(ASIHTTPRequest *)request
{
    request.error;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
