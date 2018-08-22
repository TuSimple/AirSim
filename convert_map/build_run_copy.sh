cd build

make
echo "compile Done!"

./myConvertMapLanes.app
echo "myConvertMapLanes Done!"

cp settings.json ~/Documents/AirSim/settings.json
echo "copy Done!"

