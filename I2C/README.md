# Giao tiếp I2C
Trong bài này được lấy từ chính Reference Manual của STM32F103, trong này nói về cách hoạt động, thanh ghi dữ liệu. Để khi lập trình có thể kiểm tra trạng thái bằng các thanh ghi hoặc bit, lập trình không dùng thư viện HAL. Nên sẽ nói 1 cách đầy đủ nhất có thể.
## Mô tả hoạt động I2C
Để truyền và nhận dữ liệu, giao tiếp này chuyển đổi từ dạng dữ liệu nối tiếp sang song song và ngược lại. Các ngắt được cho phép hay không cho phép bằng phần mềm. Giao tiếp này được kết nối tới bus I2C bằng 1 pin dữ liệu (SDA) và pin clock (SCL).
### Các chế độ lựa chọn
Truyền thông này có thể hoạt động 1 trong 4 chế độ:

- Slave transmitter
- Slave receiver
- Master transmitter
- Master receiver

Mặc định, nó hoạt động ở chế độ slave. Giao tiếp chuyển tự động từ slave sang master, sau khi tạo trạng thái START và từ master sang slave, nếu 1 sự quyết định (bộ quyết định) mất hoặc sự tạo Stop xảy ra, cho phép khả năng multimaster.

### Communication flow
Trong chế độ Master, giao tiếp I2C bắt đầu truyền 1 dữ liệu và tạo ra tín hiệu clock. Truyền dữ liệu nối tiếp luôn bắt đầu với 1 điều kiện bắt đầu và kết thúc với 1 điều kiện dừng lại. Cả hai điều kiện này đều được tạo ra trong chế độ master bằng phần mềm.

Trong chế độ Slave, giao tiếp có khả năng nhận ra được địa chỉ của nó (7 hoặc 10 bit), và địa chỉ General Call. Bộ phát hiện địa chỉ General Call có thể được cho phép hay từ chối bằng phần mềm.

