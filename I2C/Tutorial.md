# I2C Example

Ở đây, người thực hiện làm 1 ví dụ đơn giản về truyền thông I2C, do sử dụng là thư viện HAL và cấu hình từ CubeMX nên sẽ ít thấy hoặc không có những tên thanh ghi như trong lý thuyết đã trình bày trước đây.

Sau này, khi vào dự án cụ thể, có nhiều xử lý sẽ sử dụng nhiều hơn. Hoặc build thư viện riêng của mình không sử dụng thư viện HAL mà chỉ dùng thanh ghi.

## Giao tiếp giữa 2 Kit STM32
### Khởi tạo

Việc khởi tạo trên CubeMX khá đơn giản do dùng giao diện để cài đặt. 

  	
	I2C_HandleTypeDef hi2c1;
	hi2c1.Instance = I2C1;
  	hi2c1.Init.ClockSpeed = 100000;
  	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  	hi2c1.Init.OwnAddress1 = 0;
  	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
  	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
  	hi2c1.Init.OwnAddress2 = 0;
  	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
  	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
  	HAL_I2C_Init(&hi2c1);
Một Handel `hi2c1` được tạo ra, rất dễ nhận ra các cấu hình qua tên như: Clockspeed, DutyCycle, Addressing Mode...

### Các chế độ hoạt động
#### Chế độ hoạt động Polling IO

- MASTER


> `HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)`
> `HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)`
>
> Các tham biến truyền vào như Handle, địa chỉ máy đích, con trỏ dữ liệu để đệm dữ liệu, kích thước dữ liệu gửi, thời gian Timeout. Ở đây người thực hiện không giải thích nghĩa tường minh 1 phần do kiến thức chưa đủ, 1 phần do việc sử dụng HAL đã che rất nhiều. Rất khó để tìm kiếm  thanh ghi nào đó.


- SLAVE


>`HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)`
>`HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)`
>Cũng như hàm của Master, có các tham số truyền như Handle, con trỏ dữ liệu để đệm dữ liệu, kích thước dữ liệu gửi/nhận, thời gian Timeout. Ở đây không có tham số địa chỉ đích như ở Master do đây là slave.

#### Chế độ hoạt động Interrupt IO

- MASTER

>Truyền/nhận trong chế độ master sử dụng `HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)0x30F, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 10000)` hoặc `HAL_I2C_Master_Receive_IT(&hi2c1, (uint16_t)0x30F, (uint8_t *)aRxBuffer, RXBUFFERSIZE, 10000)` các tham số cũng giống chế độ Polling. 
>Tại cuối việc truyền/nhận hàm `HAL_I2C_MasterTxCpltCallback` hoặc `HAL_I2C_MasterRxCpltCallback` được thực thi và người dùng có thể thêm code của mình để tùy biến.c
>Lưu ý rằng phải khai báo ngắt cho I2C đang hoạt động.

- SLAVE
>Cũng như chế độ MASTER, ở chế độ SLAVE chỉ cân thay thế `Master` bằng `Slave`.
>Trong trường hợp truyền bị lỗi, hàm ` HAL_I2C_ErrorCallback()` được thực thi và chúng ta cũng có thể tùy biến code của mình trong đó. Do các hàm truyền nhận sẽ trả về trạng thái khi thực thi xong. Truyền/nhận thành công hàm trả về kết quả `HAL_OK`

#### Example truyền nhận giữa 2 KIT STM32F411

Khai báo các biến sử dụng

	#define I2C_ADDRESS        0x30F

	/* Private variables ---------------------------------------------------------*/
	/* I2C handler declaration */
	extern I2C_HandleTypeDef hi2c1;

	/* Buffer used for transmission */
	uint8_t aTxBuffer[] = " ****I2C_TwoBoards communication based on Polling****  ****I2C_TwoBoards communication based on Polling****  ****I2C_TwoBoards communication based on Polling**** ";

Khởi tạo I2C

  	hi2c1.Instance = I2C1;
  	hi2c1.Init.ClockSpeed = 400000;
  	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
  	hi2c1.Init.OwnAddress1 = I2C_ADDRESS;
  	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
  	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
  	hi2c1.Init.OwnAddress2 = 0;
  	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
  	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
  	HAL_I2C_Init(&hi2c1);
>Ở đây sử dụng Speed ở chế độ Fast Mode với tốc độ xung 400kHz, chế độ Address 10bit.

  	while(HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 10000)!= HAL_OK)
  	{
    /* Error_Handler() function is called when Timeout error occurs.
       When Acknowledge failure occurs (Slave don't acknowledge it's address)
       Master restarts communication */
    	if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
    	{
      		Error_Handler();
    	}
  	}
  
>Bắt đầu quá trình truyền, Timeout set là 10s. Hàm `Error_Handler()` được gọi nết xảy ra lỗi Timeout, khi xẩy ra lỗi thiếu ACK (Slave không công nhận địa chỉ của nó) thì Master khởi động lại quá trình truyền thông.

Các hàm truyền nhận dữ liệu ở Slave tương tự sử dụng các hàm đã giới thiệu ở trên.