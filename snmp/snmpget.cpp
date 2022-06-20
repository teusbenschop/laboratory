#include <string>
#include <iostream>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/utilities.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/output_api.h>

using namespace std;


int main (int argc, char *argv[])
{
    // The session is defined here.
    // Purpose: It's in scope even after an exception in the try...catch block.
    netsnmp_session *ss {nullptr};
    
    try {

        // Create and initialize the session to default values.
        // http://www.net-snmp.org/dev/agent/structsnmp__session.html
        // The snmp_sess_init prepares a struct snmp_session
        // that sources transport characteristics and common information
        // that will be used for a set of SNMP transactions.
        netsnmp_session session {};
        snmp_sess_init (&session);

        // Get the common command line arguments.
//        int arg { 0 };
//        switch (arg = snmp_parse_args(argc, argv, &session, NULL, NULL)) {
//            case NETSNMP_PARSE_ARGS_ERROR:
//                throw string ("Error parsing arguments");
//            case NETSNMP_PARSE_ARGS_SUCCESS_EXIT:
//                throw string ("Ready parsing arguments");
//            case NETSNMP_PARSE_ARGS_ERROR_USAGE:
//                throw string ("Error parsing arguments due to incorrect usage");
//            default:
//                break;
//        }
//        if (arg >= argc) {
//            throw string ("Missing object name");
//        }
//        if ((argc - arg) > SNMP_MAX_CMDLINE_OIDS) {
//            throw string ("Too many object identifiers specified");
//        }

        // The snmp version, one of SNMP_VERSION_1 / SNMP_VERSION_2c / SNMP_VERSION_3.
        session.version = SNMP_VERSION_2c;

        // The number of retries before timeout.
        // Note that one retry means it will try 2 times.
        session.retries = 1;

        // Number of microseconds until first timeout, then exponential backoff.
        session.timeout = 2000000;

        // Name or address of default peer (may include transport specifier and/or port number)
        session.peername = const_cast <char *> ("udp:127.0.0.1:161");

        // The community for outgoing requests and the length of the community name.
        {
            char * char_ptr = const_cast <char *> ("public");
            session.community = reinterpret_cast <u_char*> (char_ptr);
            session.community_len = strlen (char_ptr);
        }

        // Define the object name(s).
        char * names[SNMP_MAX_CMDLINE_OIDS];
        int current_name { 0 };
        //for (; arg < argc; arg++) names[current_name++] = argv[arg];
        names [current_name++] = const_cast <char *> ("iso.3.6.1.2.1.1.3.0");
        
        // Open an SNMP session.
        // The snmp_open returns a pointer to a newly-formed struct snmp_session object,
        // which the application must use to reference the active SNMP session.
        ss = snmp_open(&session);
        if (ss == NULL) {
            // Diagnose snmp_open errors with the input netsnmp_session pointer.
            snmp_sess_perror("snmpget", &session);
            throw string ("Cannot open session");
        }

        // Repeat the request several times.
        for (int repeat = 0; repeat < 10; repeat++) {

            // Create PDU for GET request and add object names to request.
            size_t name_length = MAX_OID_LEN;
            oid name[MAX_OID_LEN];
            int failures { 0 };
            netsnmp_pdu * pdu = snmp_pdu_create(SNMP_MSG_GET);
            for (int count = 0; count < current_name; count++) {
                if (!snmp_parse_oid(names[count], name, &name_length)) {
                    snmp_perror(names[count]);
                    failures++;
                } else
                    snmp_add_null_var(pdu, name, name_length);
            }
            if (failures) throw string ("Cannot parse OID");
            
            // Perform the request.
            netsnmp_pdu *response { nullptr };
            netsnmp_variable_list *vars { nullptr };
            int status = snmp_synch_response(ss, pdu, &response);
            if (status == STAT_SUCCESS) {
                if (response->errstat == SNMP_ERR_NOERROR) {
                    for (vars = response->variables; vars; vars = vars->next_variable) {
                        print_variable(vars->name, vars->name_length, vars);
                    }
                }
                else {
                    cout << "Error in packet" << endl;
                    cout << "Reason: " << snmp_errstring(response->errstat) << endl;
                    
                    if (response->errindex != 0) {
                        fprintf(stderr, "Failed object: ");
                        int count { 0 };
                        for (count = 1, vars = response->variables;
                             vars && count != response->errindex;
                             vars = vars->next_variable, count++)
                        /*EMPTY*/;
                        if (vars) {
                            fprint_objid(stderr, vars->name, vars->name_length);
                        }
                        fprintf(stderr, "\n");
                    }
                } // endif -- SNMP_ERR_NOERROR.
                
            } // endif -- STAT_SUCCESS.
            else if (status == STAT_TIMEOUT) {
                string message = "Timeout: No Response from " + string (session.peername);
                throw message;
            }
            else { // status == STAT_ERROR.
                snmp_sess_perror("snmpget", ss);
            }
            
            if (response) snmp_free_pdu(response);

            
        }
        

    }
    catch (const string & msg) {
        cout << "Execution error: " << msg << endl;
    }
    catch (...) {
        cout << "General error" << endl;
    }

    // Close the session.
    if (ss) snmp_close(ss);

    // Ready.
    return 0;
}
