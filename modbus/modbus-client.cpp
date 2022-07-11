
#include <string>
#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>

using namespace std;

int main ()
{
    
    // Create a new TCP context with proper parameters
    // (in this example, the local host and the correct port number).
    modbus_t * ctx = modbus_new_tcp("127.0.0.1", 1502);
    if (!ctx) {
        cout << "Failed to create the context: " << modbus_strerror(errno) << endl;
        return 0;
    }
    
    //modbus_set_debug(ctx, TRUE);
    
    // Define a timeout of 500ms.
    modbus_set_response_timeout(ctx, 0, 500000);
    
    // Keep going.
    constexpr bool keep_going {true};
    while (keep_going) {
        
        // This client connects to the server.
        if (modbus_connect(ctx) == -1) {
            cout << "Unable to connect: " << modbus_strerror(errno) << endl;
            modbus_free(ctx);
            return 0;
        }
        
        // Set the Modbus address of the remote slave (to 3).
        // The slave number is only required in TCP
        // if the message must reach a device on a serial network.
        // Some not compliant devices or software uses the slave ID as unit identifier.
        // That's incorrect but without the slave value,
        // the faulty remote device or software drops the requests.
        // modbus_set_slave(ctx, 3);
        
        // This will store the values of the read registers.
        constexpr int holding_register_start { 0 };
        constexpr int holding_registers_count { 5 };
        uint16_t registers [holding_registers_count];
        
        // Do several requests then be done with this connection.
        for (int i = 0; i < 5; i++) {
            
            // Read 5 holding registers starting from address 10.
            int num = modbus_read_registers (ctx, 10, holding_registers_count, registers);
            
            // If the number of read registers is okay, print them.
            if (num == holding_registers_count) {

                time_t tt = std::chrono::system_clock::to_time_t ( std::chrono::system_clock::now() );
                std::string time = ctime(&tt);
                time.erase(time.find_last_not_of("\n ") + 1); // right trim
                std::cout << time << " holding register";
                for (size_t offset = 0; offset < holding_registers_count; offset++) {
                    std::cout << " " << holding_register_start + offset << ":" << registers[offset];
                }
                std::cout << std::endl;
                
            }
            
            // Handle error.
            else {
                cout << "Failed to read: " << modbus_strerror(errno) << endl;
            }

            static uint16_t output_register_1 { 0 };
            num = modbus_write_register (ctx, 10, output_register_1++);
            if (num != 1) {
                cout << "Failed to read: " << modbus_strerror(errno) << endl;
            }
            
            this_thread::sleep_for(chrono::milliseconds(500));
        }
        
        int socket = modbus_get_socket (ctx);
        if (socket > 0) close (socket);
        
    }
    
    // Ready.
    modbus_close(ctx);
    modbus_free(ctx);
    
    return 0;
}
