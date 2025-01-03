import SwiftUI
@preconcurrency import WebKit

struct ContentView: View {
    let webView = WebView(request: URLRequest(url: URL(string: "https://bibledit.org:8091/exports/Sample/usfm/full")!))
    var body: some View {
        webView
    }
}


class Coordinator: NSObject, WKNavigationDelegate, WKDownloadDelegate {
    
    let parent: WebView
    
    fileprivate var downloadDestinationURL: URL?
    
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
    
    public func destinationUrlForFile(withName name: String) -> URL? {
        let temporaryDir = NSTemporaryDirectory()
        let url = URL(fileURLWithPath: temporaryDir)
            .appendingPathComponent(UUID().uuidString)
        
        if ((try? FileManager.default.createDirectory(at: url, withIntermediateDirectories: false)) == nil) {
            return nil
        }
        
        return url.appendingPathComponent(name)
    }
    
    func download(_ download: WKDownload,
                  decideDestinationUsing response: URLResponse,
                  suggestedFilename: String,
                  completionHandler: @escaping (URL?) -> Void) {
        print ("decide destination using")
        completionHandler(URL(string: "/tmp/ios-download"))
    }
    
    func downloadDidFinish(_ download: WKDownload) {
        print ("download did finish")
    }
    
    public func download(_ download: WKDownload, didFailWithError error: Error, resumeData: Data?) {
        print ("download error")
    }
    
    public func downloadDidFailed(withError error: Error) {
         print (error)
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

}
