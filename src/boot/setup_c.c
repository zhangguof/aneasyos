
#include "elf.h"
#include "type.h"
#include "const.h"

#define VIDEO_ADDR (0x0b8000)
#define LINE_CHAR_NUM (80)
#define BITWIDTH (8)



typedef unsigned short u16;
typedef unsigned int u32;
u16 *p_gs_base = (u16*) VIDEO_ADDR; //显存地址

KERNEL_ENV g_kernel_env;
int g_mem_size;

int g_dwDispPos = (80 * 6 + 0);

//Address Range Descriptor Structure
//int dwMemSize = 0;

typedef struct
{
	u32 dwBaseAddrLow;
	u32 dwBaseAddrHigh;
	u32 dwLengthLow;
	u32 dwLengthHigh;
	u32 dwType;
}ARDStruct;

typedef union
{
	u32 pde;
	struct 
	{
		u32 P:1;  //Presend
		u32 RW:1;  //Read/write
		u32 US:1; //User/supervisor
		u32 PWT:1; //write-through
		u32 PCD:1; //Cache disable
		u32 A:1; //Accessed 
		u32 R:1;//Reserved (set to 0)
		u32 PS:1;//page size (0 is 4kb)
		u32 G:1; //Global page
		u32 Avail:3;//available for system programmer's use
		u32 base:20; //page table base address.
	}; 
}PageDirEntry;

typedef union
{
	u32 pte;
	struct 
	{
		u32 P:1;  //Presend
		u32 RW:1;  //Read/write
		u32 US:1; //User/supervisor
		u32 PWT:1; //write-through
		u32 PCD:1; //Cache disable
		u32 A:1; //Accessed 
		u32 D:1;//Dirty
		u32 PAT:1;//page table attribute index
		u32 G:1; //Global page
		u32 Avail:3;//available for system programmer's use
		u32 base:20; //page table base address.
	}; 
}PageTableEntry;





void SetCR_0_3();// start pageing 

void DispStr(char *s)
{
	u16 color = 0x0F;
	u16* p = p_gs_base + g_dwDispPos;
	while(*s)
	{
		char c = *s++;
		if(c=='\n')
		{
			int line_no = (p - p_gs_base)/LINE_CHAR_NUM;
			p = p_gs_base + (line_no+1)*LINE_CHAR_NUM;
		}
		else
		{
			*p++ = (u16)(c | (color<<8));
		}
	}
	g_dwDispPos = p - p_gs_base;
}

void DispReturn()
{
	DispStr("\n");
}

void DispInt(unsigned int a)
{
	char *p = "00000000h ";
	for(int i=0;i<BITWIDTH;++i)
	{
		int c = (a>>((BITWIDTH-1 - i)*4)) & (0xF);
		if(c >= 10)
		{
			p[i] = (char)('A'+(c-10));
		}
		else
		{
			p[i] = (char)('0'+c);
		}
	}
	DispStr(p);	
}

void* memcpy(void *p_dst, const void *p_src, u32 size)
{
	char* p_d = (char*) p_dst;
	char* p_src_end = ((char*)p_src + size);
	while (p_src!=p_src_end)
	{
		*p_d++ = *((char*)p_src++);
	}
	return p_dst;
}

u32 DispMemInfo(ARDStruct *p_buf,int McrNum)
{
	u32 max_mem_size = 0;
	DispStr("BaseAddrL BaseAddrH LengthLow LengthHigh   Type\n");
	for(int i=0;i<McrNum;++i)
	{
		ARDStruct *p = p_buf+i;
		u32 mem_size = p->dwBaseAddrLow + p->dwLengthLow;
		DispInt(p->dwBaseAddrLow);
		DispInt(p->dwBaseAddrHigh);
		DispInt(p->dwLengthLow);
		DispInt(p->dwLengthHigh);
		DispInt(p->dwType);
		DispReturn();
		if(p->dwType == 1 && mem_size > max_mem_size)
		{
			max_mem_size = mem_size;
		}
	}
	DispStr("RAM size:");
	DispInt(max_mem_size);
	DispReturn();
	g_mem_size = max_mem_size;
	return max_mem_size;
}



