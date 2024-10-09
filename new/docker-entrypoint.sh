#!/bin/bash

# Start the first process
cd /cdt && yarn browser start --hostname=0.0.0.0 &
cd /webadb && npm --prefix ./packages/webadb start & 
cd /site && npm start &
cd /service && ./WizardService
