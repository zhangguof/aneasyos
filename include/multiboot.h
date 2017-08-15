#ifndef _multiboot_h_
#define _multiboot_h_


//用于使用multiboot的头文件
/* 多启动信息结构 */
typedef unsigned short		multiboot_uint16_t;
typedef unsigned int		multiboot_uint32_t;
typedef unsigned long long	multiboot_uint64_t;

/* Check if the bit BIT in FLAGS is set.  */
#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))

/* is there basic lower/upper memory information? */
#define MULTIBOOT_INFO_MEMORY			0x00000001
/* is there a boot device set? */
#define MULTIBOOT_INFO_BOOTDEV			0x00000002
/* is the command-line defined? */
#define MULTIBOOT_INFO_CMDLINE			0x00000004
/* are there modules to do something with? */
#define MULTIBOOT_INFO_MODS			0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MULTIBOOT_INFO_AOUT_SYMS		0x00000010
/* is there an ELF section header table? */
#define MULTIBOOT_INFO_ELF_SHDR			0X00000020

/* is there a full memory map? */
#define MULTIBOOT_INFO_MEM_MAP			0x00000040

/* Is there drive info?  */
#define MULTIBOOT_INFO_DRIVE_INFO		0x00000080

/* Is there a config table?  */
#define MULTIBOOT_INFO_CONFIG_TABLE		0x00000100

/* Is there a boot loader name?  */
#define MULTIBOOT_INFO_BOOT_LOADER_NAME		0x00000200

/* Is there a APM table?  */
#define MULTIBOOT_INFO_APM_TABLE		0x00000400

/* Is there video information?  */
#define MULTIBOOT_INFO_VIDEO_INFO		0x00000800

/* The symbol table for a.out.  */
struct multiboot_aout_symbol_table
{
    multiboot_uint32_t tabsize;
    multiboot_uint32_t strsize;
    multiboot_uint32_t addr;
    multiboot_uint32_t reserved;
};
typedef struct multiboot_aout_symbol_table multiboot_aout_symbol_table_t;
/* The section header table for ELF.  */
struct multiboot_elf_section_header_table
{
  multiboot_uint32_t num;
  multiboot_uint32_t size;
  multiboot_uint32_t addr;
  multiboot_uint32_t shndx;
};
typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;

typedef struct multiboot_info
{
    unsigned long flags;
    unsigned long mem_lower;
    unsigned long mem_upper;
    unsigned long boot_device;
    unsigned long cmdline;
    unsigned long mods_count;
    unsigned long mods_addr;
    union
    {
    multiboot_aout_symbol_table_t aout_sym;
    multiboot_elf_section_header_table_t elf_sec;
    } u;
    unsigned long mmap_length;
    unsigned long mmap_addr;
} multiboot_info_t;






#endif
