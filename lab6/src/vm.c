#include "vm.h"
#include "util.h"

void identity_mapping_init()
{
    write_sysreg(tcr_el1, TCR_CONFIG_DEFAULT);
    write_sysreg(mair_el1, MAIR_CONFIG_DEFAULT);

    memset(KERNEL_TT_L0, 0, 0x1000);
    memset(KERNEL_TT_L1, 0, 0x1000);

    KERNEL_TT_L0[0] = (unsigned long)KERNEL_TT_L1 | BOOT_PGD_ATTR;
    KERNEL_TT_L1[0] = 0x00000000 | BOOT_PUD_ATTR;
    KERNEL_TT_L1[1] = 0x40000000 | BOOT_PUD_ATTR;

    write_sysreg(ttbr0_el1, KERNEL_TT_L0);
    write_sysreg(ttbr1_el1, KERNEL_TT_L0);

    unsigned long sctlr = read_sysreg(sctlr_el1);
    write_sysreg(sctlr_el1, sctlr | MMU_ENABLE);
}
