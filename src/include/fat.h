#ifndef _H_FAT_
#define _H_FAT_
#include "type.h"
//see https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system
#define PACKED_STRUC __attribute__((packed)) 


//Common structure of the first 25 bytes of the BIOS Parameter Block (BPB) used by FAT versions since DOS 2.0 
#define DEF_STRUCT_BPB \
u16 sector_size; \
u8 cluster_size; \
u16 reserved_sectors; \
u8 nfat; \
u16 max_root_dir; \
u16 n_sectors; \
u8 fatid; \
u16 fat_sector_size; \
u16 n_track; \
u16 n_heads; \
u32 n_hide_sectors; \
u32 n_sectors32;


typedef struct PACKED_STRUC
{
	DEF_STRUCT_BPB
}BPB;



//Extended BIOS Parameter Block
//Further structure used by FAT12 and FAT16 since OS/2 1.0 and DOS 4.0

#define DEF_STRUCT_EBPB \
u8 phy_drive_num; \
u8 reserved; \
u8 ext_boot_sig;\
u32 vol_id;\
char vol_label[11]; \
char fs_type[8];


typedef struct PACKED_STRUC
{
	DEF_STRUCT_BPB
	DEF_STRUCT_EBPB
	
}EBPB;


	

//FAT32 Extended BIOS Parameter Block
//In essence FAT32 inserts 28 bytes into the EBPB

#define DEF_STRUCT_FAT32_EBPB \
u32 fat_sector_size32;\
u16 drive_des;\
u16 version;\
u32 root_dir_start;\
u16 n_fs_info;\
u16 copy_boot_sector;\
u8 reserved[12];\
u8 phy_drive_num; \
u8 fat_reserved; \
u8 ext_boot_sig;\
u32 vol_id;\
char vol_label[11];\
char fs_type[8];


typedef struct PACKED_STRUC
{
	DEF_STRUCT_BPB
	DEF_STRUCT_FAT32_EBPB
	
}FAT32_EBPB;






//Boot Sector (VBR) is the first sector
typedef struct PACKED_STRUC
{
	u8 jmp_code[3];
	char oem_name[8];
	union
	{
		EBPB ebpb;
		struct PACKED_STRUC
		{
			DEF_STRUCT_BPB
			DEF_STRUCT_EBPB
		};
	};
}VBR_16;

typedef struct PACKED_STRUC
{
	u8 jmp_code[3];
	char oem_name[8];
	union
	{
		EBPB ebpb;
		struct PACKED_STRUC
		{
			DEF_STRUCT_BPB
			DEF_STRUCT_FAT32_EBPB
		};
	};
}VBR_32; 


//Directory entry
typedef  struct PACKED_STRUC
{
	char short_file_name[11];
	u8 file_attr;
	u8 reserved[10];
	u16 mtime;
	u16 mdata;
	//Start of file in clusters in FAT12 and FAT16. Low two bytes of first cluster in FAT32; 
	u16 start_cluster;
	u32 file_size;
} DirectoryEntry;
/*
Byte Offset	Length (bytes)	Description
0x00	1	Sequence Number (bit 6: last logical, first physical LFN entry, bit 5: 0; bits 4-0: number 0x01..0x14 (0x1F), deleted entry: 0xE5)
0x01	10	Name characters (five UCS-2 characters)
0x0B	1	Attributes (always 0x0F)
0x0C	1	Type (always 0x00 for VFAT LFN, other values reserved for future use; for special usage of bits 4 and 3 in SFNs see further up)
0x0D	1	Checksum of DOS file name
0x0E	12	Name characters (six UCS-2 characters)
0x1A	2	First cluster (always 0x0000)
0x1C	4	Name characters (two UCS-2 characters)*/
typedef struct PACKED_STRUC
{
	u8 seq_num;
	char name[10];
	u8 attr;
	u8 type;
	u8 checksum;
	char name2[12];
	u16 start_cluster;
	char name3[2];
}LFN;


void read_vbr_16();

#endif