#TIMER VÀ COUNTER

Trong chip STM32F103xx hỗ trợ tới 13 Timer và chia chúng thành 3 nhóm khác nhau, với những chức năng sử dụng khác nhau.

## Advanced-control timer (TIM1&TIM8)

TIM1&TIM8 gồm 1 couter 16-bit auto-reload điều khiển bởi bộ presaler lập trình được. Nó được sử dụng rất nhiều mục đích khác nhau như input capture hay tạo dạng sóng ngõ ra (so sánh ngõ ra, PWM, bù PWM với việc lồng dead-time).

Độ rộng xung và chu kỳ dạng sóng có thể được điều chế từ vài micro giây tới vài mini giây sử dụng bộ prescaler timer và bộ điều khiển prescaler RCC. TIM1&TIM8 và các timer TIMx thông thường hoạt động hòa toàn độc lập nhau và chúng không chia sẻ bất cứ nguồn nào. Chúng có thể đồng bộ cùng nhau mà thôi.

#### Time-base uint

Khối chính của timer advanded-control có thể lập trình là counter 17-bit với sự liên quan tới thanh ghi auto-reload của chúng. Counter có thể đếm lên, xuống hoặc cả lên và xuống. Xung clock Counter có thể được chia bởi bộ prescaler.

Chế độ counter, thanh ghi auto-reload và thanh ghi presacler có thể đọc hay ghi bằng phần mềm. Điều này vẫn đúng khi counter đang chạy.