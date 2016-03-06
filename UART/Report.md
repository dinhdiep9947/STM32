# UART
## Đặc điểm UART
Giao thức truyền không đồng bộ UART hiện nay rất phổ biến trong các thiết bị.  Các phiên bản trước của Keil C, cộng động đã viết sẵn các chuẩn truyền thông, nhưng sau khi **ST** phát triển thư viện hàm HAL thì những thư viện của dộng đồng không còn sử dụng được. Tuy các thanh ghi, cách hoạt động vẫn như thế, nhưng tên của chúng đã không còn giống. Tuy thế chúng ta cũng cần phải tìm hiểu một số đặc điểm của STM32Fxxx.

Chúng ta cần chú ý 1 vài đặc điểm khi cài đặt UART:

- Tốc độ Baud: đây là thành phần quyết định tốc độ truyền như thế nào. Các tiêu chuẩn tốc độ Baud: 1200, 4800, 9600, 57600, 115200... Số càng lớn, tốc độ truyền càng cao. Bình thường, chúng ta nên chỉ sử dụng tới 115200bps cho mọi ứng dụng.
- Độ dài từ: thường sử dụng độ dài 8bit, trừ khi bạn muốn truyền thông multi-slaves thì chúng ta cần sử dụng độ dài 9bit. Tuy nhiên, truyền thông multi-slave cũng có thể làm được với độ dài 8bit.
## Gửi/Nhận dữ liệu 
Trong unit này sử dụng USB PL2303 kết nối với UART1 của kit STM32F411.

### Khởi tạo

