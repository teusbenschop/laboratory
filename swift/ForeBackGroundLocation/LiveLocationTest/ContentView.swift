import SwiftUI

struct ContentView: View {
    @StateObject private var locationManager = LocationManager()
    
    var body: some View {
        NavigationView {
            VStack {
                Text("Latitude " + locationManager.location.coordinate.latitude.description)
                Text("Longitude " + locationManager.location.coordinate.longitude.description)
                Text("Speed " + locationManager.location.speed.description)
                Text("Time " + locationManager.location.timestamp.description)
                Text("Sequence " + locationManager.sequence.description)
                    .padding(.bottom)
                HStack {
                    Toggle(isOn: $locationManager.running, label: {
                        Label("Receive location updates", systemImage: locationManager.running ? "location.fill" : "location")
                    })
                    .padding()
                    .onChange(of: locationManager.running) {
                        if locationManager.running {
                            locationManager.start()
                        } else {
                            locationManager.stop()
                        }
                    }
                }
            }
        }
        .onAppear {
            locationManager.requestPermission()
        }
    }
}
