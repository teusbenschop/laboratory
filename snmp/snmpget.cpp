#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/utilities.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/output_api.h>

using namespace std;


int main ()
{
    // Prepare the default directories where the MIB files can be stored.
    netsnmp_get_mib_directory();
    // Get the home directory, then build folder where MIB files are.
    string mib_directory {};
    {
        const char *homedir;
        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }
        string home_directory = homedir;
        mib_directory = "+" + home_directory + "/snmp/mibs";
    }
    netsnmp_set_mib_directory(mib_directory.c_str());
    
    // Specify a colon separated list of MIB modules (not files) to load for this application.
    // This overrides or augments the environment variable MIBS,
    // the snmp.conf directive mibs, and the list of MIBs hardcoded into the Net-SNMP library.
    // If MIBLIST has a leading '-' or '+' character,
    // then the MIB modules listed are loaded in addition to the default list,
    // coming before or after this list respectively.
    // Otherwise, the specified MIBs are loaded instead of this default list.
    // The special keyword ALL is used
    // to load all MIB modules in the MIB directory search list.
    // Every file whose name does not begin with "." will be parsed as if it were a MIB file.
    // Set (and overwrite) the MIBS list in the environent.
    setenv("MIBS", "+ALL", 1);

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

        // Parse the simulated command line arguments.
        // There's something not yet resolved in this library.
        // It complains that it cannot resolve the name OID if there's no arguments passed.
        // So set some fake arguments here, and let the SNMP library parse them.
        // For some as yet unknown reason, then it goes well.
        // The fake arguments will be allocated in dynamic memory,
        // because they can be written to by the parser.
        // At the end,free the temporal arguments allocated in dynamic memory.
        {
            constexpr int argc = 3;
            char * argv [argc] { strdup ("program"), strdup ("host"), strdup ("oid") };
            snmp_parse_args(argc, argv, &session, NULL, NULL);
            for (int i = 0; i < argc; i++) free (argv[i]);
        }

        // The snmp version, one of SNMP_VERSION_1 / SNMP_VERSION_2c / SNMP_VERSION_3.
        session.version = SNMP_VERSION_3;

        // The number of retries before timeout.
        // Note that one retry means it will try 2 times.
        session.retries = 1;

        // Number of microseconds until first timeout, then exponential backoff.
        session.timeout = 2000000;

        // Name or address of default peer (may include transport specifier and/or port number)
        string peername = "tcp:192.168.140.10:161";
        session.peername = const_cast <char *> (peername.c_str());

        // The default SNMPv3 security name to use when using SNMPv3.
        string security_name = "username";
        {
            char * char_ptr = const_cast <char *> (security_name.c_str());
            session.securityName = char_ptr;
            session.securityNameLen = strlen (char_ptr);
        }

        // The default SNMPv3 context name to use.
        string context_name = "VMS1";
        {
            char * char_ptr = const_cast <char *> (context_name.c_str());
            session.contextName = char_ptr;
            session.contextNameLen = strlen (char_ptr);
        }

        // The default SNMPv3 security level to use.
        // One of: SNMP_SEC_LEVEL_NOAUTH / SNMP_SEC_LEVEL_AUTHNOPRIV / SNMP_SEC_LEVEL_AUTHPRIV
        session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;

        // The default SNMPv3 authentication type name to use.
        {
            string default_authentication_type {"MD5"};
            int auth_type = usm_lookup_auth_type(default_authentication_type.c_str());
            if (auth_type > 0) {
                session.securityAuthProto = sc_get_auth_oid(auth_type, &session.securityAuthProtoLen);
            }
        }

        // The default SNMPv3 authentication pass phrase to use.
        // Note: It must be at least 8 characters long.
        string pass_phrase = "password";
        {
            char * Apsz = const_cast <char *> (pass_phrase.c_str());
            if (Apsz) {
                session.securityAuthKeyLen = USM_AUTH_KU_LEN;
                if (generate_Ku(session.securityAuthProto,
                                session.securityAuthProtoLen,
                                (u_char *) Apsz, strlen(Apsz),
                                session.securityAuthKey,
                                &session.securityAuthKeyLen) != SNMPERR_SUCCESS) {
                    throw string ("Error generating a key (Ku) from the supplied authentication pass phrase.");
                }
            }
        }

        // Define the object name(s).
        char * names[SNMP_MAX_CMDLINE_OIDS];
        int current_name { 0 };
        names [current_name++] = const_cast <char *> ("shortErrorStatus.0");

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
        constexpr int max_repeat = 10000000;
        for (int repeat = 0; repeat < max_repeat; repeat++) {
            cout << repeat << " ";

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

            //this_thread::sleep_for(chrono::milliseconds (100));
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
