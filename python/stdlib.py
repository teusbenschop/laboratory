#!/usr/bin/env python

# https://docs.python.org/3/tutorial/stdlib2.html

import logging

logging.debug('Debugging information')
logging.info('Informational message')
logging.warning('Warning:config file %s not found', 'server.conf')
logging.error('Error occurred')
logging.critical('Critical error -- shutting down')

print (1)

