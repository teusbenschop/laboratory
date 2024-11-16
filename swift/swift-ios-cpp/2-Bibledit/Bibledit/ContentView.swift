import SwiftUI
import WebKit

let webview1 : WebView = WebView()
let webview2 : WebView = WebView()

let about_blank : String = "about:blank"

struct ContentView: View {
    
    let timer = Timer.publish(every: 3, on: .main, in: .common).autoconnect()
    
    @State var enable_second_view: Bool = false
    
    var body: some View {
        NavigationStack {
            webview1
            .navigationDestination(isPresented: $enable_second_view) {
                webview2
                .navigationBarBackButtonHidden(true)
                .onAppear() {
                    print ("webview2 appears")
                    webview2.loadURL(urlString: "https://freesoftwareconsultants.nl")
                    webview1.loadURL(urlString: about_blank)
                }
            }
            .onAppear() {
                print ("webview1 appears")
                webview1.loadURL(urlString: "https://bibledit.org:8091")
                webview2.loadURL(urlString: about_blank)
            }
        }
        .onAppear(){
            print ("body view appears")
        }
        .onReceive(timer) { input in
            enable_second_view.toggle()
        }
    }
    
    init() {
        print ("init")
    }
}


struct WebView: UIViewRepresentable {
    
    let webView: WKWebView
    
    init() {
        self.webView = WKWebView()
    }
    
    func makeUIView(context: Context) -> WKWebView {
        webView.allowsBackForwardNavigationGestures = true
        return webView
    }
    
    func updateUIView(_ uiView: WKWebView, context: Context) {
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
}

