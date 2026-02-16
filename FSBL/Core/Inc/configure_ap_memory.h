 /* Aps256xx APMemory memory */

#ifndef CONFIGURE_APMEMORY_H
#define CONFIGURE_APMEMORY_H

  /* Read Operations */
#define READ_CMD                                0x00
#define READ_LINEAR_BURST_CMD                   0x20
#define READ_HYBRID_BURST_CMD                   0x3F

  /* Write Operations */
#define WRITE_CMD                               0x80
#define WRITE_LINEAR_BURST_CMD                  0xA0
#define WRITE_HYBRID_BURST_CMD                  0xBF

  /* Reset Operations */
#define RESET_CMD                               0xFF

  /* Registers definition */
#define MR0                                     0x00000000
#define MR1                                     0x00000001
#define MR2                                     0x00000002
#define MR3                                     0x00000003
#define MR4                                     0x00000004
#define MR8                                     0x00000008

  /* Register Operations */
#define READ_REG_CMD                            0x40
#define WRITE_REG_CMD                           0xC0

  /* Default dummy clocks cycles, 7(6+1) to support up to 200MHz CLK */
#define DUMMY_CLOCK_CYCLES_READ                 6
#define DUMMY_CLOCK_CYCLES_WRITE                6

  /* Size of buffers */
#define BUFFERSIZE                              10240
#define KByte                                   1024


void Configure_APMemory(void);

#endif /* CONFIGURE_APMEMORY_H */