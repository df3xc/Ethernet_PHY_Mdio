#ifndef __TJA_1100__
#define __TJA_1100__

#define BCR 0       /* Basic control register */
#define BSR 1       /* Basic status register */
#define PHY_ID1 2   /* 4.2.3  PHY IDENTIFIER 1 REGISTER */
#define PHY_ID2 3   /* 4.2.4  PHY IDENTIFIER 2 REGISTER */
#define ANEG 4      /* 4.2.5  AUTO NEGOTIATION ADVERTISEMENT REGISTER */
#define ANLP 5      /* 4.2.6  AUTO NEGOTIATION LINK PARTNER ABILITY REGISTER */
#define ANEX 6      /* 4.2.7  AUTO NEGOTIATION EXPANSION REGISTER */
#define MCS 17      /* 4.2.8  MODE CONTROL/STATUS REGISTER */
#define CS 23      /*  4      Communication status register */
#define PSC 31      /* 4.2.14 PHY SPECIAL CONTROL/STATUS REGISTER */


typedef struct
{

    int addr;      /*!< register address */
    char name[60]; /*!< register name */
}
phy_reg_struct;

#endif
