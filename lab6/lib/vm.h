#define PHYS_OFFSET 0xffff000000000000

#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) | (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1
#define MAIR_CONFIG_DEFAULT ((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_PAGE 0b11
#define PD_ACCESS (1 << 10)
#define PD_USER_RW (0b01 << 6)
#define PD_USER_R (0b11 << 6)
#define MMU_ENABLE 1
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

static inline void *pa2va(unsigned long p)
{
    return (void *)(p + PHYS_OFFSET);
}

static inline unsigned long va2pa(void *p)
{
    return (unsigned long)p - PHYS_OFFSET;
}

#define KERNEL_TT_L0 ((unsigned long *)0x1000)
#define KERNEL_TT_L1 ((unsigned long *)0x2000)
#define KERNEL_TT_L0_VA ((unsigned long *)pa2va(0x1000))
#define KERNEL_TT_L1_VA ((unsigned long *)pa2va(0x2000))

void identity_mapping_init();