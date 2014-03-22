#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

@interface MacWebDelegate : NSObject {
}

- (WebView *)webView:(WebView *)sender createWebViewWithRequest:(NSURLRequest *)request;
@end
