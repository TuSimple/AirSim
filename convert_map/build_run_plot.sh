cd build

make
echo "compile Done!"

./myConvertMap.app
echo "myConvertMap Done!"

# cp settings.json ~/Documents/AirSim/settings.json
echo "plotting..."

cd ..
python plot_bounds.py build/bounds.txt

echo "plot_bounds Done!"

