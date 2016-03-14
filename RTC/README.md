# Real-time clock (RTC)

Đồng hồ thời gian thực là một timer hoạt động độc lâp. RTC cung cấp 1 counter cài đặt chạy liên tiếp mà nó có thể sử dung, cung cấp các hàm ngày-tháng. Giá trị counter có thể được ghi bởi thời gian/ ngày tháng hiện tại của hệ thống.

Lõi RTC và cấu hình xung clock (thanh ghi RCC_BDCR) ở trong vùng Back-up, nghĩa là các cài đặt RTC và thời gian được giữ sau khi reset hay wakeup từ chế độ Standby.

Sau khi reset, truy cập vào thanh ghi Backup và RTC không được cho phép và vùng Backup (BKP) được bảo vệ để chống lại việc truy cập ghi.

## Đặc điểm chính của RTC

- Prescaler có thể lập trình: hệ số chia tới 2^20
- Counter 32-bit
- Hai xung clock độc lập: PCLK1 từ APB1 và xung RTC (giá trị nhỏ nhất phải nhỏ hơn 4 lần so với xung PCLK1 với)
- Nguồn clock RTC có thể cung cấp bởi một trong những nguồn sau:
  - Clock HSE được chia bởi 128
  - Bộ dao động LSE
  - Bộ dao động LSI

- Hai loại reset riêng rẽ:
  - Reset Interface APB1 bằng reset hệ thống.
  - RTC Core (Prescaler, Alarm, Counter và Divider) là reset chỉ bởi một reset vùng Backup.
  
- Ba đường Interrupt:
  - Ngắt Alarm, cho việc tạo 1 ngắt báo hiệu có thể lập trình bằng phần mềm.
  - Ngắt thứ 2, để tạo 1 tín hiệu ngắt theo chu kỳ với độ dài chuy kỳ có thể lập trình được (lên tới 1s).
  - Ngắt overflow, để phát hiện khi counter nội có thể lập trình được trồi về 0.

## Mô tả hoạt động RTC

