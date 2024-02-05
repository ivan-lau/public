```
sed -i 's/\(ABSL_OPTION_USE_STD_[a-zA-Z0-9_]*\) 2/\1 1/g' absl/base/options.h

mkdir build;cd build

export CC=gcc;export CXX=g++

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=NO -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_CXX_STANDARD=17 -DABSL_PROPAGATE_CXX_STD=ON ..

make -j $(nproc)

make DESTDIR=./output install

grep -nr define\ ABSL_LTS_RELEASE_VERSION output/usr/local/include/absl/base/config.h
```
