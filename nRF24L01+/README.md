# STM32Fxxx với nRF24L01

# nRF24L01
Module nrf24L01+ là sản phẩm của NORDIC, là chip thu phát 2.4Ghz được thiết kế cho các ứng dụng không dây công suất thấp, hoạt động ở băng tần 2.4 - 2.4835Ghz. nRF24L01 được cấu hình và hoạt động thông qua giao tiếp SPI.

Giao thức nhúng (Enhanced ShockBurst) được dựa trên truyền thông gói và hỗ trợ nhiều chế độ từ hoạt động bằng tay tới các hoạt động tự động. Các FiFo bên trong đảm bảo luồng dữ liệu "mượt mà" giữa vô tuyến ngoại vi và hệ thống MCU.

Tốc độ dữ liệu không dây của nRF24L01 có thể cấu hình được lên tới 2Mbps. Tốc độ dữ liệu không dây cao kết hợp với hai chế độ tiết kiệm năng lượng làm cho nRF24L01 rất thích hợp cho những thiết kế công suất rất thấp.

## Đặc điểm

Đặc điểm của nRF24L01 gồm có:

1. Vô tuyến:
 - Hoạt động băng tần 2.4Ghz toàn cầu.
 - 126 kênh RF
 - Tốc độ dữ liệu không dây 1 và 2Mbps.
 - 1Mhz khoảng kênh truyền không chồng tại 1Mbps.
 - 2Mhz khoảng kênh truyền không chồng tại 2Mbps.

2. Bộ thu phát
 - Độ nhậy -82dBm ở 2Mbps.
 - Độ nhạy -85dBm ở 1Mbps.
 - Độ lợi LNA lập trình được.

3. Enhanced ShockBurst:
 - Chiều dài dynamic payload 1 đến 32 byte.
 - Tự động bắt tay gói.
 - 6 pipe dữ liệu MultiCeiver cho mạng sao 1:6

4. Quản lý công suất
 - Nguồn cung cấp từ 1.9 đến 3.6V
 - Tối đa 1.5ms khởi động từ chế độ power down.
 - Tối đa 130us khởi động từ chế độ standby-I.

5. Host Interface
 - Phần cứng 4-pin SPI.
 - Tối đa 8Mbps.
 - 3 FiFo độc lập 32 byte.

## Điều khiển vô tuyến
nRF24L01 xây dừng 1 trạng thái máy để điều khiển chuyển đổi giữa các chế độ hoạt động khác nhau. Trạng thái máy nhận ngõ vào từ các giá trị thanh ghi người dùng chỉ định và tín hiệu nội.

### Các chế độ hoạt động
nRF24L01 chưa được định nghĩa cho tơi khi VDD = 1.9V hoặc cao hơn. Khi đó, nRF24L01 đi vào trạng thái reset Power, sau đó đi tới chế độ Power Down. Mỗi khi nRF24L01 đi vào chế độ Power Down MCU có thể điều khiển chip qua SPI và chân CE có 3 trạng thái được sử dụng.

#### Chế độ Power Down
Trong chế độ này nRF24L01 bị từ chối với dòng tiêu thụ rất nhỏ, mọi giá trị thanh ghi sẵn có từ SPI được duy trì và SPI có thể được hoạt động. 
#### Chế độ Standby
Bằng việc cài đặt bit PWR_UP trong thanh ghi CONFIG bằng 1, thiết bị đi vào chế độ Standby-I. Chế độ này được sử dụng dòng tiêu thụ trung bình tối thiểu duy trì thời gian khởi động ngắn. Đây là chế độ nRF24L01 trở lại từ chế độ TX hoặc RX khi CE xuống thấp.

Standby-II xảy ra khi CE kéo lên cao trong thiết bị PTX với FIFO TX rỗng. Nếu 1 gói được tải lên TX FIFO, PLL bắt đầu và gói được truyền. 
#### Chế độ RX
Chế độ RX là chế độ hoạt động khi nRF24L01 là bộ thu. Để vào chế dộ này, nRF24L01 phải set it PWR_UP mức cao, bit PRIM_RX mức cao và CE mức cao.

Trong chế độ này bộ thu giải điều chế tín hiệu từ kênh truyền RF. Nếu 1 gói có hiệu lực được tìm thấy payload của gói được đưa vào slot trống trong FIFO RX. Nếu FIFO RX đầy, gói nhận được bị lọa bỏ.

nRF24L01 ở trong chế độ RX cho tới khi MCU cấu hình nó vào chế độ standby-I hoặc chế độ Power Down. Trong chế độ RX, 1 bộ truyền phát hiện tín hiệu có sẵn. 
#### Chế độ TX
Chế độ TX là chế độ hoạt động khi nRF24L01 truyền 1 gói tin. Để vào chế độ này, nRF24L01 phải set bit `PWR_UP` mức cao, bit PRIM_RX mức thấp, 1 payload trong FIFO TX và, 1 xung mức cao trên chân CE hơn 10us.

