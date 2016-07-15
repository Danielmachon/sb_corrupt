Small program to purposely corrupt the superblock (sb) magic number of a chosen device.
The corrupted partition can be repaired afterwards using fsck and friends.

Example:
	./sb -d /dev/mmcblk0p2
