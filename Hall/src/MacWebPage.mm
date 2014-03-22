//****************************************************************************
//Copyright (C) 2005-2006 Beijing BlueDJ Technology Co.,Ltd. All rights reserved.

//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program (in the file LICENSE.GPL); if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

//Please visit http://www.bluedj.com for more infomation about us.
//Contact us at ggwizard@gmail.com or darkdong@gmail.com.
//****************************************************************************/

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#import <WebKit/WebKit.h>
#import <WebKit/HIWebView.h>
#import <WebKit/CarbonUtils.h>

#import "MacWebDelegate.h"

static MacWebDelegate *webDelegate	= 0;

void EmbedWebPage( CFStringRef url, HIViewRef hiView, HIViewRef &hiWebView )
{
	WebInitForCarbon();
	
	if ( 0 == webDelegate ) {
		webDelegate	= [[MacWebDelegate alloc] init];
	}
	
	if ( 0 == hiWebView ) {
		HIWebViewCreate( &hiWebView ); // create the web view
		
		//set web view frame when receive resizeEvent
		// Embed the web view in the content view and make it visible 
		HIViewAddSubview( hiView, hiWebView );
		HIViewSetVisible( hiWebView, true );
	}
	
    WebView*			nativeView;
    NSURLRequest*       request;
    WebFrame*           mainFrame;
 
    nativeView = HIWebViewGetWebView( hiWebView ); // get the Cocoa view
	[nativeView setUIDelegate:webDelegate];
    // Use Objective-C calls to load the actual content
    request = [NSURLRequest requestWithURL:[NSURL URLWithString:(NSString*)url]];
    mainFrame = [nativeView mainFrame];
    [mainFrame loadRequest:request];
}
