# build the doxygen documentation
cd doc/
doxygen
cd .. 

# extract number of cores on the system
CORES=`grep -c ^processor /proc/cpuinfo`
echo "cores: " $CORES

#CORES=1

# build the library and the tests
mkdir -p build
cd build/
cmake -D CMAKE_BUILD_TYPE=Release\
      -D CGAL_INC_DIR=$CGALINC\
      -D CGAL_LIB_DIR=$CGALLIB\
      ../
make -j$CORES
cd ..
