# Bộ phân tích AT command ESP8266

Giao tiếp giữa stack ESP và các thiết bị ESP8266 là giao tiếp UART, chúng độc lập về dữ liệu truyền và nhận.  Trong phần này, tôi sẽ cố gắng giải thích cách stack ESP tự làm việc và tôi cố gắng làm rõ ràng bạn có thể làm gì trong mỗi trường hợp.

### Ứng dụng bộ đệm dữ liệu

Ứng dụng sử dụng 2 buffer lưu trữ dữ liệu. Đầu tiên bufer USART nơi mà dữ liệu có sẵn có trong ngắt UART và thứ 2 là buffer riêng tức thời cho việc lưu trữ dữ liệu tức thời.

Tại sao lại sử dụng thế? Thư viện cố gắng điều khiển nhiều 'event' nhất có thể, do cho mỗi thứ mà ESP8266 gửi tới ESP stack, hàm callback có thể được gọi để chỉ ra trạng thái này. Điều này rất thuận tiện, nhưng tạo ra vấn đề nếu callback này muốn làm việc nào đó khác, sẽ không nhận được trạng thái sự kiện và lưu nó lại.

Để kết nối với mạng Wifi, lệnh AT+CWJAP được sử dụng như dưới:

`AT_CWJAP="SSID NAME","SSID PASSWORD"<CR><LF>`

Cho tới khi modun hồi đáp chúng ta **OK** hoặc **ERROR**, chúng ta không thể gửi lệnh khác hay ESP8266 sẽ trả về **busy p...** tới chúng ta.

Modun kết nối thành công với mạng, nó sẽ trả về những cho chúng ta:

>WIFI CONNECTED<CR><LF>
>WIFI GOT IP<CR><LF>
>OK<CR><LF>

Có thể thấy được, một vài chuỗi thêm vào được trả về, 2 chuỗi trên rất quan trọng để sử dụng, khi mà ESP8266 lấy được 1 IP, chúng ta có thể bắt đầu tạo các kết nối như clients hoặc như server. Đây là lý do, tại sao gọi callback `WIFI CONNECTECED` và `WIFI GOT IP`, để chỉ cho người dùng một vài sự kiện mới đã xảy ra.

Nhưng điều gì sẽ xảy ra, nếu người dùng muốn bắt đầu kết nối mới trong hàm `WifiGotIPCallBack`?

Chúng ta sẽ gửi `AT+CIPSTART=...` và modun sẽ trả về **busy p..** và chúng ta ko làm được việc gì.

Do đó trong trường hợp này, tôi lưu mọi lệnh nhận được từ ESP8266 khi command được active và statement không được công nhận trong phần của command này. Tôi sử dụng buffer tức thời cho mọi thứ. Sau đó không hoạt động command nào nữa, tôi bắt đầu đọc bộ nhớ đệm 