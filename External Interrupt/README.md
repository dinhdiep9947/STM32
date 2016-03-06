# Interrupt

Ngắt là loại tín hiệu báo cho CPU và yêu cầu CPU dừng chương trình chính lại để thực hiện một chương trình khác ở nơi khác, đó được gọi là thủ tục ngắt (ISR), sau khi thực hiện xong sẽ trở lại hàm chính để tiếp tục. Một vài đặc điểm để đạt được thủ tục ngắt tin cậy thì ta cần nhớ:

- Code trong thủ tục ngắt phải ngắn gọn nhất có thể.
- Không bao giờ sử dụng delay hoặc chờ hay 1 loại thời gian nào đó trong thủ tục ngắt.
- Việc sử dụng biến chung phải quản lý cẩn thận.



## Nested vectored interrupt controller (NVIC)

Đầy là bộ điều khiển vecto ngắt lồng nhau của lõi ARM Cortex M. Mọi ngắt của lõi đều được quản lý bởi NVIC. Mỗi ngắt tương ứng với một địa chỉ vật lý (interrupt vector).
![](http://i.imgur.com/Kmdtt4P.png)

NVIC có khả năng điều khiển ngắt với các mức ưu tiên khác nhau như bảng vecto trên, số ở cột Priority càng nhỏ thì độ ưu tiên càng cao. Mức ưu tiên có thể thay đổi bởi người dùng bằng cách lập trình. Có rất nhiều nguồn ngắt: đường EXTI, Timer, ADC...

STM32F1 hỗ trợ việc lồng ngắt, nghĩa là trong khi đang thực thi 1 handler ngắt có độ ưu tiên thấp, 1 thủ tục ngắt cao có ưu tiên trước và handler có ưu tiên thấp bị hoãn lại, và được resume khi thủ tục ngắt cao hơn hoàn thành.

## External Interrupt (ngắt ngoài)

Bộ điều khiển ngắt ngoài/sự kiện trong STM32F103xx gồm có tới 20 bộ phát hiện cạnh kết nối với các thiết bị. Mỗi đường ngõ vào có thể được cấu hình độc lập để chọn loại (xung hoặc chờ) và tương ứng với sự kiện trigger (lên, xuống hoặc cả 2).

### Mô tả hoạt động

Để tạo ngắt, chân ngắt phải được cấu hình và cho phép. Điều này thực hiện được bằng việc lập trình thanh ghi trigger với bộ phát hiện cạnh mong muốn và bằng cho phép request ngắt. Khi cạnh đã chọn được phát hiện ở chân ngắt ngoài thì 1 request ngắt được tạo ra.

#### Sơ đồ chân ngắt ngoài

112 chân GPIO được kết nối với 16 đường ngắt/sự kiên ngoài.
![](http://i.imgur.com/7MBdYCS.png)

Trên hình là sơ đồ kết nối các đường ngắt/sự kiện ngoài. Trong cùng 1 lúc ta không thể sử dụng ngắt PA0 và PB0 để xử lý hai sự kiện khác nhau được. Vì 2 chân này đều kết nối tới 1 đường ngắt. Ta chỉ sử dụng trong cùng 1 lúc các chân mà nó kết nối với đường ngắt tương ứng khác nhau.



## Lập trình trong Keil C

### Khởi tạo

Khởi tạo ngắt ngoài bằng CubeMx. Chọn chân mong muốn để khi tác động xảy ra ngắt.
![](http://i.imgur.com/prxPE2o.png)

Chọn GPIO_EXTI13, nghĩa là chân này kết nối với đường ngắt 13.

![](http://i.imgur.com/3RFmYui.png)

Trong phần Pin Configuration ta chọn chân 13 rồi cài đặt chế độ ngắt ở tab GPIO Pull-up/Pull-down. Với mỗi ứng dụng khác nhau lựa chọn chế độ phát hiện cạnh khác nhau.

![](http://i.imgur.com/j5pnyC7.png)

Chúng ta vào tab NVIC trong thẻ System, cho phép ngắt EXTI line (dòng cuối cùng), ta có thể thiết lập độ ưu tiên cho đường ngắt ngoài ở thẻ Preemtion Priority phía dưới. Sau đó tạo code trên Keil C.


    void MX_GPIO_Init(void)
	{
  	GPIO_InitTypeDef GPIO_InitStruct;

  	/* GPIO Ports Clock Enable */
  	__GPIOC_CLK_ENABLE();

  	/*Configure GPIO pin : PC13 */
  	GPIO_InitStruct.Pin = GPIO_PIN_13;
  	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  	/* EXTI interrupt init*/
  	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
>Đây là khai báo ngắt cho GPIO, thủ tục ngắt này ở trong file **stm32f1xx_it.c**


	void EXTI15_10_IRQHandler(void)
	{
  	/* USER CODE BEGIN EXTI15_10_IRQn 0 */

  	/* USER CODE END EXTI15_10_IRQn 0 */
  	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  	/* USER CODE BEGIN EXTI15_10_IRQn 1 */

  	/* USER CODE END EXTI15_10_IRQn 1 */
	}

>Đây là thủ tục ngắt, ta có thể viết code thực thi ở đây. Khi bộ phát hiện cạnh ở chân GPIO\_PIN\_13 này phát hiện được thì sẽ gọi hàm này ra.

Hoặc có 1 cách khác để thực hiện, đó là gọi hàm Callback khi đường ngắt ngoài được phát hiện. Nghĩa là bất cứ ngắt ngoài nào được phát hiện thì sẽ gọi hàm này.

	void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
	{
		// do something1
		if (GPIO_Pin == GPIO_PIN_13)
		{
			//do something2
		}
	}
>Ở đây, nếu ngắt này xảy ra ở chân GPIO\_PIN\_13 mong muốn thì ta có thể lập trình thực thi công việc của mình.

Trên đây là trình bày sơ lược về ngắt, cách thiết lập và sử dụng ngắt ngoài. Trong lần tiếp theo sẽ sử dụng dụng các chế độ ưu tiên để xem rõ hơn về ưu điểm NVIC của STM32. 