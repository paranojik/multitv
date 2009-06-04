compl=$1
cd ./codec/speech
./build32.sh $compl
cd ../../

cd ./application/usc_speech_codec
./build32.sh $compl
cd ../

cd  ./usc_ec
./build32.sh $compl

cd  ../usc_nr
./build32.sh $compl

cd  ../usc_tones
./build32.sh $compl

cd ../../