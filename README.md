
#Tallwide
A beaglebone service to drive TCL lighting with content from another computer. 

## Features
* Receives uncompressed bitmaps via oversized UDP packets
* Maps bitmap onto strands
* Allows for multiple zones of different strand winding
	* allows overlapping and/or disjoint zones
	* zones may be horizontal or vertical
	* zones may flip horizontally or vertically
		



#To Build

	local% scp * <ssh-identity>:udplights
	local% ssh <ssh-identity>
	beaglebone% cd udplights
	beaglebone% make tallwide
	