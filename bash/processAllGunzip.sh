#!/bin/bash

if [[ $# -ne 1 ]]
then
    echo "Usage: bash bash/processAllGunzup.sh <inDir>. exit 1"
    exit 1
fi

if [[ -d $1 ]]
then
    dummyVal=0
else
    echo "<inDir> \'$1\' given is not valid. exit 1"
    exit 1
fi
    

if [[ $ALEPHPROJPATH == *"/"* ]]
then
    echo "Using ALEPHPROJPATH '$ALEPHPROJPATH'"
else
    echo "ALEPHPROJPATH not set. source setEnv.sh. exit 1"
    exit 1
fi

mkdir -p $ALEPHPROJPATH/local

cleanupStrings=("Primary vertex info flag" "px" "py" "pz" "charge" "pwflag" "d0" "z0" "ntpc" "nitc" "nvdet" "END_EVENT" "vx" "vy" "ex" "ey" "ALEPH_DATA" "RUN" "EVENT" "ECM" "GEV" "=" "END_FILE" "m")

for i in $1/*
do
    if [[ -d $i ]]
    then
	bash $ALEPHPROJPATH/bash/processAllGunzip.sh $i
    elif [[ -f $i ]]
    then
	if [[ $i == *".gz" ]]
	then
	    fileName=$i
	    while [[ $fileName == *"/"* ]]
	    do
		fileName=${fileName#*/}
	    done

	    fileName=$ALEPHPROJPATH/local/$fileName	    
	    fileName2=${fileName%.gz}

	    if [[ -f $fileName2 ]]
	    then
		echo "Found $fileName2. continue..."
		continue
	    fi
	    
	    cp $i $ALEPHPROJPATH/local/
	    gunzip $fileName

	    for j in "${cleanupStrings[@]}"
	    do
		sed -i -e "s@$j@@g" $fileName2
	    done

	    bool=$(grep "  " $fileName2 | wc -l)
	    while [[ bool -gt 0 ]]
	    do
		sed -i -e "s@  @ @g" $fileName2
		bool=$(grep "  " $fileName2 | wc -l)
	    done

	    sed -i -e "s@^ @@g" $fileName2

#	    $ALEPHPROJPATH/bin/processCleanedAleph.exe $fileName &
#
#	    count=$(ps | grep processC | wc -l)
#	    while [[ $count -ge 9 ]]
#	    do
#		sleep 15
#		count=$(ps | grep processC | wc -l)
#	    done	    
	fi
    fi
done
    



