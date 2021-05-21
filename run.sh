#!/bin/bash

for i in /home/cfmcginn/Samples/LEP1Data1992_recons_aftercut-0*.aleph
do
    ./bin/processCleanedAleph.exe $i
done
