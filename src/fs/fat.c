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

void get_83_name(char *buf,DirectoryEntry* pde,int is_dir)
{
	memcpy(buf, pde->short_file_name, 8);
	while(*buf!='\x20' && buf<buf+8) ++buf;
	if(is_dir)
	{
		*buf++ = '\0';
		return;
	}
	*buf++ = '.';
	memcpy(buf, pde->short_file_name+8, 3);
	while(*buf!='\x20' && buf<buf+3) ++buf;
	*buf = '\0';

}

void get_date(char*buf,DirectoryEntry* pde)
{
	// Hour	5 bits
	// Minutes	6 bits
	// Seconds	5 bits
	
	// Year	7 bits
	// Month	4 bits
	// Day	5 bit
	int hour;
	int min;
	int sec;
	int year;
	int mon;
	int day;
	u16 mdate = pde->mdata;
	u16 mtime = pde->mtime;
	hour = (mtime >> 11) & 0b011111;
	min = (mtime >> 5) & 0b0111111;
	sec = (mtime) & 0b011111;
	year = ((mdate>>9) & 0b01111111) + 1980;
	mon = (mdate>>5) & 0b01111;
	day = (mdate) & 0b011111;
	sprintf(buf, "%d-%d-%d %d:%d:%d", year,mon,day,hour,min,sec);
}

void read_fat(char* fat,VBR_16* vbr)
{
	const int partition_offest_sectors = 1;
	int first_fat_sector = partition_offest_sectors + vbr->reserved_sectors;
	int fat_size = vbr->fat_sector_size;
	for(int i=0;i<fat_size;++i)
	{
		hd_read_bytes(0, first_fat_sector+i, SECTOR_SIZE, fat);
		fat += SECTOR_SIZE;
	}
}

