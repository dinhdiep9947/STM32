# Serial Peripheral Interface (SPI)

Trong các đường kết nối thiết bị mật độ cao và XL, interface SPI cung cấp 2 chức năng chính, hoặc nó hỗ trợ  giao thức SPI hoặc là giao thức âm thanh I2S. Mặc định thì chứ năng SPI được lựa chọn, chúng có thể chuyển đổi qua lại bằng phần mềm.

Serial peripheral interface (SPI) cho phép truyền thông bán song công/song công, đồng bộ, nối tiếp với các thiết bị bên ngoài. Interface có thể được cấu hình như master và trong trường hợp này nó cung cấp xung liên lạc (SCK) cho các thiết bị slave bên ngoài. 

**Có một số chân của SPI13/I2S3 được dùng chung với chân JTAG, chúng không bị điều khiển bởi bộ điều khiển IO và chúng dành riêng cho việc sử dụng JTAG (sau mỗi lần RESET). Vì mục đích này, trước khi cấu hình chân SPI13/I2S3, người dùng phải không cho phép JTAG và sử dụng interface SWD**

## Mô tả chức năng SPI

Thông thường, SPI được kết nối với thiết bị khác qua 4 chân:

- MISO: Master In/ Slave Out data. Chân này có thể sử dụng để truyền dữ liệu trong chế độ slave và nhận dữ liệu trong chế độ master.
- MOSI: Master Out/ Slave In data. Chân này sử dụng để truyền dữ liệu trong chế độ master và nhận dữ liệu trong chế độ slave.
- SCK: ngõ ra Serial Clock cho SPI master và ngõ vào cho SPI slave.
- NSS: lựa chọn Slave. Chân này để lựa chọn 1 thiết bị slave 

Truyền thông luôn được bắt đầu bởi master. Khi thiết bị master truyền dữ liệu tới thiết bị slave qua chân MOSI, thiết bị slave đáp ứng qua chân MISO. Điều này bao gồm truyền thông song công với cả dữ liệu ra và dữ liệu vào đồng bộ với cùng tín hiệu clock.

### Quản lý lựa chọn Slave (NSS)

Việc quản lý lựa chọn Slave phần cứng hoặc phần mềm có thể cài đặt sử dụng bit SSM trong thanh ghi SPI_CR1.

- Quản lý NSS phần mềm (SSM = 1): thông tin lựa chọn slave được điều khiển bên trong bởi giá trị của bit SSI trong thanh ghi SPI_CR1. Chân ngoài NSS có thể sử dụng cho các ứng dụng khác.
- Quản lý lựa chọn Slave phần cứng (SSM = 0): có hai cấu hình có thể sử dụng phụ thuộc vào việc cấu hình ngõ ra NSS (bit SSOE trong thanh ghi SPI_CR2)
 	- Cho phép ngõ ra NSS (SSM = 0, SSOE = 1): cấu hình này được sử dụng chỉ khi thiết bị hoạt động trong chế độ master. Tín hiệu NSS được kéo mức thấp khi master bắt đầu truyền thông và giữ mức thấp tới khi SPI không được phép.
 	- Không cho phép ngõ ra NSS (SSM = 0, SSOE = 0): cấu hình này cho phép multimaster có khả năng cho thiết bị hoạt động trong chế độ master. Cho các thiết bị cài đặt như slave, chân NSS hoạt động như ngõ vào NSS cổ điển: slave được chọn khi NSS mức thấp và bỏ chọn khi NSS mức thấp.

### Định dang khung dữ liệu

Dữ liệu có thể dịch ra hoặc là MSB hoặc LSB đầu tienf phụ thuộc vào giá trị của bit LSBFIRST trong thanh ghi SPI_CR1.

Mỗi khung dữ liệu có độ dài 8 hay 16-bit phụ thuộc vào kích thước của dữ liệu được lập trình.

### Cấu hình SPI trong chế độ slave

Trong việc cấu hình slave, xung clock nối tiếp được nhận ở chân SCK từ thiết bị master. Giá trị cài đặt trong bit BR[2:0] trong thanh ghi SPI_CR1, không làm ảnh hưởng tới tốc độ truyền dữ liệu.

