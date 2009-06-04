compl=$1
cd ./codec/speech
./buildem64t.sh $compl
cd ../../

cd ./application/usc_speech_codec
./buildem64t.sh $compl
cd ../

cd  ./usc_ec
./buildem64t.sh $compl

cd  ../usc_nr
./buildem64t.sh $compl

cd  ../usc_tones
./buildem64t.sh $compl

cd ../../