![](http://i.imgur.com/qtIEEZs.png)

Hình trên là sơ đồ khối RTC đơn giản, ta có thể thấy được RTC gồm có 2 nhóm chính. Đầu tiên là APB1 interface được sử dụng để giao tiếp với bus APB1. Nhóm này cũng gồm có 1 tập thanh ghi 16-bit có thể truy cập được từ bus APB1 trong chế độ đọc hoặc ghi.

Nhóm khác (RTC core) gồm có 1 chuỗi các counter lập trình được tạo ra 2 khối chính. Khối đầu tiên là RTC prescaler, mà nó tạo ra thời gian nền `TR_CLK` có thể lập trình với chu kỳ lên tới 1s. Nó gồm có 1 bộ chia lập trình được 20-bit. Mỗi chu kỳ `TR_CLK`, RTC tạo ra một ngắt (ngắt thứ 2) nếu nó được cho phép. Khôi thứu 2 là counter lập trinh được 32-bit mà có thể được khởi tạo cho thời gian hệ thống hiện tại. Thời gian hệ thống được tăng với tốc độ TR_CLK và so sánh với ngày tháng lập trình được để tạo ra một ngắt báo hiệu, nếu được cho phép.

### Việc reset thanh ghi RTC

Lưu ý rằng mọi reset hệ thống hoặc reset nguồn đều không có tác dụng với các cài đặt thanh ghi RTC. Chúng chỉ có tác dụng reset khi ta reset Backup Domain.

### Đọc các thanh ghi RTC

Core RTC hoàn toàn độc lập bởi interface RTC APB1.

Truy xuất phần mềm giá trị prescaler RTC, counter, alarm thông qua APB1 interface. Lần đầu đọc thanh ghi RTC APB1 có thể bị lỗi (thường là bằng 0) nếu interface APB1 không được cho phép trước đó và đọc giá trị xay ra ngay lập tức sau khi interface APB1 được cho phép nhưng trước lần cập nhập nội đầu tiên của thanh ghi. Điều này có thể xảy ra nếu:

- Reset hệ thống hoặc reset nguồn xảy ra.
- MCU mới được đánh thức từ chế độ Standby.
- MCU mới được đánh thức từ chế độ Stop.

Trong mọi trường hợp ở trên, core RTC vẫn giữ cho chạy trong khi interface APB1 đã không được cho phép (reset, không xung clock hoặc unpowered).

Do đó khi đọc thanh ghi RTC, sau khi interface RTC APB1 không được cho phép, phần mềm đầu tiên phải chờ cho bit RSF (Register Synchronized Flag) trong thanh ghi RTC_CRL được set bằng phần cứng.

*Lưu ý rằng interface APB1 không ảnh hưởng bởi chế độ low-power WFI và WFE*

### Cấu hình các thanh ghi RTC

Để ghi các thanh ghi `RTC_PRL`, `RTC_CNT`, `RTC_ALR`, ngoại vi phải vào chế độ Configuration Mode.

Ngoài ra, việc ghi bất kỳ thanh ghi RTC chỉ được cho phép khi hoạt động ghi trước đó đã kết thúc. Để cho phép phần mềm phát hiện được trạng thái này,  bit trạng thái RTOFF được cung cấp để chỉ ra rằng 1 cập nhập của thanh ghi đang trong quá trình hoạt động. Giá trị mới có thể được ghi vào thanh ghi RTC chỉ khi RTOFF = 1.

**Thủ tục cấu hình:**

1. Poll RTOFF, chờ tới khi giá trị bằng 1.
2. Set bit CNF để vào chế độ cấu hình.
3. Ghi một hoặc nhiều thanh ghi RTC.
4. Xóa bit CNF để thoát khỏi chế độ cấu hình.
5. Poll RTOFF, chờ tới khi giá trị bằng 1 để kiểm tra kết thúc hoạt động ghi.

Hoạt động ghi chỉ thực thi khi bit CNF bị xóa; việc này mất ít nhất 3 chu kỳ RTCCLK để hoàn thành.

### RTC Clock

Nguồn clock RTCCLK có thể cung cấp bởi HSE/128, LSE hoặc LSI. Sự lựa chọn này bằng cách lập trình các bit RTCSEL[1:0] trong thanh ghi điều khiển miền Backup (RCC\_BDCR). Sự lựa chọn không thể thay đổi được ngoại trừ việc reset miền Backup. Do đó:

- Nếu LSE được chọn là RTC Clock:
 - RTC tiếp tục làm việc dù là nguồn cung cấp VDD bị tắt, nguồn được cung cấp từ Vbat còn lại.

- Nếu LSI được chọn như đơn vị clock Auto-Wakeup (AWU):
 - Trạng thái AWU không được đảm bảo nếu nguồn VDD bị tắt. 

- Nếu HSE clock chia cho 128 được sử dụng là RTC clock:
 - Trạng thái RTC không được đảm bảo nếu nguồn VDD bị tắt hoặc bộ điều chỉnh điện áp nội bị tắt.
 - Bit DPB (Disable backup domain write protection) trong thanh ghi điều khiển Power phải được set = 1.


LSE clock ở trong miền Backup, còn HSE và LSI clock thì không. 

## Thiết lập trên STM32CubeMX và code Keil C

Để giữ lại nội dung của các thanh ghi backup RTC, backup SRAM và nuôi RTC khi VDD bị ngắt, pin VBAT có thể kết nối với nguồn áp nuôi standby như Pin...

### Cài đặt và khởi tạo RTC

	RTC_HandleTypeDef hrtc;

	void MX_RTC_Init(void)
	{

  		RTC_TimeTypeDef sTime;
  		RTC_DateTypeDef sDate;
  		RTC_AlarmTypeDef sAlarm;

    	/**Initialize RTC and set the Time and Date */
  		hrtc.Instance = RTC;
  		hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  		hrtc.Init.AsynchPrediv = 127;
  		hrtc.Init.SynchPrediv = 255;
  		hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  		hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  		hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  		HAL_RTC_Init(&hrtc);

  		sTime.Hours = 0x7;
  		sTime.Minutes = 0x52;
  		sTime.Seconds = 0x30;
  		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  		HAL_RTC_SetTime(&hrtc, &sTime, FORMAT_BCD);

  		sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
  		sDate.Month = RTC_MONTH_FEBRUARY;
  		sDate.Date = 0x3;
  		sDate.Year = 0x16;

  		HAL_RTC_SetDate(&hrtc, &sDate, FORMAT_BCD);

    	/**Enable the Alarm A */
  		sAlarm.AlarmTime.Hours = 0x0;
  		sAlarm.AlarmTime.Minutes = 0x0;
  		sAlarm.AlarmTime.Seconds = 0x0;
  		sAlarm.AlarmTime.SubSeconds = 0x0;
  		sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  		sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  		sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  		sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  		sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  		sAlarm.AlarmDateWeekDay = 0x1;
  		sAlarm.Alarm = RTC_ALARM_A;
  		HAL_RTC_SetAlarm(&hrtc, &sAlarm, FORMAT_BCD);	
	}

Ở trên là code cấu hình các thành phần của RTC như thời gian, ngày tháng năm và cho phép Alarm A.

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

Hàm lấy thời gian RTC hiện tại. Fomart có định dạng theo Bin và BCD. Chúng ta cần sử dụng SubSeconds và SecondFraction để chuyển giá trị SubSeconds trong tỉ lệ phần giây với đơn vị thời gian theo công thức:

>Second fraction ratio * time\_uint = [(SecondFraction-SubSeconds)/(SecondFraction+1)] * time\_uint.
 
Chuyển đổi này chỉ được thực thi khi không có hoạt động dịch trong lúc chờ.

**Chú ý rằng phải gọi hàm HAL\_RTC\_GetDate sau khi gọi hàm HAL\_RTC\_GetTime() để mở khóa giá trị trong những thanh ghi calendar bậc cao đảm bảo sự đồng nhất giữa giá trị thời gian và ngày tháng. Nêu không không đọc được giá trị thời gian.**