#!/bin/bash    

if [[ $ALEPHPROJPATH == *"/"* ]]
then
    echo "Using ALEPHPROJPATH '$ALEPHPROJPATH'"
else
    echo "ALEPHPROJPATH not set. source setEnv.sh. exit 1"
    exit 1
fi

count=$(ls local/*.aleph | wc -l)
if [[ $count -ne 0 ]]
then
    dummyVal=0
else
    echo "local/*.aleph has no files. exit 1"
    exit 1
fi


for i in local/*.aleph
do
    ./bin/processCleanedAleph.exe $i &

    count=$(ps | grep processC | wc -l)

    while [[ $count -ge 10 ]]
    do
	sleep 1
	count=$(ps | grep processC | wc -l)	
    done
done
