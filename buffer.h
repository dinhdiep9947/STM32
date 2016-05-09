#ifndef BUFFER_H
#define BUFFER_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C"{
#endif

/* Thu vien bo dem su dung cho module ESP 
   Buffer nay khong chi co nhung toan tu read/write cho bo dem tuan hoan, ma con doc chuoi tu bo dem neu can thiet.
   Moi chuoi trong bo dem co 1 bo tach ky tu, trong hau het truong hop, Line Feed (0x0A) duoc su dung, va no cung la gia tri mac dinh khi buffer dduoc khoi tao
   Khi doc chuoi tu buffer, ban can biet nhung thu sau day:
   - Buffer ho tro hoat dong read/write fast memory su dung dac diem copy bo nho.
   - Buffer dau tien se kiem tra neu ky tu dau tach string ton tai trong buffer.
     - Neu no ton tai, ky tu se set den buffer user cho den khi dau tach duoc phat hien.
    - Dau tach duoc include trong chuoi.
  - Neu dau tach chuoi trong co trong buffer, nhung chu ky buffer full, thi string se duoc do day vao buffer user, boi vi chung ta can
  de bo nho free cho cac ky tu tiep theo, gom co ca ky tu tach.
  - Neu kich thuoc buffer nguoi dung nho hon so ky tu trong buffer truoc khi dau tach string duoc tim thay, string cung duoc do day vao
  buffer user.
  - Trong cac truong hop khac, neu khong co dau tach string trong buffer, buffer se khong tra bat ky thu gi va se kiem tra dieu nay truoc tien.
*/

#include "stdlib.h"
#include "string.h"
#include "stdint.h"

#define BUFFER_INITALIZED	0x01  /* Buffer initialized flag */
#define BUFFER_MALLOC		0x02  /* Buffer su dung malloc cho bo nho*/

/* Cap phat custom va free function neu can thiet */
#ifndef LIB_ALLOC_FUNC	
#define LIB_ALLOC_FUNC		malloc
#endif
#ifndef LIB_FREE_FUNC
#define	LIB_FREE_FUNC		free
#endif

#ifndef BUFFER_FAST
#define BUFFER_FAST			1
#endif

typedef struct _BUFFER_t{
	uint32_t Size;					/* Kich thuoc bo dem don vi la byte, KHONG DUOC DICHUYEN OFFSET, 0*/
	uint32_t In;					/* Con tro ninput de luu gia tri ke tiep, KHONG DUOC DI CHUYEN OFFSET, 1*/
	uint32_t Out;					/* Con tro output de doc gia tri tiep theo, KHONG DUOC DI CHUYEN OFFSET, 2*/
	uint8_t* Buffer;				/* Con tro den mang du lieu buffer, 3*/
	uint8_t Flags;					/* Flag cho buffer, 4*/
	uint8_t StringDelimiter			/* ky tu cho bo tach string khi doc string tu buffer, 5*/
	void UserParameters;			/* Con tro de gia tri nguoi su dung neu can thiet */
} BUFFER_t;

/**
Khoi tao cau truc buffer
*/

uint8_t BUFFER_Init(BUFFER_t* Buffer, uint32_t Size, uint8_t* BufferPtr);

/* bo nho ranh cho buffer cap phat
   Ham nay chi co y nghia khi neu malloc duoc su dung cho cap phat dong */

void BUFFER_Free*(BUFFER_t* Buffer);

/* ghi du lieu vao buffer */
uint32_t BUFFER_Write(BUFFER_t* Buffer, uint8_t Data, uint32_t count);

/* Doc du lieu tu buffer */
uint32_t BUFFER_Read(BUFFER_t* Buffer, uint8_t* Data, uint32_t count);

/* lay so phan con trong trong buffer */
uint32_t BUFFER_GetFree(BUFFER_t* Buffer);

/* lay so thanh phan trong buffer */
uint32_t BUFFER_GetFull(BUFFER_t* Buffer);
uint32_t BUFFER_GetFullFast(BUFFER_t* Buffer);

/* Reset con tro buffer */
void BUFFER_Reset(BUFFER_t* Buffer);

/* Kiem tra gia tri thanh phan dac trung duoc luu trong buffer < 0: khong tim thay, >=0 tim thay, tra ve dia chi trong buffer */
int32_t BUFFER_FindElement(BUFFER_t* Buffer, uint8_t Element);

/* kiem tra chuoi du lieu co duoc luu tru trong buffer khong*/
int32_t BUFFER_Find(BUFFER_t* Buffer, uint8_t* Data, uint32_t Size);


/* Set ky tu tach chuoi khi doc tu buffer*/
#define BUFFER_SetStringDelimiter(Buffer, Strlen)     ((Buffer)->StringDelimiter = (StrDel))

/* ghi chuoi du lieu duoc dinh dang vao buffer*/
uint32_t BUFFER_WriteString(BUFFER_t* Buffer, char* buff);

/* doc tu buffer */
uint32_t BUFFER_ReadString(BUFFER_t* Buffer, char* buff, uint32_t buffsize);

/* kiem tra ky tu co ton tai trong dia chi trong buffer k?*/
int8_t BUFFER_CheckElement(BUFFER_t* Buffer, uint32_t pos, uint8_t* element);


/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