//启用分页机制
void setup_paging(PageDirEntry *PageDirBase,u32 mem_size)
{
	const int page_table_mem_size = 4*1024*1024; //2^22一个页表(page table)对应内存大小
	PageTableEntry* page_table_base = (PageTableEntry*) PageDirBase + 1024; //+4k,页表地址开始
	int page_table_cnt = mem_size / page_table_mem_size;
	if(mem_size % page_table_mem_size!=0)
	{
		page_table_cnt++;
	}
	//初始化pde(page dir entry)
	PageDirEntry pde = {0};
	pde.base = (u32)page_table_base >> 12;
	pde.P = 1;
	pde.RW = 1;
	pde.US = 1;
	for(int i=0;i<page_table_cnt;++i)
	{
		PageDirBase[i] = pde;
		pde.base = pde.base + 1; //一个目录对应页表个数*大小,4k*1024;
	}
	//初始化 pte(page table entry)
	PageTableEntry pte = {0};
	
	pte.US = 1;
	pte.P  = 1;
	pte.RW = 1;
	pte.base = 0;// 从0开始映射物理空间

	for(int i=0;i<page_table_cnt*1024;++i)
	{
		page_table_base[i] = pte;
		pte.base = pte.base + 1; //一页空间大小
	}

	SetCR_0_3();
	pte.base = 0;
	pte.P = 0;
	page_table_base[0] = pte; //第一页不可读写


}
typedef void (*Entry_func)(KERNEL_ENV* penv);

void init_kenle_env()
{
	g_kernel_env.mem_size = g_mem_size;
	g_kernel_env.drives = *(u8*)(BDA_DRIVES_ADD);

}

void init_kernel(Elf32_Ehdr* p_kernel_img)
{
	
	//void e_entry();
	
	u32 p_kernel_base = (u32) p_kernel_img;
	u32 phnum = (u32)p_kernel_img->e_phnum; //program header table entry count
	Entry_func e_entry = (Entry_func)(p_kernel_img->e_entry);
	
	u32 phentsize = (u32)p_kernel_img->e_phentsize; //Program header table entry size 
	
	//Program header table file offset
	Elf32_Phdr* e_phoff = (Elf32_Phdr *)(p_kernel_img->e_phoff + p_kernel_base);
	Elf32_Phdr* p_phdr;

	//从镜像中加载kenel到对应的地址
	for(u32 i=0;i<phnum;++i)
	{
		p_phdr = e_phoff + i;
		memcpy((void*)p_phdr->p_vaddr, 
			   (void*)(p_kernel_base + (u32)p_phdr->p_offset),
			   p_phdr->p_filesz
			   );
	}

	e_entry(&g_kernel_env); //进入内核
}

//======= for test =======
void print_mem(u32 *addr, u32 size)
{
	DispStr("mem addr:");
	DispInt((u32)addr);
	DispReturn();
	for(int i=0;i < size;++i)
	{
		if(i%4==0)
		{
			DispReturn();
		}
		DispInt(addr[i]);

	}
}

void print_hello()
{
	char p1[] = "aaaaaaaa\n";
	char p2[] = "bbbb\n";
	DispStr(p1);
	DispStr(p2);
	memcpy(p1,p2,sizeof(p2));
	DispStr(p1);

	// DispInt(0x1);
	// DispInt(0x0);
	// DispInt(0x2);
	// DispStr("Hello in C!!!\nTEST\nTset2\n");
	// DispInt(0x123456EF);
	// DispInt(0x123451FF);
	// DispReturn();
	// DispInt(0x123456FF);
	// DispStr("Hello in C2!!!\nTEST\nTset2\n");
}