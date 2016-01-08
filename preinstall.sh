#!/bin/sh

#download package talloc and go inside the directory and execute

sudo ./configure --prefix=/usr && sudo make 
sudo make install 
#go out from talloc directory
cd ..

#install the missing libosmocore package
sudo apt-get install build-essential libtool shtool autoconf automake git-core pkg-config make gcc 
sudo apt-get install libpcsclite-dev 
git clone git://git.osmocom.org/libosmocore.git 

cd libosmocore/
sudo autoreconf -i

sudo ./configure
sudo make
sudo make install
sudo ldconfig -i
cd ..

#install gr-gsm
sudo apt-get install git cmake libboost-all-dev libcppunit-dev swig doxygen liblog4cpp5-dev python-scipy
git clone https://github.com/ptrkrysik/gr-gsm.git
cd gr-gsm
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig

#install kalibrate hackrf
git clone https://github.com/scateu/kalibrate-hackrf.git
cd kalibrate-hackrf
./bootstrap
./configure
make
sudo make install

#make the file(install gksu for permission in the live boot) and enter the following lines
#[grc]
#local_blocks_path=/usr/local/share/gnuradio/grc/blocks

