import SwiftUI
@preconcurrency import WebKit

struct ContentView: View {
    let webView = WebView(request: URLRequest(url: URL(string: "https://bibledit.org:8091/exports/Sample/usfm/full")!))
    var body: some View {
        VStack {
            webView
            HStack {
                Button(action: {
                    self.webView.goBack()
                }){
                    Image(systemName: "arrowtriangle.left.fill")
                        .font(.title)
                        .foregroundColor(.blue)
                        .padding()
                }
                Spacer()
                Button(action: {
                    self.webView.goHome()
                }){
                    Image(systemName: "house.fill")
                        .font(.title)
                        .foregroundColor(.blue)
                        .padding()
                }
                Spacer()
                Button(action: {
                    self.webView.refresh()
                }){
                    Image(systemName: "arrow.clockwise.circle.fill")
                        .font(.title)
                        .foregroundColor(.blue)
                        .padding()
                }
                Spacer()
                Button(action: {
                    self.webView.goBack()
                }){
                    Image(systemName: "arrowtriangle.right.fill")
                        .font(.title)
                        .foregroundColor(.blue)
                        .padding()
                }
            }
        }
    }
}

struct WebView: UIViewRepresentable {
    let request: URLRequest
    private var webView: WKWebView

    init(request: URLRequest) {
        self.webView = WKWebView()
        self.request = request
    }

    func makeCoordinator() -> Coordinator {
        Coordinator(parent: self)
    }
    
    func makeUIView(context: Context) -> WKWebView {
        return webView
    }

    func updateUIView(_ uiView: WKWebView, context: Context) {
        uiView.navigationDelegate = context.coordinator
        uiView.load(request)
    }
    
    func goBack(){
        webView.goBack()
    }
    
    func goForward(){
        webView.goForward()
    }
    
    func loadURL(urlString: String) {
        webView.load(URLRequest(url: URL(string: urlString)!))
    }

    func refresh() {
        webView.reload()
    }
    
    func goHome() {
        webView.load(request)
    }

    class Coordinator: NSObject, WKNavigationDelegate {
        let parent: WebView
        
        init(parent: WebView) {
            self.parent = parent
        }

        func webView(_ webView: WKWebView, decidePolicyFor navigationAction: WKNavigationAction, preferences: WKWebpagePreferences, decisionHandler: @escaping (WKNavigationActionPolicy, WKWebpagePreferences) -> Void) {
            if navigationAction.shouldPerformDownload {
                decisionHandler(.download, preferences)
                print ("action decision download")
            } else {
                decisionHandler(.allow, preferences)
                print ("action decision allow")
            }
        }

        func webView(_ webView: WKWebView, decidePolicyFor navigationResponse: WKNavigationResponse, decisionHandler: @escaping (WKNavigationResponsePolicy) -> Void) {
            if navigationResponse.canShowMIMEType {
                decisionHandler(.allow)
                print ("response decision allow")
            } else {
                decisionHandler(.download)
                print ("response decision download")
            }
        }

        func download(_ download: WKDownload, decideDestinationUsing
                      response: URLResponse, suggestedFilename: String,
                      completionHandler: @escaping (URL?) -> Void) {
            print ("call completion handler")
            completionHandler(URL(string: "/tmp/ios-download"))
        }

        func downloadDidFinish(_ download: WKDownload) {
            print ("download did finish")
        }
        
        public func download(_ download: WKDownload, didFailWithError error: Error, resumeData: Data?) {
            print ("download error")
        }
        
    }
}
