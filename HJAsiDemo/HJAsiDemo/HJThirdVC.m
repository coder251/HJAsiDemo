//
//  HJThirdVC.m
//  HJAsiDemo
//
//  Created by huangjiong on 16/4/16.
//  Copyright © 2016年 huangjiong123. All rights reserved.
//

#import "HJThirdVC.h"
#import "ASIHTTPRequest.h"

@interface HJThirdVC ()

@end

@implementation HJThirdVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

-(void)postAndPut
{
    ASIHTTPRequest *request;
    //默认变为post
    [request appendPostData:[@"自定义数据" dataUsingEncoding:NSUTF8StringEncoding]];
    request.requestMethod = @"PUT";
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
