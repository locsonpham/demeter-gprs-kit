#include "enc28j60.h"
static uint8_t Enc28j60Bank;
static uint16_t NextPacketPtr;
uint8_t SPI_ReadWrite(uint8_t data)
{
   uint8_t bit_ctr;
   SCK=0;
      for(bit_ctr=0;bit_ctr<8;bit_ctr++) // Truyen 8 bit du lieu
      {
      if(data & 0x80)
      MOSI =1;                               // truyen du lieu bit MSB truyen qua MOSI 
      else MOSI=0;
      data = (data << 1);               // dich trai du lieu..
      //delay_us(1);
      SCK = 1;                              //Thiet lap muc cao cho chan  SCK          
      if(MISO==1)
      data |=0x01;                      // Lay gia tri bit hien tai cua day MISO
      //delay_us(1);
      SCK = 0;                           // Thiet lap muc thap cho chan SCK
      }
    return(data);                   // Tra ve gia tri du lieu nhan duoc
}
uint8_t ENC28J60_ReadOp(uint8_t op, uint8_t address)
   {
   uint8_t dat = 0;
   
   ENC28J60_CS=0;
   dat = op | (address & ADDR_MASK);
   SPI_ReadWrite(dat);
   dat = SPI_ReadWrite(0xFF);
   // do dummy read if needed (for mac and mii, see datasheet page 29)
   if(address & 0x80)
       {
      dat = SPI_ReadWrite(0xFF);
       }
   // release CS
   ENC28J60_CS=1;
   return dat;
   }

void ENC28J60_WriteOp(uint8_t op, uint8_t address, uint8_t data)
   {
   uint8_t dat = 0;
     
   ENC28J60_CS=0;
   // issue write command
   dat = op | (address & ADDR_MASK);
   SPI_ReadWrite(dat);
   // write data
   dat = data;
   SPI_ReadWrite(dat);
   ENC28J60_CS=1;
   }

void ENC28J60_ReadBuffer(uint16_t len, uint8_t* data)
   {
   ENC28J60_CS=0;
   // issue read command
   SPI_ReadWrite(ENC28J60_READ_BUF_MEM);
   while(len)
      {
        len--;
        // read data
        *data = (uint8_t)SPI_ReadWrite(0);
        data++;
      }
   *data='\0';
   ENC28J60_CS=1;
   }

void ENC28J60_WriteBuffer(uint16_t len, uint8_t* data)
   {
   ENC28J60_CS=0;
   // issue write command
   SPI_ReadWrite(ENC28J60_WRITE_BUF_MEM);
   while(len)
      {
      len--;
      SPI_ReadWrite(*data);
      data++;
      }
   ENC28J60_CS=1;
   }

void ENC28J60_SetBank(uint8_t address)
   {
   // set the bank (if needed)
   if((address & BANK_MASK) != Enc28j60Bank)
      {
        // set the bank
        ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
        ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
        Enc28j60Bank = (address & BANK_MASK);
      }
   }

uint8_t ENC28J60_Read(uint8_t address)
   {
   // set the bank
   ENC28J60_SetBank(address);
   // do the read
   return ENC28J60_ReadOp(ENC28J60_READ_CTRL_REG, address);
   }

void ENC28J60_Write(uint8_t address, uint8_t data)
   {
   // set the bank
   ENC28J60_SetBank(address);
   // do the write
   ENC28J60_WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
   }

void ENC28J60_PhyWrite(uint8_t address, uint16_t data)
   {
   // set the PHY register address
   ENC28J60_Write(MIREGADR, address);
   // write the PHY data
   ENC28J60_Write(MIWRL, data);
   ENC28J60_Write(MIWRH, data>>8);
   // wait until the PHY write completes
   while(ENC28J60_Read(MISTAT) & MISTAT_BUSY)
      {
      }
   }

void ENC28J60_ClkOut(uint8_t clk)
   {
    //setup clkout: 2 is 12.5MHz:
   ENC28J60_Write(ECOCON, clk & 0x7);
   }
void ENC28J60_SetMac(uint8_t *macaddr)
   {
   ENC28J60_Write(MAADR5, macaddr[0]);   
   ENC28J60_Write(MAADR4, macaddr[1]);
   ENC28J60_Write(MAADR3, macaddr[2]);
   ENC28J60_Write(MAADR2, macaddr[3]);
   ENC28J60_Write(MAADR1, macaddr[4]);
   ENC28J60_Write(MAADR0, macaddr[5]);
   }
