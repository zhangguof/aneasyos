#include "MyOs.h"


static void	init_hd();
static void	hd_cmd_out(struct hd_cmd* cmd);
static int	waitfor(int mask, int val, int timeout);
static void	interrupt_wait();
static void	hd_identify(int drive);
static void	print_identify_info(u16* hdinfo);

static	u8	hd_status;
static	u8	hdbuf[SECTOR_SIZE * 2];



static void init_hd()
{
    //get the num of driver from the bios date area

    u8 NrDrives = kernel_env.drives;
    printf("NrDrives:%d.\n",NrDrives);
    assert(NrDrives);
    put_irq_hangler(AT_WINI_IRQ,hd_handler);
    enable_irq(CASCADE_IRQ);
    enable_irq(AT_WINI_IRQ);
}

void hd_identify(int driver)
{
    struct hd_cmd cmd;

    cmd.device = MAKE_DEVICE_REG(0, driver,0); //master driver
    cmd.command = ATA_IDENTIFY;
    hd_cmd_out(&cmd);
    interrupt_wait();
    port_read(REG_DATA, hdbuf, SECTOR_SIZE);
    print_identify_info((u16*)hdbuf);

}

void print_identify_info(u16 *hdinfo)
{
    int i,k;
    char s[64];

    struct iden_info_ascii
    {
        int idx;
        int len;
        char *desc;
    }iinfo[] = {{10, 20, "HD SN"}, /* Serial number in ASCII */
		     {27, 40, "HD Model"} /* Model number in ASCII */ };
    int capabilities = hdinfo[49];
	printf("LBA supported: %s\n",
	       (capabilities & 0x0200) ? "Yes" : "No");

	int cmd_set_supported = hdinfo[83];
	printf("LBA48 supported: %s\n",
	       (cmd_set_supported & 0x0400) ? "Yes" : "No");

	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
	printf("HD size: %dMB\n", sectors * 512 / 1000000);

}

void hd_cmd_out(struct hd_cmd* cmd)
{
    if(!waitfor(STATUS_BSY, 0, HD_TIMEOUT))
        panic("hd error");

  /* Activate the Interrupt Enable (nIEN) bit */
	out_byte(REG_DEV_CTRL, 0);
	/* Load required parameters in the Command Block Registers */
	out_byte(REG_FEATURES, cmd->features);
	out_byte(REG_NSECTOR,  cmd->count);
	out_byte(REG_LBA_LOW,  cmd->lba_low);
	out_byte(REG_LBA_MID,  cmd->lba_mid);
	out_byte(REG_LBA_HIGH, cmd->lba_high);
	out_byte(REG_DEVICE,   cmd->device);
	/* Write the command code to the Command Register */
	out_byte(REG_CMD,     cmd->command);
}

void interrupt_wait()
{
    //wait until a disk interrupt occurs
    MESSAGE msg;
    send_rec(RECEIVE, INTERRUPT, &msg);
}

/************************************************
*  waitfor
* wait for a certain status
 @param mask  status mask
 @param required status
 @param timeout timeout in milliseconds

 @return one if sucess,zero if timeout
*************************************************/
int waitfor(int mask, int val, int timeout)
{
    int t=get_ticks();
    while(((get_ticks() - t) * 1000 / HZ) < timeout)
    {
        if((in_byte(REG_STATUS) & mask) == val)
            return 1;
    }
    return 0;
}

int read_ata_st()
{

}

int read_ata_lab28()
{

}

int hd_read_bytes(int driver,u32 lba,u32 byte_cnt,char data[])
{
    /*
        1.test state.if busy?
    */
    assert(driver==0 || driver == 1);
    struct hd_cmd cmd;
    int sectors_cnt = byte_cnt/SECTOR_SIZE;
    char *p = data;
    if(byte_cnt%SECTOR_SIZE!=0)
        sectors_cnt++;
    cmd.device = MAKE_DEVICE_REG(1,driver,(0x0F&(lba>>24)));
    cmd.lba_low = (lba & 0xff);
    cmd.lba_mid = ((lba>>8) & 0xff);
    cmd.lba_high = ((lba>>16) & 0xff);
    cmd.count = sectors_cnt;
    cmd.command = ATA_READ;
    hd_cmd_out(&cmd);

    printf("lba:%d,byty cnt:%d\n", lba,byte_cnt);
    u32 byte_to_read = byte_cnt;
    while (byte_to_read)
    {
        u32 size = byte_to_read > SECTOR_SIZE?SECTOR_SIZE:byte_to_read;
        //printf("befor interrupt!!:%d\n",size);
        interrupt_wait();
        //printf("after interrupt!!\n");
        port_read(REG_DATA, hdbuf, SECTOR_SIZE);
        memcpy(p,hdbuf,size);
        p+=size;
        byte_to_read -= size;
    }
    //test
    // u32 *pi = (u32*)data;
    // for(int i=0;i<byte_cnt/4;++i)
    // {
    //     printf("%x ", pi[i]);
    //     if(i && i%4==0)
    //         printf("\n");
    // }
}

void hd_read_MBR(int driver, struct part_ent partition_table[])
{
    //PARTITION_TABLE_OFFSET=0x1be
    char buf[1024];
    const int part_size = 4;
    hd_read_bytes(driver,0,SECTOR_SIZE,buf);//lba=0,MBR
    for(int i=0;i<part_size;++i)
    {
        partition_table[i] = *(struct part_ent*)(buf+PARTITION_TABLE_OFFSET+i*sizeof(struct part_ent));
        // printf("part_ent:%d,boot_ind:%d,sys_id:%d,start_sect:%d,cnt:%d\n", \
        //         i,partition_table[i].boot_ind,partition_table[i].sys_id,
        //         partition_table[i].start_sect,partition_table[i].nr_sects
        //         );
    }

}

/************************************************
*      hd_handler
*  ring0 interrupt handler
************************************************/
void hd_handler(int irq)
{
    hd_status = in_byte(REG_STATUS);
    inform_int(TASK_HD);
}

//main loop of hd driver
void task_hd()
{
    printf("init task_hd: ticks:%d\n", get_ticks());
    MESSAGE msg;
    //char buf[2048];
    struct part_ent partition_table[4];
    
    init_hd();
    while(1)
    {
        send_rec(RECEIVE, ANY, &msg);
        int src = msg.source;
        switch(msg.type)
        {
        case DEV_OPEN:
             hd_identify(0);  //something erro with usb boot
             //read_bytes(0,0,1024,buf);
             //hd_read_MBR(0,partition_table);
             read_vbr_16();
             break;
        default:
             panic("unknow msg");
             break;
        }
        send_rec(SEND, src, &msg);
    }

}
