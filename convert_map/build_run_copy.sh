cd build

make
echo "compile Done!"

./myConvertMap.app
echo "myConvertMap Done!"

cp settings.json ~/Documents/AirSim/settings.json
echo "copy Done!"

