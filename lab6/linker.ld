_svkernel = 0xffff000000080000;
_evkernel = 0xffff000000400000;

_skernel = 0x80000;
_ekernel = 0x400000;

SECTIONS
{
    . = _svkernel;

    _start = .;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.*) }
    .bss : {
        . = ALIGN(16);
        _bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        _bss_end = .;
    }
    _end =.;
    . = _evkernel;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}

_bss_size = 1;
_devicetree = 0x200000 + _skernel;
/*(_bss_end - _bss_start)>>3;*/