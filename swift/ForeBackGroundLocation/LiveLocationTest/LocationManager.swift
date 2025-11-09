import Foundation
import CoreLocation

import SwiftUI

@MainActor
class LocationManager: ObservableObject {

    @Published var location: CLLocation = CLLocation()
    @Published var sequence: Int = 0
    @Published var running : Bool = false
    
    private var backgroundActivitySesion: CLBackgroundActivitySession? = nil

    private var updates = CLLocationUpdate.liveUpdates(.otherNavigation)
    
    func requestPermission() {
        let manager = CLLocationManager()
        manager.requestWhenInUseAuthorization()
    }

    func start() {
        sequence = 0
        
        // Create an object that manages a visual indicator to the user
        // that keeps the app in use in the background,
        // allowing it to receive updates or events.
        backgroundActivitySesion = CLBackgroundActivitySession()

        Task {
            do {
                for try await update in updates {
                    if let loc = update.location {
                        sequence += 1
                        print (sequence, loc.coordinate.latitude, loc.coordinate.longitude)
                        self.location = update.location ?? CLLocation()
                        self.sequence = sequence
                    }
                    // To stop updates, break out of the for loop.
                    if !running {
                        break;
                    }
                }
            } catch {
                print("Error in liveUpdates: \(error)")
            }
        }
    }

    func stop() {
        // End background session.
        backgroundActivitySesion = nil
    }
}