void print_dirs(DirectoryEntry* dirs,u32 count)
{
	//const int dir_entry_size = sizeof(DirectoryEntry);
	printf("dir count:%d\n",count);
	DirectoryEntry* p = dirs;

	for(int i=0;i<count;++i,p++)
	{
		LFN* lfn = (LFN*)p;
		char filename[50];
		if(lfn->seq_num == 0xE5)
		{
			printl("delete entry!!!\n");
			continue;
		}
		if(p->file_attr == 0xF)
		{
			get_long_name(filename, lfn);
			printf("first_byte:%x,LFN:%s,attr:%x,seq:%x\n",*(u8*)lfn,filename,lfn->attr,lfn->seq_num);
		}
		else
		{
			if(p->file_attr & ATTR_DIRECTORY)
			{
				get_83_name(filename, p, 1);
			}
			else
			{
				get_83_name(filename, p, 0);
			}
			
			printf("first_byte:%x,filename:%s,attr:%x,start:%d,size:%d\n",*(u8*)p,filename,p->file_attr,
				p->start_cluster,p->file_size
			);
			// char *buf = filename;
			// get_date(buf,p);
			// printf("mtime:%s\n",buf);
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
	// print_dir(first_data_sector - root_dir_sectors);
}

int check_dir_valid(DirectoryEntry* p)
{
	//1 file
	//2 dir
	//0 No valid.
	//-1 end mark.
	if(*(u8*)p == 0)
	{
		return -1;
	}
	if(*(u8*)p == 0xE5)
	{
		return 0;
	}
	if(p->file_attr & ATTR_DIRECTORY)
		return 2;
	return 1;
}

void get_root_dirs(DirectoryEntry *dir_entrys,u32 *entry_num, VBR_16* vbr)
{
	int max_root_dir = vbr->max_root_dir;
	const int partition_offest_sectors = 1;
	const int ENTRY_SIZE = sizeof(DirectoryEntry); //32 byte
	int fat_size = vbr->fat_sector_size; //fat32 for 0.
	int root_dir_sectors = (max_root_dir*32 + (vbr->sector_size-1))/vbr->sector_size;

	int first_data_sector = partition_offest_sectors + vbr->reserved_sectors + (vbr->nfat * fat_size) + root_dir_sectors;

	int root_start_sector = first_data_sector - root_dir_sectors;
	char hd_buf[SECTOR_SIZE];
	int end_mark = 0;
	*entry_num = 0;
	DirectoryEntry* p_dst_entry = dir_entrys;

	for(int i=0;i<root_dir_sectors && !end_mark;i++)
	{
		hd_read_bytes(0, root_start_sector+i, SECTOR_SIZE, hd_buf);
		DirectoryEntry* p = (DirectoryEntry*) hd_buf;
		for(int j=0;j<SECTOR_SIZE/ENTRY_SIZE;++j)
		{
			int check_mark = check_dir_valid(p+j);
			if(check_mark == -1)
			{
				end_mark = 1;
				break;
			}
			if(check_mark > 0)
			{
				*p_dst_entry++ = *(p+j);
			}
		}
	}
	*entry_num = p_dst_entry - dir_entrys;
}

u32 get_next_cluster(u32 cur_cluster,u16* fat)
{
	u16 mark = fat[cur_cluster];
	if(mark >= 0xFFF8)
	{
		printf("end mark:%x\n",mark);
		return 0;
	}
	return mark;
}

void read_cluster_chain(char* buf, u32 size,u32 start_cluster,VBR_16* vbr,u16* fat)
{
	const int partition_offest_sectors = 1;
	const int CLUSTER_SIZE = 4;
	const int ONE_CLUSTER_BYTES = CLUSTER_SIZE * SECTOR_SIZE;
	assert(CLUSTER_SIZE == vbr->cluster_size);
	
	int fat_size = vbr->fat_sector_size;
	int root_dir_sectors = (vbr->max_root_dir*32 + (vbr->sector_size-1))/vbr->sector_size;
	int first_data_sector = partition_offest_sectors + vbr->reserved_sectors + (vbr->nfat * fat_size) + root_dir_sectors;
	
	int clustes = (size+ONE_CLUSTER_BYTES-1)/ONE_CLUSTER_BYTES;
	printf("read:clustes:%d\n",clustes);
	
	u32 cur_cluster = start_cluster;
	u32 lba = first_data_sector + (cur_cluster-2)*CLUSTER_SIZE;
	while(clustes--){
		hd_read_bytes(0, lba, ONE_CLUSTER_BYTES, buf);
		printf("read offest:%d,%d\n",lba,lba*512);
		buf += ONE_CLUSTER_BYTES;
		cur_cluster = get_next_cluster(cur_cluster, fat);
		if(!cur_cluster)
		{
			break;
			printf("break!!!:%d\n",clustes);
		}
		lba = first_data_sector + (cur_cluster-2) * CLUSTER_SIZE;
	}
}

void get_parent_dir(DirectoryEntry* root_dirs, const char* filepath)
{

}

//filename = "/test/2.txt" || "/1.txt"
u32 read_file(const char* pfilename, char* buf, u32 count)
{
	u32 read_byte = 0;


	return 0;
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

	strncpy(oem_name,vbr.oem_name,sizeof(vbr.oem_name));
	strncpy(vol_label,vbr.vol_label,sizeof(vbr.vol_label));
	strncpy(fs_type,vbr.fs_type,sizeof(vbr.fs_type));


	printf("oem:%s,dri_num:%d,res:%d,sig:%d,vol_id:%x,label:%s,fs_type:%s\n", \
		oem_name,vbr.phy_drive_num,vbr.reserved, \
		vbr.ext_boot_sig,vbr.vol_id,vol_label,fs_type
		);
	printf("FAT_SIZE:%d,bytes:%d,cluster_size:%d\n",
		vbr.fat_sector_size,vbr.fat_sector_size*SECTOR_SIZE,vbr.cluster_size);

	//print_vbr(&vbr);
	DirectoryEntry dir_entrys[128];
	int dir_count = 0;
	get_root_dirs(dir_entrys, &dir_count, &vbr);
	print_dirs(dir_entrys,dir_count);
	char file_buf[7925];
	u16 fat[50000];
	read_fat((char*)fat,&vbr);
	print_buf(fat,4*10);
	read_cluster_chain(file_buf, 7925, 23, &vbr, fat);
	file_buf[7925] = '\0';
	printf("%s\n",file_buf+7925-10);



	

}


