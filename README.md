Ring-Buffer
===========

A simple ring buffer (circular buffer) designed for embedded systems.

Building and Running Tests
---------------------------

```bash
mkdir -p build
cmake -DBUILD_TESTING=ON -S . -B build
cmake --build build --target all --target test
```

Please note that gtest needs to be installed in the system, what can be done
for example the following way:

```bash
git clone https://github.com/google/googletest.git

cd googletest
mkdir build && cd build

cmake .. -GNinja -DBUILD_SHARED_LIBS=OFF -DINSTALL_GTEST=ON \
    -DCMAKE_INSTALL_PREFIX:PATH=/usr
cmake --build .
sudo cmake --install .
sudo ldconfig
```