void ENC28J60_Init(uint8_t* macaddr)
   {  
   ENC28J60_CS=1;         
   ENC28J60_WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
   NextPacketPtr = RXSTART_INIT;
   ENC28J60_Write(ERXSTL, RXSTART_INIT&0xFF);   
   ENC28J60_Write(ERXSTH, RXSTART_INIT>>8);
   ENC28J60_Write(ERXRDPTL, RXSTART_INIT&0xFF);
   ENC28J60_Write(ERXRDPTH, RXSTART_INIT>>8);
   // RX end
   ENC28J60_Write(ERXNDL, RXSTOP_INIT&0xFF);
   ENC28J60_Write(ERXNDH, RXSTOP_INIT>>8);
   // TX start     1500
   ENC28J60_Write(ETXSTL, TXSTART_INIT&0xFF);
   ENC28J60_Write(ETXSTH, TXSTART_INIT>>8);
   // TX end
   ENC28J60_Write(ETXNDL, TXSTOP_INIT&0xFF);
   ENC28J60_Write(ETXNDH, TXSTOP_INIT>>8);
   ENC28J60_Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
   ENC28J60_Write(EPMM0, 0x3f);
   ENC28J60_Write(EPMM1, 0x30);
   ENC28J60_Write(EPMCSL, 0xf9);
   ENC28J60_Write(EPMCSH, 0xf7);
   ENC28J60_Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
   ENC28J60_Write(MACON2, 0x00);
   ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
   ENC28J60_Write(MAIPGL, 0x12);
   ENC28J60_Write(MAIPGH, 0x0C);
   ENC28J60_Write(MABBIPG, 0x15);
   ENC28J60_Write(MAMXFLL, MAX_FRAMELEN&0xFF);   
   ENC28J60_Write(MAMXFLH, MAX_FRAMELEN>>8);
   ENC28J60_SetMac(macaddr);
   ENC28J60_PhyWrite(PHCON1, PHCON1_PDPXMD);
   ENC28J60_PhyWrite(PHCON2, PHCON2_HDLDIS);
   ENC28J60_SetBank(ECON1);
   ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
   ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
   }
// read the revision of the chip:
/*uint8_t ENCJ60_GetRev(void)
   {
   return(ENC28J60_Read(EREVID));
   }
*/
void ENC28J60_PacketSend(uint16_t len, uint8_t* packet)
   {
   // Thiet lap con tro bat dau khong gian bo dem truyen
   ENC28J60_Write(EWRPTL, TXSTART_INIT&0xFF);
   ENC28J60_Write(EWRPTH, TXSTART_INIT>>8);

   //Thiet lap con tro nhan du lieu theo kich thuoc goi nhan duoc
   ENC28J60_Write(ETXNDL, (TXSTART_INIT+len)&0xFF);
   ENC28J60_Write(ETXNDH, (TXSTART_INIT+len)>>8);

   ENC28J60_WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

   // Copy du lieu tu goi tin vao bo dem truyen
   ENC28J60_WriteBuffer(len, packet);

   // Gui noi dung bo dem truyen len tren mang
   ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
   if( (ENC28J60_Read(EIR) & EIR_TXERIF) )
      {
        ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
        }
   }

// Gets a packet from the network receive buffer, if one is available.
// The packet will by headed by an ethernet header.
//      maxlen  The maximum acceptable length of a retrieved packet.
//      packet  Pointer where packet data should be stored.
// Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
uint16_t ENC28J60_PacketReceive(uint16_t maxlen, uint8_t* packet)
   {
   uint16_t rxstat;
   uint16_t len;
   if( ENC28J60_Read(EPKTCNT) ==0 )  
      {
      return(0);
        }

   //Thiet lap con tro de doc du lieu tu goi tin nhan duoc       
   ENC28J60_Write(ERDPTL, (NextPacketPtr));
   ENC28J60_Write(ERDPTH, (NextPacketPtr)>>8);

   //Doc gai tri con tro cua goi tin tiep theo
   NextPacketPtr  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0);
   NextPacketPtr |= (uint16_t)ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

   // Doc kich thuoc cua goi tin
   len  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0);
   len |= (uint16_t)ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    len-=4; // xoa 4byte dem CRC
   //Doc trang thai cua bo nhan
   rxstat  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0);
   rxstat |= (uint16_t)ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
    if (len>maxlen-1)
      {
        len=maxlen-1;
       }
    if ((rxstat & 0x80)==0)
         {
          len=0;
          }
      else
         {
            // Copy du lieu vao bo dem nhan
            ENC28J60_ReadBuffer(len, packet);
           }
   // Chuyen con tro du lieu nhan toi phan dau cua goi tin tiep theo.
   // Giai phong bo nho ma ta vua doc ra
   ENC28J60_Write(ERXRDPTL, (NextPacketPtr));
   ENC28J60_Write(ERXRDPTH, (NextPacketPtr)>>8);
   ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
   return(len);
   }



