//
//  HJFirstVC.m
//  HJAsiDemo
//
//  Created by huangjiong on 16/4/16.
//  Copyright © 2016年 huangjiong123. All rights reserved.
//

#import "HJFirstVC.h"
#import "ASIHTTPRequest.h"

@interface HJFirstVC ()<ASIHTTPRequestDelegate>

@end

@implementation HJFirstVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

#pragma mark - 同步请求
/**
 *  同步请求
 *  同步请求将发生死锁现象, 并且同步请求不能被取消
 */
-(void)syncRequest
{
    //1.url
    NSURL *url = [NSURL URLWithString:@""];
    //2.请求对象
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    //3.开始同步请求
    [request startSynchronous];
    //4.处理请求结果
    NSError *error = request.error;
    if (error) {
        NSLog(@"请求失败:%@",error);
    } else {
        NSLog(@"请求成功:%@",request.responseString);
        
    }
}


#pragma mark - 异步请求,block
/**
 *  第1种: 设置block回调来获取响应数据
 */
-(void)asyncRequestWithBlock
{
    //1.url
    NSURL *url = [NSURL URLWithString:@""];
    //2.请求对象
    __block ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    //3.设置回调block
    [request setCompletionBlock:^{
        request.responseString;
        request.responseData;
    }];
    [request setFailedBlock:^{
        request.error;
    }];
    
    //4.开始异步请求
    [request startAsynchronous];
    
    //5.注意: 此处在声明请求时，用到了__block限定语，这很重要！它告诉block不要保留请求，来防止一个保留循环(retain-cycle)，因为请求总会保留block.
}


#pragma mark - 异步请求,代理
/**
 *  第2种: 通过设置代理, 实现代理方法来获取响应数据
 */
-(void)asyncRequestWithDelegate
{
    //1.url
    NSURL *url = [NSURL URLWithString:@""];
    //2.请求对象
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
    //3.设置代理
    request.delegate = self;
    //4.开始异步请求
    [request startAsynchronous];
}





#pragma mark - ASIHTTPRequestDelegate
//实现代理方法

/**
 *  完成
 */
-(void)requestFinished:(ASIHTTPRequest *)request
{
    //字符串时
    NSString *responseString = request.responseString;
    
    //二进制流时
    NSData *responseData = request.responseData;
}

/**
 *  失败
 */
-(void)requestFailed:(ASIHTTPRequest *)request
{
    NSLog(@"请求失败:%@",request.error);
}










//- (void)didReceiveMemoryWarning {
//    [super didReceiveMemoryWarning];
//    // Dispose of any resources that can be recreated.
//}

@end
