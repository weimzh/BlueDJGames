#import "MacWebDelegate.h"

@implementation MacWebDelegate

- (WebView *)webView:(WebView *)sender createWebViewWithRequest:(NSURLRequest *)request
{
	return sender;
}

@end
