

    /*
     * Create PDU for GET request and add object names to request.
     */
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    for (count = 0; count < current_name; count++) {
        name_length = MAX_OID_LEN;
        if (!snmp_parse_oid(names[count], name, &name_length)) {
            snmp_perror(names[count]);
            failures++;
        } else
            snmp_add_null_var(pdu, name, name_length);
    }
    if (failures)
        goto close_session;

    exitval = 0;

    /*
     * Perform the request.
     *
     * If the Get Request fails, note the OID that caused the error,
     * "fix" the PDU (removing the error-prone OID) and retry.
     */
  retry:
    status = snmp_synch_response(ss, pdu, &response);
    if (status == STAT_SUCCESS) {
        if (response->errstat == SNMP_ERR_NOERROR) {
            for (vars = response->variables; vars;
                 vars = vars->next_variable)
                print_variable(vars->name, vars->name_length, vars);

        } else {
            fprintf(stderr, "Error in packet\nReason: %s\n",
                    snmp_errstring(response->errstat));

            if (response->errindex != 0) {
                fprintf(stderr, "Failed object: ");
                for (count = 1, vars = response->variables;
                     vars && count != response->errindex;
                     vars = vars->next_variable, count++)
                    /*EMPTY*/;
                if (vars) {
                    fprint_objid(stderr, vars->name, vars->name_length);
		}
                fprintf(stderr, "\n");
            }
            exitval = 2;

            /*
             * retry if the errored variable was successfully removed 
             */
            if (!netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
					NETSNMP_DS_APP_DONT_FIX_PDUS)) {
                pdu = snmp_fix_pdu(response, SNMP_MSG_GET);
                snmp_free_pdu(response);
                response = NULL;
                if (pdu != NULL) {
                    goto retry;
		}
            }
        }                       /* endif -- SNMP_ERR_NOERROR */

    } else if (status == STAT_TIMEOUT) {
        fprintf(stderr, "Timeout: No Response from %s.\n",
                session.peername);
        exitval = 1;

    } else {                    /* status == STAT_ERROR */
        snmp_sess_perror("snmpget", ss);
        exitval = 1;

    }                           /* endif -- STAT_SUCCESS */


    if (response)
        snmp_free_pdu(response);

close_session:
    snmp_close(ss);

}                               /* end main() */
