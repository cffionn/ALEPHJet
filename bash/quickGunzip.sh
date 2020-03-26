#!/bin/bash

if [[ $# -ne 1 ]]
then
    echo "Usage: bash quickGunzip.sh <doCleaning>. exit 1"
    exit 1
fi

if [[ $1 -eq 1 ]]
then
    dummyVal=0
elif [[ $1 -eq 0 ]]
then
     dummyVal=0
else
    echo "$1 must be 1 or 0 <doCleaning>. exit 1"
    exit 1
fi

if [[ $ALEPHDATAPATH == *"/"* ]]
then
    echo "Using ALEPHDATAPATH '$ALEPHDATAPATH'"
else
    echo "ALEPHDATAPATH not set. source setEnv.sh. exit 1"
    exit 1
fi

count=$(ls $ALEPHDATAPATH/LEP1/1992/*gz | wc -l)

if [[ $count -lt 1 ]]
then
    echo "No *.gz files in $ALEPHDATAPATH/LEP1/1992/. exit 1"
    exit 1
fi

mkdir -p $ALEPHPROJPATH/local
rm -f $ALEPHPROJPATH/local/*.aleph
rm -f $ALEPHPROJPATH/local/*.gz

count=$(ls $ALEPHPROJPATH/local/* | wc -l)

if [[ $count -ge 1 ]]
then
    echo "$ALEPHPROJPATH/local/ is not empty. exit 1"
    exit 1
fi

for i in $ALEPHDATAPATH/LEP1/1992/*.gz
do
    cp $i $ALEPHPROJPATH/local
    break
done

gunzip $ALEPHPROJPATH/local/*.gz 

if [[ $1 -eq 0 ]]
then
    exit 0
fi

fileName=$(ls $ALEPHPROJPATH/local/)
fileName=$ALEPHPROJPATH/local/$fileName

cleanupStrings=("Primary vertex info flag" "px" "py" "pz" "m" "charge" "pwflag" "d0" "z0" "ntpc" "nitc" "nvdet" "END_EVENT" "vx" "vy" "ex" "ey" "ALEPH_DATA" "RUN" "EVENT" "ECM" "GEV" "=" "END_FILE")

for i in "${cleanupStrings[@]}"
do
    sed -i -e "s@$i@@g" $fileName
done

bool=$(grep "  " $fileName | wc -l)
pos=0
while [[ bool -gt 0 ]]
do   
    sed -i -e "s@  @ @g" $fileName
    bool=$(grep "  " $fileName | wc -l)
    pos=$((pos + 1))
done

sed -i -e "s@^ @@g" $fileName

