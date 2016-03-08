# STM32
Repo chứa các bài viết về các giao thức trên STM32, do kiến thức người thực hiện chưa đủ nên các bài viết chỉ dựa theo các bài hướng dẫn trên Internet, các tài liệu từ datasheet của ST. 

Việc ST ra mắt hàm HAL hỗ trợ cùng với phần mềm cấu hình giao diện để tạo code CubeMX đã trợ giúp cho người mới dùng rất nhiều. Chúng ta có thể nhờ đó mà tiếp cận nhanh với các dòng STM32 của ST hơn, nhưng cũng khó giúp chúng ta được cách hiểu sâu hơn. Nhất là về cách hoạt động chi tiết của nó.

Trước kia cộng đồng đã tạo sẵn nhiều thư viện để giúp chúng ta sử dụng luôn một cách nhanh chóng và dễ dàng. Sau những phiên bản gần đây ST cũng ra thư viện HAL có sẵn để hỗ trợ. Trong những bài viết ở đây, có cả sử dụng hàm HAL và sử dụng thư viện của cộng đồng (của tác giả Tilen Majerle). Thư viện của Tilen Majerle viết trước kia không còn sử dụng được cho các phiên bản gần đây của Keil C, nhưng tác giả đã chuyển đổi sử dụng thư viện HAL. Thanks Tilen Majerle for somethings you do.

Người thực hiện viết bài (thực ra dịch và tổng hợp lại) có những bài nói về chi tiết set-up từ những bit thanh ghi, có những bài viết chỉ là nói về cách hoạt động trong stm32. Nếu tự build các ngoại vi, các giao tiếp thì phải đọc, hiểu và nhớ những chi tiết này. Còn nếu sử dụng các thư viện có sẵn, thì chỉ đọc để tìm hiểu cách hoạt động trong STM32 như thế nào mà thôi.