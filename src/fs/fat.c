#include "MyOs.h"
//implent file system.

int hd_read_bytes(int driver,u32 lba,u32 byte_cnt,char data[]);
void hd_read_MBR(int driver, struct part_ent partition_table[]);

void print_bpb(BPB *bpb)
{
	//printf("%x\n", bpb->sector_size);
	printf("sec_size:%d,clu_size:%d,reserved_sec:%d,nfat:%d,max_root_dir:%d" \
		   "n_secs:%d,fatid:%d,fat_sec_size:%d,n_trac:%d,n_heads:%d,n_h_sed:%d,"\
		   "n_sectors32:%d\n", \
		   bpb->sector_size,bpb->cluster_size,bpb->reserved_sectors,
		   bpb->nfat,bpb->max_root_dir,bpb->n_sectors,bpb->fatid,
		   bpb->fat_sector_size,bpb->n_track,bpb->n_heads,bpb->n_hide_sectors,
		   bpb->n_sectors32
		   );
}

void read_vbr_16()
{
	VBR_16 vbr;
	char buf[512];
	char oem_name[9];
	char vol_label[11+1];
	char fs_type[8+1];

	struct part_ent partition_table[4];
	hd_read_MBR(0,partition_table);
	int part_start_sec = partition_table[0].start_sect;
	hd_read_bytes(0,part_start_sec,SECTOR_SIZE,buf); //read firt secort of fat partition.
	vbr = *(VBR_16*)(buf);
	memcpy(oem_name,vbr.oem_name,8);
	oem_name[8]='\0';

	memcpy(vol_label,vbr.ebpb.vol_label,11);
	vol_label[11] = '\0';

	memcpy(fs_type,vbr.ebpb.fs_type,8);
	fs_type[8]= '\0';

	// print_buf((void*)buf,100);
	// printf("===========\n");
	// print_buf((void*)&vbr,100);

	print_bpb(&(vbr.ebpb.bpb));

	printf("oem:%s,dri_num:%d,res:%d,sig:%d,vol_id:%x,label:%s,fs_type:%s\n", \
		oem_name,vbr.ebpb.phy_drive_num,vbr.ebpb.reserved, \
		vbr.ebpb.ext_boot_sig,vbr.ebpb.vol_id,vol_label,fs_type
		);
}