Sử dụng phần mềm CubeMx để cài đặt bằng giao diện sau đó tạo code.
![](http://i.imgur.com/zmcI4Ks.png)

Ta có thể tuy chỉnh các thông số cài đặt như mong muốn. Sau đó tạo project.
>
	void MX_USART1_UART_Init(void)
	{
  		huart1.Instance = USART1;
  		huart1.Init.BaudRate = 115200;
  		huart1.Init.WordLength = UART_WORDLENGTH_8B;
  		huart1.Init.StopBits = UART_STOPBITS_1;
 	    huart1.Init.Parity = UART_PARITY_NONE;
 	    huart1.Init.Mode = UART_MODE_TX_RX;
 	    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
 	    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
 	    HAL_UART_Init(&huart1);
	}
>Đây chính là hàm khởi tạo của UART trong Keil C.

Nếu muốn sử dụng ngắt, sang tab `NVIC Settings` tick chọn Interrupt. Hàm ngắt này được khai báo trong `stm32f4xx_it.c` 

>
	void USART1_IRQHandler(void)
	{
  	/* USER CODE BEGIN USART1_IRQn 0 */
  	/* USER CODE END USART1_IRQn 0 */
  		HAL_UART_IRQHandler(&huart1);
  	/* USER CODE BEGIN USART1_IRQn 1 */
  	/* USER CODE END USART1_IRQn 1 */
	}
>Đây chính là hàm khai báo ngắt. Code ở đây sẽ thực thi khi xảy ra ngắt này.

### Truyền nhận trong chế độ Polling

- Sử dụng hàm truyền UART trong C/C++, ở đây là hàm `printf` hoặc `sprintf` có sẵn ở trong Keil. Hoặc để đơn giản hơn dùng thư viện HAL xây dựng cho ARM:

>	
		// In ra 1 chuỗi static
		sprintf(buffer,"Test...\r\n");
		length=strlen(buffer);
		HAL_UART_Transmit(&huart1, buffer, len, 1000);
		// In ra 1 chuỗi động hoặc có các biến
		sprintf(buffer,"This is i:%i\r\n",i);
		length=strlen(buffer);
		HAL_UART_Transmit(&huart1, buffer, len, 1000);
>Để sử dụng hàm `sprintf` hay `strlen` thì phải khai báo các thư viện C/C++ liên quan. [http://www.cplusplus.com/](http://www.cplusplus.com/) giải thích rất đầy đủ và chi tiết về các hàm, thư viện...trong C/C++.

- Sử dụng thư viện hàm HAL.

>Gửi/nhận dữ liệu bằng hàm `HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)` `HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)`. 
>
>Trong đó thông số đầu tiên là biến uart, thứ 2 là con trỏ đệm dữ liệu, tiếp theo là kích thước, cuối cùng là thời gian Timeout.
>Các hàm HAL này đều trả về trạng thái sau khi thực thi nên chúng ta cũng có thể kiểm tra xem đã gửi/nhận dữ liệu thành công hay chưa.

### UART Interrupt

Sự kiện ngắt UART được kết nối như vector ngắt. 

>Truyền/nhận sử dụng hàm `HHAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)` và `HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)`
>
>Khi số `Size` truyền/nhận xong thì hàm ngắt `HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)` hay  `HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)` được thực thi.
>Trong trường hợp lỗi truyền thì hàm `HAL_UART_ErrorCallback()` được thực thi. Các hàm ngắt là hàm không trả về giá trị, chúng ta có thể lập trình các hoạt động khi ngắt thực thi.

Về chương trình, ở đây dùng Interrupt để phát hiện mỗi ký tự nhận được, sau đó lưu vào mảng, đến khi có ký tự kết thúc thì cũng kết thúc chương trình nhận 1 chuỗi. (Có rất nhiều cách để nhận dữ liệu UART sử dụng timer hoặc các cách khác).

Thủ tục ngắt UART như sau:

	char Rx_indx, Rx_data[2], Rx_Buffer[100], Transfer_cplt;
 	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	{
	uint8_t i;
	if (huart->Instance == USART1)	// UART hiện tại
		{
		if (Rx_indx==0) 
		{
			for (i = 0;i < 100;i++) 
            Rx_Buffer[i] = 0;	//xóa Rx_buffer trước khi nhận
		}	
		
		if (Rx_data[0]!=13)	// nếu dữ liệu khác với ký tự ascii 13 (enter)
			Rx_Buffer[Rx_indx++] = Rx_data[0];	// ghi dữ liệu
		else
		{			
			// nếu nhận được ký tự báo kết thúc chuỗi
			Rx_indx=0;
			Transfer_cplt = 1; // việc truyền đã kết thúc, sẵn sàng nhận dữ liệu mới
 		}
		HAL_UART_Receive_IT(&huart1, Rx_data, 1);	// khai báo để ngắt UART có hiệu lực mọi lúc
		}
	}
Lưu ý rằng các hàm Callback sẽ được gọi tự động. Và ký tự phát hiện kết thúc là ký tự `enter` nên do đó string nhận phải có ký tự `enter` ở cuối.

Ví dụ đơn giản đọc dữ liệu nhận được UART và gửi lại qua UART:

	while (1)
	{
		if (Transfer == 1)
		{
			sprintf(buffer,"%s\r\n",Rx_Buffer);
			length=strlen(buffer);
			HAL_UART_Transmit(&huart1, buffer, len, 1000);
			Transfer_cpl t= 0;		// xóa trạng thái
			HAL_Delay(500);
		}
	}


#### Truyền UART sử dụng DMA

Dữ liệu tải từ vùng SRAM đã được cấu hình sử dụng ngoại vi DMA tới thanh ghi USART_DR bất cứ lúc nào bit TXE = 1. Khi chương trình truyền 1 lượng dữ liệu trong bộ điều khiển được phát hiện, bộ điều khiển DMA tạo ra ngắt trong vector ngắt của kênh DMA.

Trong chế độ truyền, mỗi khi DMA ghi tất cả mọi dữ liệu để truyền đi (cờ TCIF = 1), cờ TC có thể được điều khiển để chắc chắn rằng truyền thông USART đã hoàn tất. Điều này phải yêu cầu để tránh việc truyền lần cuối bị mất trước khi không cho phép hay dừng USART. Lập trình phần mềm phải đợi tới khi TC = 1.

#### Nhận sử dụng DMA

Dữ liệu được tải từ thanh ghi USART\_DR tới vùng SRAM đã được thiết lập sử dụng ngoại vi DMA bất cứ lúc nào 1 byte dữ liệu được nhận. Bit DMAR nên được xóa bằng phần mềm trong thanh ghi USART\_CR3 trong khi thủ tục con ngắt. **Nếu DMA được sử dụng cho việc nhận, không cho phép bit RXNEIE**


**CẦN PHẢI LƯU Ý RẰNG TÊN GHI TRÊN CÁC PIN KHÔNG GIỐNG VỚI TÊN GHI TRONG DATASHEET, nghĩa là pin A3 trên KIT không phải pin PA3**.

