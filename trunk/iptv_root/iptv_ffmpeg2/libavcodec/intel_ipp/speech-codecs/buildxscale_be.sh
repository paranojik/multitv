cpu=$1
cd ./codec/speech
./buildxscale_be.sh
cd ../../

cd ./application/usc_speech_codec
./buildxscale_be.sh $cpu 
cd ../

cd  ./usc_nr
./buildxscale_be.sh $cpu 

cd  ../usc_tones
./buildxscale_be.sh $cpu 

cd ../../