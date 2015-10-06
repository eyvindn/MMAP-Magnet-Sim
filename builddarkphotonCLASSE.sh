#!/bin/bash                                                                     
# Run annihilation                                                              

source /opt/rh/devtoolset-3/enable
#source /nfs/opt/root/bin/thisroot.sh
export CXX="/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/gcc/4.9.1-cms/bin/c++"; export CC="/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/gcc/4.9.1-cms/bin/cc"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/gcc/4.9.1-cms/lib/:/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/gcc/4.9.1-cms/lib64/"
export G4LEDATA="/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/geant4-G4EMLOW/6.35/data/G4EMLOW6.35/"
export G4LEVELGAMMADATA="/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/geant4-G4PhotonEvaporation/3.0/data/PhotonEvaporation3.0/"
export G4SAIDXSDATA="/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/geant4-G4SAIDDATA/1.1/data/G4SAIDDATA1.1/"
export G4NEUTRONXSDATA="/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/geant4-G4NEUTRONXS/1.4/data/G4NEUTRONXS1.4/"
rm -rf darkPhotonBuild2/*
cd darkPhotonBuild2

# export G4LIB_USE_GDML=0
# export XERCESCROOT=/build/dmendezl/CMSSW_7_2_0_pre4-build/slc6_amd64_gcc491/external/xerces-c/2.8.0-cms
# export G4LIB_BUILD_SHARED=1

env Geant4_DIR=/home/een7/darkphotons_new/10.00.p03 \
cmake -DGeant4_BUILD_MULTITHREADED=OFF-DGEANT4_USE_GDML=OFF \
-DHEPMC_INCLUDE_DIR=/nfs/cms/mc1/cjc359/Dark-Photons/madgraph/hepmcbuild/include/ \
-DHEPMC_LIBRARIES=/nfs/cms/mc1/cjc359/Dark-Photons/madgraph/hepmcbuild/lib/libHepMC.so \
../darkPhoton2
#-DXERCESC_ROOT_DIR=/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/xerces-c/2.8.0-cms/ -DHEPMC_INCLUDE_DIR=../../madgraph/hepmcbuild/include/ -DXERCESC_INCLUDE_DIR=/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/xerces-c/2.8.0-cms/ -DXERCESC_LIBRARY=/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/xerces-c/2.8.0-cms/ -DH\
#cmake -DGEANT4_BUILD_MULTITHREADED=ON -DGeant4_DIR=/usr/local/geant4.10.00.p0lib64/Geant4-10.0.0 /home/local1/Dark-Photons/Cari_Code/darkPhoton             
make clean
make -j4
./darkPhoton

currentfolder=$(date +%Y:%m:%d#%H:%M:%S)
mkdir ../output/$currentfolder
mv *.root  ../output/$currentfolder
rm -rf *.root
cd ../output/$currentfolder
hadd complete.root *.root
mkdir threadfiles
mv *.root threadfiles
cd threadfiles
mv complete.root ../