nRF24L01 ở trong chế độ TX tới khi nó kết thúc việc phát gói tin hiện tại. Nếu CE = 0 nRF24L01 trở lại chế độ Standby-I. Nếu CE = 1, hoạt động tiếp theo được xác định bởi trạng thái của FIFI TX. Nếu FIFO TX không rỗng nRF24L01 còn ở trong chế độ TX, phát gói tiếp theo. Nếu FIFO TX rỗng nRF24L01 đi vào chế độ Standby-II. PLL bộ phát nRF24L01 hoạt động trong vòng mở (open loop) khi trong chế độ TX. Điều này quan trong để không bao giờ giữ nRF24L01 trong chế độ TX quá 4ms tại 1 thời điểm. Nếu việc tự động phát lại được cho phép, nRF24L01 không bao giờ ở trong chế độ TX đủ dài để không tuân theo luật này.
#### Cấu hình các chế độ hoạt động
![](http://i.imgur.com/wvIntto.png)
Bảng các chế độ hoạt động.

### Tốc độ truyền không dây
Tốc độ truyền không dây là tốc độ tín hiệu được điều chế nRF24L01 sử dụng khi truyền và nhận dữ liệu. 

Tốc độ dữ liệu không dây có thể là 1Mbps hay 2Mbps. Tốc độ dữ liệu 1Mbps cho 3dB hơn độ nhậy bộ thi so với 2Mbps. Tốc độ dữ liệu cao nghĩa là dòng tiêu thụ trung bình thấp hơn và giảm xác xuất xung đột không dây.

Tốc độ dữ liệu không dây được set bởi bit `RF_DR` trong thanh ghi `RF_SETUP`.

Bộ thu và bộ phát phải được lập trình với cùng một tốc độ dữ liệu không dây để có thể giao tiếp được với nhau.
### Kênh truyền tần số RF
Kênh truyền chiếm băng thông 1Mhz tại 1Mbps và 2Mhz tại 2Mbps. nRF24L01 có thể hoạt động trên các tần số từ 2.4Ghz đến 2.525Ghz. Độ phân giải của tần số kênh truyền RF là 1Mhz.

Tần số kênh truyền RF set bởi thanh ghi RF_CH theo biểu thức:

F0 = 2400 + RF_CH [Mhz]

Bộ thu và phát để giao tiếp được với nhau thì phải cùng một tần số kênh truyền RF.

### Điều khiển PA
Điều khiển PA được sử dụng để set công suất ngõ ra từ khuếch đại công suất (PA) nRF24L01. Trong chế dộ TX điều khiển PA có 4 mức có thể lập trình.

Điều khiển PA được set bởi 2 bit `RF_PWR` trong thanh ghi `RF_SETUP`.

![](http://i.imgur.com/ycFARd9.png)

## Enhanced ShockBurst
Nó có đặc điểm dịch mã gói dữ liệu và thời gian tự động, tự động chấp nhận và phát lại gói dữ liệu. Enhanced ShockBurst cho phép thực thi của công suất rất thấp, hiệu suất truyền tiếp cao với các microcontroller giá rẻ. 
### Đặc điểm
Đặc điểm chính Enhanced ShockBurst:
- Độ rộng payload từ 1 tới 32 byte.
- Tự động xử lý gói tin.
- Tự động xử lý thực hiện:
 - Tự động chấp nhận
 - Tự động phát lại.
- 6 Pipe dữ liệu cho mạng sao 1:6

### Khái quát
Enhanced ShockBurst sử dụng ShockBurst để tự động xử lý gói tin và timing. Trong khi truyền, ShockBurst dịch mã gói tin và clock trong gói dữ liệu vào bộ phát để truyền đi. Trong khi nhận, ShockBurst luôn luôn tìm kiếm địa chỉ có hiệu lực trong tín hiệu giải điều chế. 
#### Định dạng gói tin Enhanced ShockBurst
Gói tin Enhanced ShockBurst gồm vùng preamble, địa chỉ, vùng điều khiển gói tin, vùng payload và vùng CRC.
![](http://i.imgur.com/Y6MTS55.png)

##### Preamble
Preamble là 1 chuỗi liên tiếp được sử dụng để phát hiện mức 0 và 1 trong bộ thu. Preamble có độ dài 1 byte và hoặc là 01010101 hoặc 10101010. Nếu bit đầu tiên trong địa chỉ là 1 preamble tự động set 10101010 và ngược lại. Điều này thực hiện để đảm bảo có đủ sự chuyển tiếp trong preamble để ổn định bộ thu.
##### Địa chỉ
Đây là địa chỉ bộ thu. Một địa chỉ đảm bảo rằng đúng gói tin được phát hiện bởi bọ thu. Vùng địa chỉ có thể được cấu hình độ dài là 3,4,5 byte với thanh ghi AW.
##### Vùng điều khiển gói tin
Vùng điều khiển gói tin gồm có vùng độ dài payload 6 bit, 2 bit PID (Packet Indentity) và 1 bit cờ NO_ACK.
####### Độ dài Payload
Đây là 6 bit định ra độ dài của payload theo byte. Độ dài của payload có thể từ 0 tới 32 byte.

000000 = 0 byte (chỉ sử dụng trong gói rỗng ACK) 100000 = 32 byte, 100001 = Don't care.
###### PID (Packet identification)

2 bit vùng PID được sử dụng để phát hiện gói tin nhận được là gói mới hay gói được gửi lại. PID ngăn ngừa thiết bị PRX từ việc có mặt hơn 1 payload như nhau tới MCU. Trường PID được tăng tại phía TX cho mỗi gói mới đã nhận qua SPI. Các trường PID và CRC được sử dụng bởi các thiết bị PRX để xác định gói mới hay gói phát lại. Nếu gói dữ liệu có cùng PID với gói trước đó, nRF24L01 so sánh tổng CRC cho cả 2 gói. Nếu tổng CRC bằng nhau, gói nhận cuối cùng được xem xét là copy gói trước và bị từ chối.

####### No Acknowledgment flag (NO_ACK)

Cờ này chỉ được sử dụng khi đặc điểm tự động công nhận được sử dụng. Set cờ lên mức cao, nói cho bộ thu rằng gói tin không được tự động công nhận.

##### Payload
Payload là nội dung gói tin được người sử dụng định nghĩa. Nó có thể có độ dài từ 0 tới 32 byte và được truyền không dây như upload tới thiết bị

#### Tự động xử lý gói tin
##### Độ dài payload động và tĩnh

Enhanced ShockBurst cung cấp hai lựa chọn loại xử lý độ dài payload, tĩnh và động.

Mặc định là độ dài payload tĩnh. Với độ dài payload tĩnh mọi gói tin giữa bộ phát và bộ thu có cùng độ dài. Độ dài payload tĩnh được cài đặt bởi thanh ghi `RX_PW_Px` ở phía bộ thu. Độ dài payload ở phí bộ phát được set bởi số byte đi vào `TX_FIFO` và phải bằng với giá trị thanh ghi `RX_PW_Px` ở phía bộ thu.

Dynamic Payload Length (DPL) là một lựa chọn còn lại. DPL cho phép bộ phát gửi các gói tin với độ dài payload có thể khác nhau tới bộ thu. Với đặc điểm DPL nRF24L01 có thể giải mã tự động độ dài payload của gói tin nhận được bằng việc sử dụng thanh ghi `RX_PW_Px`. MCU có thể đọc độ dài payload nhận được bằng cách sử dụng lệnh `R_RX_PL_WID`.

Để cho phép DPL bit `EN_PL` trong thanh ghi `FEATURE` phải được set. Trong chế độ RX thanh ghi `DYNPD` phải được set.

## Data and Control Interface

- IRQ: tín hiệu này tích cực mức thấp
- CE: tín hiệu tích cực mức cao và được sử dụng để active chip trong chế độ TX hay RX.
- CSN: tín hiệu SPI
- SCK: tín hiệu SPI
- MOSI
- MISO

### Hoạt động SPI

#### SPI command

Mỗi lệnh mới phải được bắt đầu bởi 1 xung cao xuống thấp trên CSN.

Lệnh SPI đẩy song song vào chân MOSI, thanh ghi STATUS được dịch nối tiếp ra chân MISO.

Lệnh SPI dịch nối tiếp theo định dạng:

<Từ lệnh: MSBit tới LSBit (1 byte)>
<Byte dữ liệu: LSByte tới MSByte>
![](http://i.imgur.com/plIWqHo.png)

Lệnh `W_REGISTER` và `R_REGISTER` có thể hoạt động 1 hoặc nhiều byte thanh ghi. Khi truy cập thanh ghi nhiều byte bạn có thể đọc hoặc ghi MSBit của LSByte đầu tiên. 


### Register Map

![](http://i.imgur.com/2J5TH9o.png)
http://i.imgur.com/J5aGlAI.png
![](http://i.imgur.com/ty7okIM.png)
![](http://i.imgur.com/jTGpc7f.png)
![](http://i.imgur.com/k7uyicP.png)













