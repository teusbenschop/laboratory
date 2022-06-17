#include <string>
#include <iostream>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/utilities.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/output_api.h>

using namespace std;

#define NETSNMP_DS_APP_DONT_FIX_PDUS 0


static void optProc(int argc, char *const *argv, int opt)
{
    switch (opt) {
        case 'C':
        {
            while (*optarg) {
                switch (*optarg++) {
                    case 'f':
                        netsnmp_ds_toggle_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_APP_DONT_FIX_PDUS);
                        break;
                    default:
                        string message = string ("Unknown flag passed to -C: ");
                        message += optarg[-1];
                        throw message;
                }
            }
            break;
        }
    }
}


void usage(void)
{
    cout << "USAGE: snmpget ";
    snmp_parse_args_usage(stdout);
    cout << " OID [OID]..." << endl << endl;
    snmp_parse_args_descriptions(stdout);
    cout << " -C APPOPTS Set various application specific behaviours:" << endl;
    cout << "  f: do not fix errors and retry the request" << endl;
}


int main (int argc, char *argv[])
{
    netsnmp_session *ss {nullptr};
    
    try {
        
        SOCK_STARTUP;

        netsnmp_session session {};

        // Get the common command line arguments.
        int arg { 0 };
        switch (arg = snmp_parse_args(argc, argv, &session, "C:", optProc)) {
            case NETSNMP_PARSE_ARGS_ERROR:
                throw string ("Error parsing arguments");
            case NETSNMP_PARSE_ARGS_SUCCESS_EXIT:
                throw string ("Error parsing arguments");
            case NETSNMP_PARSE_ARGS_ERROR_USAGE:
                usage();
                throw string ("Error parsing arguments");
            default:
                break;
        }
        
        if (arg >= argc) {
            fprintf(stderr, "\n");
            usage();
            throw string ("Missing object name");
        }
        if ((argc - arg) > SNMP_MAX_CMDLINE_OIDS) {
            usage();
            throw string ("Too many object identifiers specified");
        }
        
        // Get the object names.
        char * names[SNMP_MAX_CMDLINE_OIDS];
        int current_name { 0 };
        for (; arg < argc; arg++) names[current_name++] = argv[arg];
        
        
        // Open an SNMP session.
        ss = snmp_open(&session);
        if (ss == NULL) {
            // Diagnose snmp_open errors with the input netsnmp_session pointer
            snmp_sess_perror("snmpget", &session);
            throw string ("Cannot open session");
        }

        // Repeat the request several times.
        for (int repeat = 0; repeat < 5; repeat++) {

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

    // Cleanup.
    SOCK_CLEANUP;

    // Ready.
    return 0;
}
