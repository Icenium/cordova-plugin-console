/*
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
 */

/*
    WARNING:

    Override NSLOG() for iOS >= 10.x due to major change by Apple in iOS 10 SDK.
    Currently apps built with Xcode 7.x, iOS 9.x SDK don't produce any logs with NSLog()
    when deplyed on devices running on >= iOS v10.x.
    This implementation should be erased once AppBuilder starts using
    Xcode 8.x with iOS 10.x SDK for the build procedure.

    In iOS 10 & Xcode 8, Apple switched from the good old ASL (Apple System Log)
    to a new logging system called Unified Logging.
    NSLog calls are in fact delegating to new os_log API's.
    See for more details https://developer.apple.com/reference/os/1891852-logging

    From: https://developer.apple.com/reference/os/os_log_with_type?language=objc
        Calling this function doesn’t ensure that a message is logged.
        Logging always occurs in accordance with the behavior settings
        of the provided log object and type. Note that lengthy log
        messages may be truncated when stored by the logging system.
 
    From <os/log.h>, line 221, https://opensource.apple.com/source/xnu/xnu-3789.1.32/libkern/os/log.h?txt:
        * There is a physical cap of 1024 bytes per log line for dynamic content,
        * such as %s and %@, that can be written to the persistence store.
        * All content exceeding the limit will be truncated before it is
        * written to disk.
 
    See also:
        * http://stackoverflow.com/questions/39584707/nslog-on-devices-in-ios-10-xcode-8-seems-to-truncate-why
        * http://teampulse.telerik.com/view#item/322341
 */


#import <os/object.h>
#import <os/activity.h>

#if OS_OBJECT_SWIFT3
OS_OBJECT_DECL_SWIFT(os_log);
#elif OS_OBJECT_USE_OBJC
OS_OBJECT_DECL(os_log);
#else
typedef struct os_log_s *os_log_t;
#endif /* OS_OBJECT_USE_OBJC */

extern struct os_log_s _os_log_default;
extern __attribute__((weak)) void _os_log_internal(void *dso, os_log_t log, int type, const char *message, ...);


#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v) ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)

#define NSLog(FORMAT, ...) \
if (SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(@"10.0") && _os_log_internal != NULL) {\
    const NSUInteger kMaxChunkLength = 1023;\
    NSString *messageToLog = [NSString stringWithFormat:FORMAT, ##__VA_ARGS__];\
\
    for (int i = 0; i < messageToLog.length; i+= kMaxChunkLength) {\
        NSRange range = NSMakeRange(i, MIN(messageToLog.length - i, kMaxChunkLength));\
        NSString *subString = [messageToLog substringWithRange:range];\
        _Pragma("clang diagnostic push")\
        _Pragma("clang diagnostic error \"-Wformat\"")\
        _os_log_internal(&__dso_handle, OS_OBJECT_GLOBAL_OBJECT(os_log_t, _os_log_default), 0x00, "%{public}s", subString.UTF8String);\
        _Pragma("clang diagnostic pop")\
    }\
} else {\
    NSLog(FORMAT, ##__VA_ARGS__);\
}

#import <Cordova/CDVPlugin.h>

@interface CDVLogger : CDVPlugin

- (void)logLevel:(CDVInvokedUrlCommand*)command;

@end
