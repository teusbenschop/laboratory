import SwiftUI
import WebKit

let about_blank : String = "about:blank"

let webview_advanced : WebView = WebView()
let webview_translate : WebView = WebView()
let webview_resources : WebView = WebView()
let webview_notes : WebView = WebView()
let webview_settings : WebView = WebView()


struct ContentView: View {
    
    let timer = Timer.publish(every: 5, on: .main, in: .common).autoconnect()
    
    @State var enable_second_view: Bool = false
    
    var body: some View {
        NavigationStack {
            webview_advanced
            .navigationDestination(isPresented: $enable_second_view) {
                BasicView()
                .navigationBarBackButtonHidden(true)
                .onAppear() {
                    webview_advanced.loadURL(urlString: about_blank)
                    webview_translate.loadURL(urlString: "https://bibledit.org:8091/editone2/index")
                    webview_resources.loadURL(urlString: "https://bibledit.org:8091/resource/index")
                    webview_notes.loadURL(urlString: "https://bibledit.org:8091/notes/index")
                    webview_settings.loadURL(urlString: "https://bibledit.org:8091/index/index?item=settings")
                }
            }
            .onAppear() {
                webview_translate.loadURL(urlString: about_blank)
                webview_resources.loadURL(urlString: about_blank)
                webview_notes.loadURL(urlString: about_blank)
                webview_settings.loadURL(urlString: about_blank)
                webview_advanced.loadURL(urlString: "https://bibledit.org:8091")
            }
        }
        .onAppear(){
        }
        .onReceive(timer) { input in
            enable_second_view.toggle()
        }
    }
    
    init() {
    }
}


struct BasicView: View {
    var body: some View {
        TabView {
            webview_translate
                .tabItem {
                    Label("Translate", systemImage: "doc")
                }
            webview_resources
                .tabItem {
                    Label("Resources", systemImage: "book")
                }
            webview_notes
                .tabItem {
                    Label("Notes", systemImage: "note")
                }
            webview_settings
                .tabItem {
                    Label("Settings", systemImage: "gear")
                }
        }
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
