import SwiftUI
@preconcurrency import WebKit

struct ContentView: View {
    let web_view = WebView(request: URLRequest(url: URL(string: "https://bibledit.org:8091/exports/Sample/usfm/full")!))
    var body: some View {
        web_view
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
        print ("download decide destination")
        let file_manager = FileManager.default
        let document_directory = file_manager.urls(for: .documentDirectory, in: .userDomainMask)[0]
        let file_url = document_directory.appendingPathComponent("\(suggestedFilename)", isDirectory: false)
        do {
            try file_manager.removeItem(at: file_url)
        }
        catch {
            print(error)
        }
        parent.download_url = file_url
        print(file_url)
        completionHandler(file_url)
    }
    
    func downloadDidFinish(_ download: WKDownload) {
        print ("download did finish")
        print (parent.download_url)
        DispatchQueue.main.async {
            guard let root_view_controller = UIApplication.shared.windows.last?.rootViewController
            else { return }
            let view_controller = UIActivityViewController(activityItems: [self.parent.download_url], applicationActivities: nil)
            view_controller.excludedActivityTypes = nil
            view_controller.popoverPresentationController?.sourceView = root_view_controller.view
            root_view_controller.present(view_controller, animated: true)
        }
    }

    func download(_ download: WKDownload, didFailWithError error: Error, resumeData: Data?) {
        print ("download failed with error")
        print(error.localizedDescription)
        // You can add code here to continue the download in case there was a failure.
    }
}


struct WebView: UIViewRepresentable {
    let request: URLRequest
    private var webView: WKWebView
    var download_url = URL(fileURLWithPath: "")

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
