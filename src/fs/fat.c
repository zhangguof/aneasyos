#include "MyOs.h"
#include "fat.h"
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
/*
MBR 1 sector
VBR 1 sector
FAT
FAT
root dir entrys
*/

void char2wchar(char* buf,char* src_buf,int src_size)
{
	u16* p = (u16*)src_buf;
	for(int i=0;i<src_size/2;++i,++buf)
	{
		if(*p < 0xFF)
		{
			*buf = (char)(p[i]);
		}
		else
		{
			*buf = '*';
		}
	}
}

void get_long_name(char *buf,LFN* lfn)
{
	int size = sizeof(lfn->name);
	char2wchar(buf, lfn->name, size);

	buf+=size/2;
	size = sizeof(lfn->name2);
	char2wchar(buf, lfn->name2, size);
	
	buf+=size/2;
	size = sizeof(lfn->name3);
	char2wchar(buf, lfn->name3, size);
	buf+=size/2;
	*buf = '\0';
}

void get_83_name(char *buf,DirectoryEntry* pde)
{
	char *t = buf;
	memcpy(buf, pde->short_file_name, 8);
	while(*buf!='\x20' && buf<buf+8) ++buf;
	*buf++ = '.';
	memcpy(buf, pde->short_file_name+8, 3);
	while(*buf!='\x20' && buf<buf+3) ++buf;
	*buf = '\0';
	printf("test::::%s\n",t);
	print_buf(t, 12);
	printf("\nxxxx:%s\n","1.TXT\0\x20\x20");

}

void print_dir(int root_dir_sector)
{
	int offest = root_dir_sector * 512;
	//const int dir_entry_size = sizeof(DirectoryEntry);
	char buf[SECTOR_SIZE];
	hd_read_bytes(0, root_dir_sector, SECTOR_SIZE, buf);
	DirectoryEntry dir_entry[10];
	DirectoryEntry *p = (DirectoryEntry*)buf;
	printf("root,start:%d\n",root_dir_sector*512);
	print_buf(buf, 120);
	for(int i=0;i<10;++i,p++)
	{
		dir_entry[i] = *p;
		char filename[30];
		if(p->file_attr == 0xF)
		{
			LFN* lfn = (LFN*)p;
			if(lfn->seq_num == 0xE5)
			{
				printl("delete entry!!!\n");
				continue;
			}
			get_long_name(filename, lfn);
			printf("LFN:%s,attr:%x,seq:%x\n",filename,lfn->attr,lfn->seq_num);
		}
		else
		{
			get_83_name(filename, p);
			printf("filename:%s,attr:%x,start:%d,size:%d\n",filename,dir_entry[i].file_attr,
				dir_entry[i].start_cluster,dir_entry[i].file_size
			);
		}
		
	}
}

void print_vbr(VBR_16* vbr)
{
	const int partition_offest_sectors = 1;
	int total_sectors = vbr->n_sectors==0?vbr->n_sectors32:vbr->n_sectors;
	//fat_size = (fat_boot->table_size_16 == 0)? fat_boot_ext_32->table_size_16 : fat_boot->table_size_16;
	int fat_size = vbr->fat_sector_size; //fat32 for 0.
	//root_dir_sectors = ((fat_boot->root_entry_count * 32) + (fat_boot->bytes_per_sector - 1)) / fat_boot->bytes_per_sector;
	int root_entry_count = vbr->max_root_dir;
	int root_dir_sectors = (root_entry_count*32 + (vbr->sector_size-1))/vbr->sector_size;
	//first_data_sector = fat_boot->reserved_sector_count + (fat_boot->table_count * fat_size) + root_dir_sectors;
	int first_data_sector = partition_offest_sectors + vbr->reserved_sectors + (vbr->nfat * fat_size) + root_dir_sectors;

	//first_fat_sector = fat_boot->reserved_sector_count;
	int first_fat_sector = partition_offest_sectors+vbr->reserved_sectors;
	//data_sectors = fat_boot->total_sectors - (fat_boot->reserved_sector_count + (fat_boot->table_count * fat_size) + root_dir_sectors);
	int data_sectors = total_sectors - vbr->reserved_sectors - vbr->nfat * fat_size - root_dir_sectors;
	
	//total_clusters = data_sectors / fat_boot->sectors_per_cluster;
	int total_clusters = data_sectors / vbr->cluster_size;

	printf("total_sectors:%d,fat_size:%d,root_entry_count:%d,root_dir_sectors:%d,first_data_sector:%d,"
		   "first_fat_sector:%d,data_sectors:%d\n",total_sectors,fat_size,root_entry_count,root_dir_sectors,
		   first_data_sector,first_fat_sector,data_sectors
		   );
	printf("total_clusters:%d\n",total_clusters);
	printf("first_data_offset:%d\n",first_data_sector*512);
	printf("first_fat_sector:%d\n",first_fat_sector*512);
	print_dir(first_data_sector - root_dir_sectors);
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
	printf("partition_id:%x\n", partition_table[0].sys_id);
	hd_read_bytes(0,part_start_sec,SECTOR_SIZE,buf); //read firt secort of fat partition.
	vbr = *(VBR_16*)(buf);

	memcpy(oem_name,vbr.oem_name,8);
	oem_name[8]='\0';

	memcpy(vol_label,vbr.vol_label,11);
	vol_label[11] = '\0';

	memcpy(fs_type,vbr.fs_type,8);
	fs_type[8]= '\0';


	//print_bpb((BPB*)&(vbr.ebpb));
	print_vbr(&vbr);

	printf("oem:%s,dri_num:%d,res:%d,sig:%d,vol_id:%x,label:%s,fs_type:%s\n", \
		oem_name,vbr.phy_drive_num,vbr.reserved, \
		vbr.ext_boot_sig,vbr.vol_id,vol_label,fs_type
		);
}