*Việc này được khuyến cáo để cho phép slave SPI trước khi master gửi xung clock. Nếu không, việc truyền dữ liệu có thể xảy ra việc không mong muốn. Thành ghi dữ liệu của slave cần sẵn sàng trước khi cạnh đầu tiên của xung clock hoặc trước khi kết thúc chế độ truyền thông ongoing. *

#### Chuỗi Phát

Byte dữ liệu được đẩy ra song song tới bộ đệm Tx trong khi ghi 1 vòng.

Chuỗi phát bắt đầu khi thiết bị slave nhận tín hiệu clock và bit có trọng số lớn nhất của dữ liệu đi vào chân MOSI. Những bit còn lại (7 bit trong khung dữ liệu 8 bit và 15 bit trong khung dữ liệu 16 bit) được tải vào thanh ghi đệm. Cờ TXE trong thanh ghi SPI_SR được set khi truyền dữ liệu từ Tx Buffer tới thanh ghi dịch và ngắt được tạo ra nếu bit TXEIE được set.

#### Chuỗi nhận

Trong bộ nhận, khi truyền dữ liệu hoàn thành:

- Dữ liệu trong thanh ghi dịch được chuyển tới RX Buffer và cờ RXNE được set.
- Một ngắt được tạo ra nếu bit RXNEIE được set.

### Cấu hình SPI trong chế độ master

Trong cấu hình này chân MOSI là chân ngõ ra dữ liệu, chấn MISO là chân ngõ vào dữ liệu.

#### Chuỗi phát

Chuỗi phát bắt đầu khi 1 byte được ghi trong Tx Buffer.

Dữ liệu được tải song song vào thanh ghi dịch (từ bus nội) trong lúc việc truyền bit đầu tiên và sau đó dịch nối tiếp ra chân MOSI.

#### Chuỗi nhận

Trong bộ thu, khi hoàn thành truyền dữ liệu:

- Dữ liệu trong thanh ghi dịch được chuyển tới RX Buffer và cờ RXNE được set.
- Ngắt được tạo ra nếu cho phép ngắt (bit RXNEIE)

Luồng truyền liên tiếp có thể được duy trì nếu dữ liệu tiếp theo được truyền ở trong bộ đệm mỗi khi việc truyền được bắt đầu. Lưu ý rằng cờ TXE nên bằng 1 trước khi bất kỳ sự cố gắng để ghi bộ đệm Tx.

### Cấu hình SPI cho truyền thông bán song công

SPI có khả năng hoạt động ở chế độ bán song công ở 2 cấu hình:

- 1 xung clock và dây dữ liệu song hướng.
- 1 xung clock và 1 dây dữ liệu (chỉ nhận hoặc chỉ truyền)


### Keil C

- Khai báo cấu trúc handle SPI, ví dụ: SPI_HandleTypeDef hspi;
- Khởi tạo nguồn mức thấp SPI bằng cách dùng API HAL_SPI_MspInit():
 - Cho phép xung clock của interface SPI
 - Cấu hình các chân SPI
 - Cấu hình NVIC nếu cần sử dụng interrupt: chế độ ưu tiên, cho phép NVIC SPI IRQ handle.
 - Cấu hình DMA nếu cần sử dụng DMA.
- Lập trình Mode, BidirectionMode (chế độ 2 chiều), Data size, Baudrate Prescaler, quản lý NSS, FirstBit và cấu hình CRC.
- Khởi tạo các thanh ghi SPI bằng cách gọi API HAL_SPI_Init().
- Việc giới hạn chế độ vòng:
 - Chế độ vòng DMA không thể sử dụng khi SPI được cấu hình trong chế độ sau: Master 2Lines RxOnly, Master 1Line Rx
 - Đặc trưng CRC không được quản lý khi chế độ vòng DMA được cho phép.
 - Khi DMA SPI Pause/Stop, chúng ta phải dùng các API HAL_SPI_DMAPause()/HAL_SPI_DMAStop() chỉ dưới callback SPI.