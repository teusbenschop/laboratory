
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>
#include <sys/socket.h>

using namespace std;

int main ()
{
  // Prepare a Modbus mapping as follows:
  // 0 output coils.
  // 1 input coil.
  // 30 holding registers.
  // 2 input registers.
  // This will also automatically set the value of each register to 0.
  modbus_mapping_t * mapping = modbus_mapping_new (0, 1, 30, 2);
  if (!mapping) {
    cout << "Failed to allocate the mapping: " << modbus_strerror(errno) << endl;
    return 0;
  }
  
  // Example: Set register 12 to integer value 623
  mapping->tab_registers[12] = 623;

  modbus_t *ctx = modbus_new_tcp("127.0.0.1", 1502);
  if (!ctx) {
    cout << "Failed to create the context: " << modbus_strerror(errno) << endl;
    return 0;
  }
  
  modbus_set_debug(ctx, TRUE);
  
  // Set the Modbus address of this slave (server) (to 3).
  // The slave number is only required in TCP
  // if the message must reach a device on a serial network.
  // Some not compliant devices or software uses the slave ID as unit identifier.
  // That's incorrect but without the slave value,
  // the faulty remote device or software drops the requests.
  // modbus_set_slave(ctx, 3);

  int listen_socket = modbus_tcp_listen(ctx, 1);
  if (listen_socket < 0) {
    cout << "Failed to listen on the socket: " << modbus_strerror(errno) << endl;
    modbus_free(ctx);
    return 0;
  }

  bool keep_going {true};
  while (keep_going) {

    // Block till it accepts a new connection from a client (is master).
    int accepted_socket = modbus_tcp_accept(ctx, &listen_socket);
    if (accepted_socket < 0) {
      cout << "Failed to accept a client on the socket: " << modbus_strerror(errno) << endl;
      continue;
    }

    // Buffer to store the request.
    uint8_t request [MODBUS_TCP_MAX_ADU_LENGTH];
    
    while (keep_going) {

      // The receive function stores the indication request in the request buffer.
      // It returns the request length if successful.
      // The returned request length can be zero if the indication request is ignored
      // (eg. a query for another slave in RTU mode).
      // Otherwise it return -1 and set errno.
      int len = modbus_receive(ctx, request);
      if (len == 0) continue;
      if (len < 0) {
        cout << "Failed to receive the indication request: " << modbus_strerror(errno) << endl;
        break;
      }
      
      static int counter = 0;
      counter++;
      cout << counter << endl;

      // Send the reply based on the mapping.
      len = modbus_reply(ctx, request, len, mapping);
      if (len < 0) {
        cout << "Failed to send the reply to the request: " << modbus_strerror(errno) << endl;
      }
      
    }
    
    // Close the connection to the client.
    if (accepted_socket) close (accepted_socket);
  }

  // Done: Free the structures.
  if (listen_socket != -1) {
    close(listen_socket);
  }
  modbus_mapping_free(mapping);
  modbus_close(ctx);
  modbus_free(ctx);
}
