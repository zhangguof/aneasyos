#ifndef _H_FAT_
#define _H_FAT_
#include "type.h"
//see https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system
#define PACKED_STRUC __attribute__((packed)) 


//Common structure of the first 25 bytes of the BIOS Parameter Block (BPB) used by FAT versions since DOS 2.0 
typedef struct PACKED_STRUC
{
	u16 sector_size;
	u8 cluster_size; //Logical sectors per cluster
	u16 reserved_sectors;
	u8 nfat;//Number of File Allocation Tables. Almost always 2;
	u16 max_root_dir;//Maximum number of FAT12 or FAT16 root directory entries. 0 for FAT32
	u16 n_sectors;//Total logical sectors 
	u8 fatid;//Media descriptor
	u16 fat_sector_size;//Logical sectors per File Allocation Table for FAT12/FAT16.
//DOS 3.31 BPB:
	u16 n_track;
	u16 n_heads;
	u32 n_hide_sectors;
	u32 n_sectors32;

}BPB;

//Extended BIOS Parameter Block
//Further structure used by FAT12 and FAT16 since OS/2 1.0 and DOS 4.0
typedef struct PACKED_STRUC
{
	BPB bpb;
	u8 phy_drive_num; //Physical drive number 
	u8 reserved;
	u8 ext_boot_sig;//Extended boot signature. 
	u32 vol_id; //Volume ID (serial number),Typically the serial number "xxxx-xxxx"
	char vol_label[11]; //Partition Volume Label
	char fs_type[8];//File system type
	
}EBPB;

//FAT32 Extended BIOS Parameter Block
//In essence FAT32 inserts 28 bytes into the EBPB
typedef struct PACKED_STRUC
{
	BPB bpb;
	u32 fat_sector_size;
	u16 drive_des;
	u16 version;
	u32 root_dir_start;//Cluster number of root directory start, typically 2 (first cluster)
	u16 n_fs_info;//Logical sector number of FS Information Sector, typically 1, 
//First logical sector number of a copy of the three FAT32 boot sectors,
	u16 copy_boot_sector;
	u8 reserved[12];
	//like ebpb
	u8 phy_drive_num;
	u8 fat_reserved;
	u8 ext_boot_sig;
	u32 vol_id;
	char vol_label[11];
	char fs_type[8];

	
}FAT32_EBPB;




//Boot Sector (VBR) is the first sector
typedef struct PACKED_STRUC
{
	u8 jmp_code[3];
	char oem_name[8];
	EBPB ebpb;
}VBR_16;

typedef struct PACKED_STRUC
{
	u8 jmp_code[3];
	char oem_name[8];
	FAT32_EBPB ebpb;
}VBR_32; 

void read_vbr_16();

#endif