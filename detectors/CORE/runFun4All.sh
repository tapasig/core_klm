#!/bin/bash

# Change the material in G4_Magnet.C to Copper
#sed -i 's/\/\/string material = "Copper_Magnet";/string material = "Copper_Magnet";/' G4_Magnet.C &&
# Comment out aluminum
#sed -i 's/string material = "Al5083";/\/\/string material = "Al5083";/' G4_Magnet.C && 


# Run Fun4All
#root -l 'Fun4All_G4_CORE.C' &&

# cp out.root to ~/outCu.root
#cp out.root ~/outCu.root &&


# change the material in G4_Magnet.C to Aluminum
#sed -i 's/\/\/string material = "Al5083";/string material = "Al5083";/' G4_Magnet.C &&
# Comment out copper
#sed -i 's/string material = "Copper_Magnet";/\/\/string material = "Copper_Magnet";/' G4_Magnet.C &&

# Run Fun4All
#root -l 'Fun4All_G4_CORE.C' &&

# cp out.root to ~/outAl.root
#cp out.root ~/outAl.root &&

#clear &&
ls -lt ~
