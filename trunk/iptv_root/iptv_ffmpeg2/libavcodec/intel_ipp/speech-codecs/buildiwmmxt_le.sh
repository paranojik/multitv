cpu=$1
cd ./codec/speech
./buildiwmmxt_le.sh 
cd ../../

cd ./application/usc_speech_codec
./buildiwmmxt_le.sh $cpu 
cd ../

cd  ./usc_nr
./buildiwmmxt_le.sh $cpu

cd  ../usc_tones
./buildiwmmxt_le.sh $cpu

cd ../../