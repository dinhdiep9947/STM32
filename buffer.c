#include "buffer.h"

uint8_t BUFFER_Init(BUFFER_t* Buffer, uint32_t Size, uint8_t* BufferPtr){
	/* set tat ca gia tri bang 0 */
	memset(Buffer, 0, sizeof(BUFFER_t));

	/* set cac gia tri mac dinh*/
	Buffer->Size = Size;
	Buffer->Buffer = BufferPtr;
	Buffer->StringDelimiter = '\n';

	/* Kiem tra neu malloc se duoc su dung */
	if (!Buffer->Buffer) {
		/* Thu cap phat */
		Buffer->Buffer = (uint8_t *) LIB_ALLOC_FUNC(Size * sizeof(uint8_t));

		/* Kiem tra neu duoc cap phat */
		if (!Buffer->Buffer){
			/* Reset kich thuoc */
			Buffer->Size = 0;

			/* Tra ve loi */
			return = 1;
		} else {
			/* Gan co cho malloc*/
			Buffer->Flags |= BUFFER_MACLLOC;
		}
	}

	/* Khoi tao */
	Buffer->Flags |= BUFFER_INITIALIZED;

	return 0;
}

void BUFFER_Free(BUFFER_t* Buffer){
	/* Kiem tra struc buffer */
	if (Buffer == NULL){
		return;
	}

	/* Neu malloc da duoc su dung de cap phat*/
	if (Buffer->Flags & BUFFER_MALLOC){
		/* Bo nho trong */
		LIB_FREE_FUNC(Buffer->Buffer);
	}

	/* Xoa co */
	Buffer->Flags = 0;
	Buffer->Size = 0;
}

uint32_t BUFFER_Write(BUFFER_t* Buffer, uint8_t* Data, uint32_t count){
	uint32_t i = 0;
	uint32_t free;
#if BUFFER_FAST
	uint32_t tocopy;
#endif

	/*Kiem tra cau truc buffer*/
	if (Buffer == NULL || count == 0){
		return 0;
	}

	/* Kiem tra con tro input */
	if (Buffer->In >= Buffer->Size){
		Buffer->In = 0;
	}

	/* Get free memory */
	free = BUFFER_GetFree(Buffer);

	/* Kiem tra bo nho co san */
	if 
}