Dữ liệu và địa chỉ được truyền trong byte 8-bit, đầu tiên là MSB. Byte đầu tiên theo sau bit điều kiện chứa địa chỉ (1 trong chế độ 7 bit, 2 trong chế độ 10 bit). Địa chỉ luôn được truyền trong chế độ Master.
![Giao thức I2C](http://i.imgur.com/L5ZzcA8.png)

### Chế độ Slave
Mặc định giao tiếp I2C hoạt động ở chế độ Slave. Để chuyển từ chế độ Slave mặc định sang chế độ Master thì việc tạo ra 1 điều kiện Start là cần thiết.

Xung clock vào ngoại vi phải được lập trình trong thanh ghi I2C_CR2 để tạo ra thời gian chính xác. Ngay khi 1 điều kiện bắt đầu được phát hiện, địa chỉ được nhận từ dây SDA và gửi để dịch thanh ghi. Sau đó nó so sánh với địa chỉ của interface (OAR1) và OAR2 (nếu ENDUAL = 1) hoặc địa chỉ General Call (nếu ENGC = 1).

*Trong chế độ địa chỉ 10-bit, việc so sánh bao gồm cả chuỗi header (11110xx0), xx kí hiệu 2 bit có trọng số lớn nhất của địa chỉ.*

**Header hay địa chỉ không tìm thấy:** giao tiếp từ chối và chờ điều kiện Start khác.

**Header phù hợp**(chỉ ở chế độ 10-bit): interface tạo ra 1 xung công nhận (ack) nết bit ACK = 1 và chờ địa chỉ slave 8-bit.

**Địa chỉ phù hợp**: giao tiếp tạo ra 1 chuỗi:
	
- 1 xung công nhận nếu ACK = 1.
- Bit ADDR = 1 bằng phần cứng và 1 ngắt được tạo ra nếu bit ITEVFEN = 1.
- Nếu ENDUAL = 1, phần mềm có thể đọc bit DUALF để kiểm tra xem địa chỉ slave đã được công nhận hay chưa.

Ở chế độ 10-bit, sau khi nhận được chuỗi địa chỉ slave trong chế độ Receiver. Nó sẻ đi vào chế độ Transmitter để nhận 1 điều kiện Start lặp lại theo bởi chuỗi header với những bit địa chỉ phù hợp và set bit trọng số nhỏ nhất (11110xx1).

Bit TRA chỉ ra slave ở chế độ Receiver hay Transmitter.
#### Chế độ Transmitter Slave
Theo sau địa chỉ nhận được và sau đó xóa ADDR, slave gửi các byte từ thanh ghi DR tới dây SDA qua thanh ghi dịch nội.

Slave kéo SCL xuống mức thấp cho tới khi ADDR được xóa và DR đã đầy với dữ liệu được gửi đi.

Khi xung công nhận được nhận:
- Bit TxE được set bằng phần cứng với 1 ngắt nết ITEVFEN và bit ITBUFEN được set.

Nếu TxE được set và một vài dữ liệu không được ghi vào thanh ghi I2C_DR trước khi việc kết thúc của việc truyền dữ liệu tiếp theo, bit BTF được set và interface chờ tới khi BTF được xóa bằng cách đọc I2C_SR1 theo sau bằng cách ghi thanh ghi I2C_DR, kéo SCL xuống mức thấp.

#### Chế độ receiver slave

Theo sau địa chỉ nhận được và sau khi xóa ADDR, slave nhận byte từ dây SDA vào thanh ghi DR qua thanh ghi dịch nội. Sau mỗi byte interface tạo ra 1 chuỗi liên tục:
- 1 xung công nhận nếu bit ACK = 1.
- bit RxNE được set bằng phần cứng và 1 ngắt được tạo ra nếu ITEVFEN và bit ITBUFEN được set.

#### Kết thúc giao tiếp slave

Sau khi byte dữ liệu  cuối cùng được truyền đi 1 điều kiện Stop được tạo ra bởi master. Interface phát hiện điều kiện này và set:
- Bit STOPF và tạo ra 1 ngắt nếu ITEVFEN được set.

Bit SROPF được xóa bằng cách đọc thanh ghi SR1 theo bởi việc xóa CR1.

### Chế độ master I2C

Trong chế độ master, interface I2C chỉ định 1 dữ liệu truyền và tạo ra tín hiệu clock. Chế độ master được chọn ngay khi điều kiên Start được tạo ra trên bus với bit START.

#### Điều kiện Start

Cài đặt bit START vì interface tạo ra điều kiện Start và để sang chế độ Master (bit M/SL set) khi bit BUSY bị xóa.

*Trong chế độ master, việc cài đặt bit START do interface tạo ra 1 điều kiện Restart tại cuối byte hiện tại của dữ liệu truyền đi.*
 Một điều kiện Start được gửi:
- Bit SB được set bằng phần cứng và 1 ngắt được tạo ra nếu bit ITEVFEN được set.

#### Truyền địa chỉ Slave

Sau khi địa chỉ slave được gửi tới đường SDA qua thanh ghi dịch nội.

- Trong chế độ địa chỉ 10-bit, gửi đi chuỗi header do sự kiện sau.
  - Bit ADD10 được set bằng phần cứng và 1 ngắt được tạo ra nếu bit ITEVFEN được set. (chế độ 7-bit không cài đặt bit này)
  - Bit ADDR được set bằng phần cứng và 1 ngắt được tạo ra nếu bit ITEVFEN được set.

Master có thể quyết định đi vào chế độ truyền hay chế độ nhận phụ thuộc vào LSB của địa chỉ slave được gửi.

- Trong chế độ địa chỉ 7-bit,
 - Để vào chế độ Phát, master gửi địa chỉ slave với LSB reset.
 - Để vào chế độ Nhận, master gửi địa chỉ slave với LSB set.

- Trong chế độ địa chỉ 10-bit,
 - Để vào chế độ Phát, master gửi header (11110xx00) và sau đó địa chỉ slave.
 - Để vào chế độ Nhận, master gửi header (11110xx0) và sau đó địa chỉ slave. Sau đó nó gửi 1 điều kiện Start lặp lại theo sau header (11110xx1)

Bit TRA chỉ ra master đang ở chế độ nào.

#### Chế độ phát Master

Theo việc truyền địa chỉ và sau đó xóa ADDR, master gửi byte từ thanh ghi DR tới đường SDA qua thanh ghi dịch nội.

Master chờ tới khi byte dữ liệu đầu tiên đã được đọc vào I2C_DR.

Khi xung ack được nhận, bit TxE được set bằng phần cứng và một ngắt được tạo ra nếu ITEVFEN và ITBUFEN được set.

##### Kết thúc truyền thông

Sau bute cuối cùng được ghi vào thanh ghi DR, bit STOP được set bằng phần mềm để tạo ra một điều kiện Stop. Interface tự động trở về chế độ slave (M/SL bị xóa).

*Điều kiện Stop nên được lập trình trong sự kiện EV8_2, khi hoặc TxE hay BTF được set.*

#### Chế độ Nhận master
Theo việc truyền địa chỉ và sau đó xóa ADDR, interface I2C đi vào chế đọ Nhận. Trong chế độ này, interfavce nhận byte từ đường SDA vào thanh ghi DR qua thanh ghi dịch nội. Sau mỗi byte interface tạ ra 1 chuỗi:

1. 1 xung ack nếu bit ACK được set.
2. Bit RxNE được set và một ngắt được tạo ra nếu bit ITEVFEN và bit ITBUFEN được set.

##### Kết thúc truyền thông

**Phương pháp 1**: phương pháp này là cho trường hợp khi I2C được sử dụng với ngắt có độ ưu tiên cao nhất trong ứng dụng.

Master gửi 1 NACK cho byte nhận được cuối cùng từ slave. Sau khi nhận được NACK này, slave 'phóng thích' sự điều khiển của đường SCL và SDA. Sau đó master có thể gửi 1 điều kiện Stop/Restart.

- Để tạo xung non-ack sau byte sữ liệu cuối cùng nhận được, bit ACK phải được xóa sau khi đọc byte dữ liệu cuối thứ 2 (sau sự kiện RxNE cuối lần 2).
- Để tạo điều kiện Stop/ReStart, phần mềm phải set bit STOP/START chỉ sau khi đọc byte dữ liệu cuối cùng thứ 2.
- Trong trường hợp 1 byte 'lẻ' được nhận, không cho phép ACK và việc tạp ra điều kiện STOP được thực hiện chỉ sau EV6 (trong EV6_1, chỉ sau ADDR bị xóa).

Sau việc tạo ra điều kiện Stop, interface tự động đi vào chế độ slave (bit M/SL xóa).

**Phương pháp 2**: phương pháp này cho trường hợp khi I2C được sử dụng với ngắt không có độ ưu tiên cao nhất hoặc khi I2C được sử dụng với polling.

Với phương pháp này, DataN_2 không được đọc, do sau khi DataN_1, truyền thông bị kéo(cả 2 RxNE và BTF được set). Sau đó, xóa bit ACK trước khi đọc DataN-2 trong DR để đảm bảo nó đã bị xóa trước khi có xung ACK DataN. Sau việc này, chỉ sau khi đọc DataN_2, set bit STOP/START và đọc DataN_1. Sau khi RxNE được set, đọc DataN.

### Điều khiển đường SDA/SCL

- Nếu việc 'kéo' (stretching) clock được cho phép:
 - Chế độ Phát: nếu TxE=1 và BTF=1: interface giữ đường clock thấp trước việc truyền để chờ cho vi xử lý đọc SR1 và sau đó ghi byte trong DR (cả thanh ghi đệm và dịch đều rỗng).
 - Chế độ Nhận: nếu RxNE=1 và BTF=1: interface giữ đường clock thấp sau khi nhận để chờ vi xử lý đọc SR1 và sau đó đọc byte trong DR (cả thanh ghi đệm và dịch đều full).

- Nếu việc kéo 'stretching' clock không cho phép trong chế độ slave:
 - Overrun Error trong trường hợp RxNE=1 và không đọc DR hết trước khi byte sau được nhận. Byte nhận được cuối cùng bị mất.
 - Underrun Error trong trường hợp TxE=1 và không ghi vào DR hết trước khi byte tiếp theo được truyền đi. Byte giống thế sẽ được gửi lại.
 - Write Collision không được quản lý.
 
### SMBus
#### Introdution

System Management Bus (SMBus) là giao tiếp thông qua 2 dây rất nhiều thiết bị có thể giao tiếp với nhau. Nó dựa trên nguyên lý hoạt động của I2C. SMBus cung cấp 1 bus điều khiển cho hệ thống và tác vụ quản lý hiệu suât có liên quan. 

Do chưa người thực hiện chưa có ứng dụng về SMBus nên chưa giới thiệu.

### DMA request

DMA request (khi cho phép) được tạo ra chỉ cho việc truyền dữ liệu. DMA request được tạo ra bởi DR trở nên rỗng trong việc truyền và DR trở nên đầy trong việc nhận. DMA request phải được cấp phát trước cuối byte hiện tại truyền đi. Khi số lượng dữ liệu truyền mà nó được lập trình cho tương ứng với kênh DMA được tìm thấy, bộ điều khiển DMA gửi 1 tín hiệu EOT tới interface I2C và tạo ra 1 ngắt Transfer Complete nếu cho phép:
- Master transmitter: trong thủ tục ngắt sau khi ngắt EOT, không cho phép DMA request do chờ 1 sự kiện BTF trước khi lập trình điều kiện Stop.
- Master receiver: khi số lượng byte được nhận bằng với hoặc lớn hơn 2, bộ điều khiển DMA gửi 1 tín hiệu phần cứng, EOT_1, tương ứng với cuối cùng nhưng 1 byte dữ liệu. Nếu, trong thanh ghi I2C_CR2, bit LAST được set, I2C tự động gửi 1 NACK sau khi byte tiếp theo theo EOT_1. Người dùng cần tạo ra 1 điều kiện Stop trong thủ tục ngắt DMA Transfer Complete nếu được phép.

#### Phát sử dụng DMA

Chế độ DMA có thể cho phép việc truyền bằng cách cài đặt bit DMAEN trong thanh ghi I2C\_CR2. Dữ liệu sẽ được tải từ 1 vùng Bộ nhớ được định trước sử dụng ngoại vi DMA tới thanh ghi I2C\_DR bất cứ lúc nào nếu TxE được set. Để sắp đặt 1 kênh DMA cho truyền thông I2C, thực hiện chuỗi sau: (x là số kênh truyền)

- Set địa chỉ thanh ghi I2C_DR trong thanh ghi DMA\_CPARx. Dữ liệu sẽ được chuyển từ địa chỉ này từ bộ nhớ sau mỗi sự kiện TxE.
- Set địa chỉ bộ nhớ trong thanh ghi DMA\_CMARx. Dữ liệu sẽ được tải vào I2C\_DR từ bộ nhớ sau mỗi sự kiện TxE.
- Cấu hình tổng số byte được truyền đi trong thanh ghi DMA\_CNDTRx. Sau mỗi sự kiện TxE, giá trị này sẽ giảm bớt.
- Cấu hình độ ưu tiên kênh truyền sử dụng bit PL[0:1] trong thanh ghi DMA\_CCRx.
- Set bit DIR, trong thanh ghi DMA_CCRx, và cấu hình ngắt sau 1 nửa được truyền hoặc truyền hết hoàn toàn phụ thuộc vào yêu cầu ứng dụng.
- Khởi động kênh truyền bằng cách set bit EN trong thanh ghi DMA\_CCRx.


**Không được cho phép bit ITBUFEN trong thanh ghi I2C_CR2 nếu DMA được sử để phát dữ liệu**.

### Interrupt I2C
![](http://i.imgur.com/gYsrHf9.png)

Hình trên là các ngắt của I2C (vẫn còn thiếu, nhưng chưa sử dụng nên không đề cập).

![](http://i.imgur.com/F52IY7L.png)

Hình trên là bảng interrupt mapping.