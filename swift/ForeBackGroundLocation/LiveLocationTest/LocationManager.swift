//
//  LocationViewModel.swift
//  KanbanDemo
//
//  Created by Hardik Darji on 14/07/25.
//


import Foundation
import CoreLocation

import SwiftUI

@MainActor
class LocationManager: ObservableObject {
    @Published var isStationary: Bool = false
    private var backgroundActivity: CLBackgroundActivitySession?

    private var updateTask: Task<Void, Never>?
    private var updates = CLLocationUpdate.liveUpdates(.otherNavigation)
    
    private var keepRunning : Bool = false

    func requestPermission() {
        let manager = CLLocationManager()
        manager.requestWhenInUseAuthorization()
    }

    func start() {
        var counter : Int = 0
        keepRunning = true
        updateTask?.cancel()
        backgroundActivity = CLBackgroundActivitySession() // 👈 This keeps your app alive in background

        updateTask = Task {
            do {
                for try await update in updates {
                    if let loc = update.location {
                        counter += 1
                        print (counter, loc.coordinate.latitude, loc.coordinate.longitude)
                    }
                    // Whether the user is stationary.
                    self.isStationary = update.stationary
                    // To stop updates break out of the for loop
                    if !keepRunning {
                        break
                    }
                }
            } catch {
                print("Error in liveUpdates: \(error)")
            }
            print ("end task")
        }
    }

    func stop() {
        keepRunning = false
//        updateTask?.cancel()
//        updateTask = nil
        backgroundActivity = nil // 👈 End background session

    }
}
