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
    @Published var location: CLLocation = CLLocation()
    @Published var counter: Int = 0
    
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
//        updateTask?.cancel()
        // Create an object that manages a visual indicator to the user
        // that keeps the app in use in the background,
        // allowing it to receive updates or events.
        backgroundActivity = CLBackgroundActivitySession()

        updateTask = Task {
            do {
                for try await update in updates {
//                    try Task.checkCancellation()
                    if let loc = update.location {
                        counter += 1
                        print (counter, loc.coordinate.latitude, loc.coordinate.longitude)
                        self.location = update.location ?? CLLocation()
                        self.counter = counter
                    }
                    // Whether the user is stationary.
                    self.isStationary = update.stationary
                    // To stop updates break out of the for loop
//                    try Task.checkCancellation()
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
        // End background session.
        backgroundActivity = nil
    }
}
