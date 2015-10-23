#!/bin/bash
# Run annihilation

cd darkPhotonBuild2
rm -rf *
cmake -DGEANT4_BUILD_MULTITHREADED=OFF -DGeant4_DIR=/usr/local/geant4.10.00.p01/lib64/Geant4-10.0.0 -DHEPMC_INCLUDE_DIR=/media/sf_DarkCode/Dark-Photons/madgraph/hepmcbuild/include -DHEPMC_LIBRARIES=/media/sf_DarkCode/Dark-Photons/madgraph/hepmcbuild/lib/libHepMC.so ../darkPhoton2
#cmake -DGEANT4_BUILD_MULTITHREADED=ON -DGeant4_DIR=/usr/local/geant4.10.00.p01/lib64/Geant4-10.0.0 /home/local1/Dark-Photons/Cari_Code/darkPhoton
make clean
make -j4
./darkPhoton

#mkdir /home/local1/clusterData/backgrounds
#currentfolder=$(date +%Y:%m:%d#%H:%M:%S)
#mkdir /home/local1/clusterData/backgrounds/$currentfolder
#mv *.root /home/local1/clusterData/backgrounds/$currentfolder
#rm -rf *.root
#cd /home/local1/clusterData/backgrounds/$currentfolder
hadd complete.root *.root
mkdir threadfiles
mv *.root threadfiles
cd threadfiles
mv complete.root ../
