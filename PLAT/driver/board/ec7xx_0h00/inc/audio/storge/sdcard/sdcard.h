#ifndef __SD_CARD_H__
#define __SD_CARD_H__
 

#define SD_BLOCK_SIZE   512

#define SD_OK           0
#define SD_ERROR        1

#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06

#define CMD0    (0)             /* GO_IDLE_STATE */
#define CMD1    (1)             /* SEND_OP_COND (MMC) */
#define ACMD41  (0x80 + 41)     /* SEND_OP_COND (SDC) */
#define CMD8    (8)             /* SEND_IF_COND */
#define CMD9    (9)             /* SEND_CSD */
#define CMD10   (10)            /* SEND_CID */
#define CMD12   (12)            /* STOP_TRANSMISSION */
#define ACMD13  (0x80 + 13)     /* SD_STATUS (SDC) */
#define CMD16   (16)            /* SET_BLOCKLEN */
#define CMD17   (17)            /* READ_SINGLE_BLOCK */
#define CMD18   (18)            /* READ_MULTIPLE_BLOCK */
#define CMD23   (23)            /* SET_BLOCK_COUNT (MMC) */
#define ACMD23  (0x80 + 23)     /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24   (24)            /* WRITE_BLOCK */
#define CMD25   (25)            /* WRITE_MULTIPLE_BLOCK */
#define CMD32   (32)            /* ERASE_ER_BLK_START */
#define CMD33   (33)            /* ERASE_ER_BLK_END */
#define CMD38   (38)            /* ERASE */
#define CMD55   (55)            /* APP_CMD */
#define CMD58   (58)            /* READ_OCR */


uint8_t  sdInit(void);
uint8_t sdGetCsd(uint8_t *csd_data);
uint8_t sdGetCid(uint8_t *cid_data);
uint8_t  sdReadDisk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);
uint8_t  sdWriteDisk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);
uint32_t sdGetBlockCount(void);


#endif
