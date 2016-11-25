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

#import <os/object.h>
#import <os/activity.h>

/*
 System Versioning Preprocessor Macros
 */
#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v) ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)

/*
 os_log is only supported when compiling with Xcode 8.
 Check if iOS version >= 10 and the _os_log_internal symbol exists,
 load it dynamically and call it.
 Definitions extracted from #import <os/log.h>
 See for more details https://developer.apple.com/reference/os/1891852-logging as well.
 */
#if OS_OBJECT_SWIFT3
    OS_OBJECT_DECL_SWIFT(os_log);
#elif OS_OBJECT_USE_OBJC
    OS_OBJECT_DECL(os_log);
#else
    typedef struct os_log_s *os_log_t;
#endif /* OS_OBJECT_USE_OBJC */

extern struct os_log_s _os_log_default;
extern __attribute__((weak)) void _os_log_internal(void *dso, os_log_t log, int type, const char *message, ...);

/*
 In iOS 10 NSLog only shows in device log when debugging from Xcode:
 'clang diagnostic error "-Wformat"' will make the compiler treat as errors supplied arguments
 that don't have types appropriate to the format string specified,
 or for which the conversions specified in the format string don't make sense.
 
 WARNING: Use _os_log_internal ONLY for iOS >= 10 if available.
 */

#define NSLog(FORMAT, ...) \
if (SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(@"10.0") && _os_log_internal != NULL) {\
    _Pragma("clang diagnostic push")\
    _Pragma("clang diagnostic error \"-Wformat\"")\
    _os_log_internal(&__dso_handle, OS_OBJECT_GLOBAL_OBJECT(os_log_t, _os_log_default), 0x00, [[NSString stringWithFormat:FORMAT, ##__VA_ARGS__] UTF8String]);\
    _Pragma("clang diagnostic pop")\
} else {\
    NSLog(FORMAT, ##__VA_ARGS__);\
}

#import <Cordova/CDVPlugin.h>

@interface CDVLogger : CDVPlugin

- (void)logLevel:(CDVInvokedUrlCommand*)command;

@end
