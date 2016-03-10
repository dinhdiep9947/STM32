# Sử Dụng Oled SSD1306 Với STM32F411
Trong unit này, sử dụng Oled SSD1306 để hiển thị ký tự mong muốn. Các hàm trong unit này sử dụng thư viện HAL, Oled này sử dụng giao thức I2C. Các hàm chủ yếu tham khảo từ thư viện của Tilen Majerle. (Thanks Tilen Majerle)

Do người sử dụng chưa tìm được cách sử dụng chung biến con trỏ để khai báo I2C nên các chưa tách được các hàm ra thành một file .h riêng, mà phải để chung với file main. Vấn đề này là do khởi tạo code bằng CubeMX. Chú ý rằng, CubeMX là một công cụ trợ giúp chúng ta tạo code đơn giản, trức quan và không phải ghi nhớ nhiều những thanh ghi, những cấu hình. Chứ không phải là một giải pháp để giải quyết bài toán. 

## OLED I2C SSD1306

Đây là một graphic LCD, hiển thị ký tự hoặc hình ảnh lên đó. 

Interface truyên thông I2C gồm có bit địa chỉ slave, tín hiệu dữ liệu data-I2C SDA và tín hiệu clock I2C SCL. Cả hai chân này đều đã được nối điện trở kéo lên ở trong module.
![](http://i.imgur.com/Pmgvd7P.png)

Hình trên là định dạng khung bus dữ liệu I2C.

- Bit địa chỉ Slave (SA0): SSD1306 được định địa chỉ slave trước khi truyền hay gửi bất kỳ tín hiệu nào đó bằng bus I2C. Thiết bị sẽ hồi đáp tới địa chỉ slave theo bởi bit địa chỉ slave và bit chọn đọc/ghi. R/W# = 1 là ở chế độ đọc, ngược lại bằng 0 là ở chế độ ghi.

### Các hàm giao tiếp

#### Gửi tín hiệu lệnh hoặc dữ liệu

	void SSD1306_WRITECOMMAND(uint8_t command)
	{
		uint8_t d[2];
	
		d[0] = 0x00;
		d[1] = command;
		HAL_I2C_Master_Transmit(&hci2c, (uint16_t)SSD1306_I2C_ADDR, (uint8_t*)d, 2, 1000);
	}

SSD1306_I2C_ADDR ở đây là 0x78. Mảng d gồm 2 phần từ, phần tử thứ nhất 0x00 để báo cho SSD1306 biết đây là mệnh lệnh, phần tử tiếp theo là lệnh muốn ghi.

	/* Bo dem du lieu SSD1306 */
	static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

	void SSD1306_Fill(SSD1306_COLOR_t color)
	{
		memset(SSD1306_Buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer));
	}

Khai báo bộ đệm dữ liệu gồm có `SSD1306_WIDTH * SSD1306_HEIGHT / 8` phần tử. Hàm này có chức năng đổ đầy dữ liệu vào bộ nhớ. `void * memset ( void * ptr, int value, size_t num )` với cách thành phần lần lượt là con trỏ, giá trị và kích thước.

	char SSD1306_Putc(char ch, TM_FONT_t* Font, SSD1306_COLOR_t color)
	{
		uint32_t i, j, b;
	
		/* Kiem tra khoang trong co san trong LCD */
		if(
			SSD1306_WIDTH <= (SSD1306.CurrentX + Font->FontWidth) ||
			SSD1306_HEIGHT <= (SSD1306.CurrentY + Font->FontHeight)
		)
		{
			/* loi khong du khoang trang */
			return 0;
		}
	
		for (i = 0; i < Font->FontHeight; i++) {
			b = Font->data[(ch - 32) * Font->FontHeight + i];
			for (j = 0; j < Font->FontWidth; j++) {
				if ((b << j) & 0x8000) {
					SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t) color);
				} else {
					SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)!color);
				}
			}
		}
		/* Tang kich thuoc con tro */
		SSD1306.CurrentX += Font->FontWidth;
	
		/* Tra ve ky tu duoc ghi */
		return ch;
	}

Hàm đẩy 1 ký tự tới bộ nhớ đệm.

	char SSD1306_Puts(char* str, TM_FONT_t* Font, SSD1306_COLOR_t color)
	{
		while(*str)
		{	
			/* ghi ky tu */
			if (SSD1306_Putc(*str, Font, color) != *str)
				return *str;
		
			/* Tang kich thuoc con tro */
			str++;
		}
		return *str;
	}

Hàm đẩy 1 chuỗi ký tự, với font và màu mong muốn (chỉ có màu đen và trắng).

	void SSD1306_UpdateScreen(void)
	{
		uint8_t m;
	
		for (m = 0; m < 8; m++ )
		{
			SSD1306_WRITECOMMAND(0xB0 + m);
			SSD1306_WRITECOMMAND(0x00);
			SSD1306_WRITECOMMAND(0x10);
		
			/* Ghi nhieu du lieu*/
			HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, &SSD1306_Buffer[SSD1306_WIDTH * m], SSD1306_WIDTH, 1000);
		}
	}

Đây là hàm cập nhập màn hình. Hàm `HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)` dùng để ghi một lượng dữ liệu tới vùng nhớ đặc trưng. 

Trên đây là những hàm cơ bản để ghi ký tự lên LCD SSD1306. Sử dụng giao tiếp I2C để điều khiển, cần lưu ý rằng biến con trỏ `I2C_HandleTypeDef hi2c1` phải được khởi tạo trước khi sử dụng các hàm điều khiển OLED.
