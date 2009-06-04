compl=$1
cd ./codec/speech
./build64.sh $compl
cd ../../

cd ./application/usc_speech_codec
./build64.sh $compl
cd ../

cd  ./usc_ec
./build64.sh $compl

cd  ../usc_nr
./build64.sh $compl

cd  ../usc_tones
./build64.sh $compl

cd ../../