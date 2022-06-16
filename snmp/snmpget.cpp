#include <string>
#include <iostream>
#include <net-snmp/net-snmp-config.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
# include <sys/time.h>
# include <time.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>
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
                        fprintf(stderr, "Unknown flag passed to -C: %c\n",
                                optarg[-1]);
                        exit(1);
                }
            }
            break;
        }
    }
}


void usage(void)
{
    fprintf(stderr, "USAGE: snmpget ");
    snmp_parse_args_usage(stderr);
    fprintf(stderr, " OID [OID]...\n\n");
    snmp_parse_args_descriptions(stderr);
    fprintf(stderr, " -C APPOPTS\t\tSet various application specific behaviours:\n");
    fprintf(stderr, "\t\t\t  f:  do not fix errors and retry the request\n");
}


int main (int argc, char *argv[])
{
    try {

        
        netsnmp_session session, *ss;
        netsnmp_pdu    *pdu;
        netsnmp_pdu    *response;
        netsnmp_variable_list *vars;
        int             arg;
        int             count;
        int             current_name = 0;
        char           *names[SNMP_MAX_CMDLINE_OIDS];
        oid             name[MAX_OID_LEN];
        size_t          name_length;
        int             status;
        int             failures = 0;
        int             exitval = 1;
        
        SOCK_STARTUP;
        
        // Get the common command line arguments.
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
        for (; arg < argc; arg++) names[current_name++] = argv[arg];
        
        // Open an SNMP session.
        ss = snmp_open(&session);
        if (ss == NULL) {
            // Diagnose snmp_open errors with the input netsnmp_session pointer
            snmp_sess_perror("snmpget", &session);
            throw string ("Cannot open session");
        }
        
        
        
        

    }
    catch (const string & msg) {
        cout << msg << endl;
    }

    // Cleanup.
    SOCK_CLEANUP;

    // Ready.
    return 0;
}
