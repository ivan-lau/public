
sed -i 's/\\(ABSL_OPTION_USE_STD_[a-zA-Z0-9_]\*\\) 2/\1 1/g' absl/base/options.h
? -DABSL_INTERNAL_AT_LEAST_CXX17=ON ?

mkdir build;cd build

export CC=gcc;export CXX=g++

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=NO -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_CXX_STANDARD=17 -DABSL_PROPAGATE_CXX_STD=ON ..

make -j $(nproc)

make DESTDIR=./output install

grep -nr define\ ABSL_LTS_RELEASE_* output/usr/local/include/absl/base/config.h


### Enable Sanitizer
```
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")

cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=NO -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_CXX_STANDARD=17 -DABSL_PROPAGATE_CXX_STD=ON ..
```
