
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
  
  modbus_set_debug(ctx, TRUE);
  
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
    constexpr int number_of_registers { 5 };
    uint16_t registers [number_of_registers];

    // Do several requests then be done with this connection.
    for (int i = 0; i < 5; i++) {

      static int counter = 0;
      counter++;
      cout << counter << endl;

      // Read 5 holding registers starting from address 10.
      int num = modbus_read_registers (ctx, 10, number_of_registers, registers);
      // Check the number of read registers whether it's correct.
      if (num != number_of_registers) {
        cout << "Failed to read: " << modbus_strerror(errno) << endl;
      }

      this_thread::sleep_for(chrono::milliseconds(100));

    }
    
    int socket = modbus_get_socket (ctx);
    if (socket > 0) close (socket);
    
  }

  // Ready.
  modbus_close(ctx);
  modbus_free(ctx);
  
  return 0;
}
