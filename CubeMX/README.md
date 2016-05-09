# STM32CubeMX

STM32CubeMX là phần mềm giúp cho người dùng mới bắt đầu phát triển dễ dàng hơn, thời gian và tiền bạc. 

## Tùy chọn STM32Cube Firmware Library Package

Có những thực thi sau đâu:

- Copy toàn bộ thư viện người sử dụng vào thư mục project: STM32CubeMX sẽ copy tới thư mục project người sử dụng, mọi thư viện driver (HAL, CMSIS) và mọi thư viện middleware thích hợp cho việc cấu hình (FatFs, USB..).
- Chỉ copy những file thư viện cần thiết: STM32CubeMX sễ chỉ copy những thư viện thích hợp cho người sử dụng cấu hình vào thư mục project (SDIO HAL driver from HAL library..).
- Thêm những thư viện được yêu cầu như sự tham khảo trong file cấu hình toolchain project: mặc định, những file thư viện được yêu cầu được copy tới project người sử dụng. 

## Tùy chọn cài đặt HAL

- Đặt mọi chân không sử dụng thành anolog để tối ưu năng lượng tiêu thụ.
- Enable/Disable Sử dụng hàm Full Assert: các lệnh Define trong stm32xx_hal_conf.h sẽ được commented hoặc uncommented.cs