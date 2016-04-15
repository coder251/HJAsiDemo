//
//  ViewController.m
//  HJAsiDemo
//
//  Created by huangjiong on 16/4/15.
//  Copyright © 2016年 huangjiong123. All rights reserved.
//



#import "ViewController.h"
#import "ASIHTTPRequest.h"

@interface ViewController ()<ASIHTTPRequestDelegate>

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
}

#pragma mark - 同步请求
/**
 *  同步请求
 */
-(void)sync
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
-(void)asyncWithBlock
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
    
    //5.注意:注意：在我们声明请求时，用到了__block限定语，这很重要！它告诉block不要保留请求，来防止一个保留循环(retain-cycle)，因为请求总会保留block.
}


#pragma mark - 异步请求,代理
/**
 *  异步请求
 */
-(void)asyncWithDelegate
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
