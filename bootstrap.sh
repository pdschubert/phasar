#!/bin/bash
set -e

NUM_THREADS=$(nproc)

# Parsing command-line-parameters
# See "https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash" as a reference

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -j|--jobs)
    NUM_THREADS="$2"
    shift # past argument
    shift # past value
    ;;
    -DBOOST_VERSION)
    DESIRED_BOOST_VERSION="$2"
    shift # past argument
    shift # past value
    ;;
    -DBOOST_VERSION=*)
    DESIRED_BOOST_VERSION="${key#*=}"
    shift # past argument=value
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

# End - Parsing command-line-parameters


echo "installing phasar dependencies..."

sudo apt-get update
sudo apt-get install zlib1g-dev sqlite3 libsqlite3-dev libmysqlcppconn-dev bear python3 doxygen graphviz python python-dev python3-pip python-pip libxml2 libxml2-dev libncurses5-dev libncursesw5-dev swig build-essential g++ cmake libz3-dev libedit-dev python-sphinx libomp-dev libcurl4-openssl-dev -y
sudo pip install Pygments
sudo pip install pyyaml
# installing boost
#wget https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
#tar xvf boost_1_66_0.tar.gz
#cd boost_1_66_0/
#./bootstrap.sh
#sudo ./b2 install
#cd ..

# New way of installing boost:

#TODO: Use DESIRED_BOOST_VERSION  as boost-version when installing boost 

BOOST_VERSION=$(echo -e '#include <boost/version.hpp>\nBOOST_LIB_VERSION' | gcc -s -x c++ -E - 2>/dev/null| grep "^[^#;]" | tr -d '\"') 

if [ -z $BOOST_VERSION ] ;then
    if [ -z $DESIRED_BOOST_VERSION ] ;then
        sudo apt-get install libboost-all-dev -y
    else
        # DESIRED_BOOST_VERSION in form d.d, i.e. 1.65 (this is the latest version I found in the apt repo)
        sudo apt-get install "libboost${DESIRED_BOOST_VERSION}-all-dev" -y
    fi
    #verify installation
    BOOST_VERSION=$(echo -e '#include <boost/version.hpp>\nBOOST_LIB_VERSION' | gcc -s -x c++ -E - 2>/dev/null| grep "^[^#;]" | tr -d '\"') 
    if [ -z $BOOST_VERSION ] ;then
        echo "Failed installing boost $DESIRED_BOOST_VERSION"
        exit 1
    else
        echo "Successfully installed boost v${BOOST_VERSION//_/.}"
    fi
else
    echo "Already installed boost version ${BOOST_VERSION//_/.}"
fi



# installing LLVM
./utils/install-llvm-8.0.0.sh $NUM_THREADS ./utils/
# installing wllvm
sudo pip3 install wllvm

echo "dependencies successfully installed"
echo "build phasar..."

#git submodule init
#git submodule update

export CC=/usr/local/bin/clang
export CXX=/usr/local/bin/clang++

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j $NUM_THREADS
echo "phasar successfully built"
echo "install phasar..."
sudo make install
sudo ldconfig
cd ..
echo "phasar successfully installed"