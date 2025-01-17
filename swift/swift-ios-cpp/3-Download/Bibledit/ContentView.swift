import SwiftUI
@preconcurrency import WebKit

struct ContentView: View {
    let webView = WebView(request: URLRequest(url: URL(string: "https://bibledit.org:8091/exports/Sample/usfm/full")!))
    var body: some View {
        webView
    }
}


class Coordinator: NSObject {
    var parent: WebView
    fileprivate var downloadDestinationURL: URL?
    init(parent: WebView) {
        self.parent = parent
    }
}


extension Coordinator: WKNavigationDelegate {
    
    func webView(_ webView: WKWebView, decidePolicyFor navigationAction: WKNavigationAction, preferences: WKWebpagePreferences, decisionHandler: @escaping (WKNavigationActionPolicy, WKWebpagePreferences) -> Void) {
        if navigationAction.shouldPerformDownload {
            print ("action decision download")
            decisionHandler(.download, preferences)
        } else {
            print ("action decision allow")
            decisionHandler(.allow, preferences)
        }
    }
    
    func webView(_ webView: WKWebView, decidePolicyFor navigationResponse: WKNavigationResponse, decisionHandler: @escaping (WKNavigationResponsePolicy) -> Void) {
        if navigationResponse.canShowMIMEType {
            print ("response decision allow")
            decisionHandler(.allow)
        } else {
            print ("response decision download")
            decisionHandler(.download)
        }
    }
}


extension Coordinator: WKDownloadDelegate {
    
    func webView(_ webView: WKWebView, navigationAction: WKNavigationAction, didBecome download: WKDownload) {
        download.delegate = self
    }
    
    func download(_ download: WKDownload, decideDestinationUsing response: URLResponse, suggestedFilename: String, completionHandler: @escaping (URL?) -> Void) {
        let fileManager = FileManager.default
        let documentDirectory = fileManager.urls(for: .documentDirectory, in: .userDomainMask)[0]
        let fileUrl =  documentDirectory.appendingPathComponent("\(suggestedFilename)", isDirectory: false)
        
        parent.downloadUrl = fileUrl
        completionHandler(fileUrl)
    }
    
    func downloadDidFinish(_ download: WKDownload) {
        print ("download did finish")
        print (download)
    }

    func downloadDidFinish(location url: URL) {
        print ("download did finish 2")
        DispatchQueue.main.async {
            let activityVC = UIActivityViewController(activityItems: [url], applicationActivities: nil)
//            activityVC.popoverPresentationController?.sourceView = self.view
//            activityVC.popoverPresentationController?.sourceRect = self.view.frame
//            activityVC.popoverPresentationController?.barButtonItem = self.navigationItem.rightBarButtonItem
//            self.present(activityVC, animated: true, completion: nil)
        }
    }
    
    func download(_ download: WKDownload, didFailWithError error: Error, resumeData: Data?) {
        print(error.localizedDescription)
        // you can add code here to continue the download in case there was a failure.
    }
}


struct WebView: UIViewRepresentable {
    let request: URLRequest
    private var webView: WKWebView
    var downloadUrl = URL(fileURLWithPath: "")

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
    
    func loadURL(urlString: String) {
        webView.load(URLRequest(url: URL(string: urlString)!))
    }

}
