# FINAL PROJECT 
# Tên Đề tài : Phát triển trang trại thông minh Smart Farm 
# Nội dung: 
- Với đam mê về cây cối và mong muốn phát triển nông nghiệp. Dự án cuối kì này được em phát triển trong khả năng từ đề tài giữa kì. Từ việc tưới cây đơn thuần em tích hợp thêm 1 số tính năng với mong muốn phát triển thành nông trại thông minh.
- ESP32 tích hợp thêm quang trở để mô phỏng cung cấp ánh sáng vào những ngày trời thiếu nắng, các thông số như độ ẩm, ánh sáng, nhiệt độ được hiển thị lên màn hình oled để giúp người trồng dễ quan sát, cùng đó em có phát triển thêm tính năng cửa tự động sử dụng RFID để tăng cường bảo an.
- Các dữ liệu sẽ gửi qua MQTT đến dashboard cũng như có thể điều khiển ngược lại từ dashboard.
# Linh kiện:
- Esp32
- Cảm biến độ ẩm đất
- Module RFIF RC522
- Relay5V
- Máy bơm mini
- Oled SSD1306
- Servo SG90
- Cảm biến vật cản hồng ngoại HW488
- Quang trở,Led, điện trở
# Đặt vấn đề:  
Dự án hệ thống chăm sóc cây trồng tự động không chỉ dừng lại ở việc tự động hóa tưới nước, mà còn mở ra nhiều tiềm năng lớn để phát triển thành mô hình nông trại thông minh. 
# 1.Lợi ích của Nông Trại Thông Minh 

Tự động hoá tưới nước: Hệ thống sử dụng cảm biến để đo độ ẩm của đất, đề xuất thời gian và lượng nước tưới tối ưu. 

Giảm thiểu lao động: Giúp người nông dân giảm bớt thời gian và công sức trong việc chăm sóc cây trồng. 
Phân tích dữ liệu: Thu thập dữ liệu từ các cảm biến về đất, nước, và môi trường để tối ưu hoá quy trình trồng trọt. 
Quản lý từ xa: Tích hợp IoT cho phép theo dõi và điều khiển từ xa qua ứng dụng di động. 

# 2.Mục Tiêu Phát Triển 
Mở rộng từ hệ thống chăm sóc cây trồng nhỏ lẻ đến quy mô trang trại. 
Tích hợp nhiều công nghệ hiện đại như cảm biến môi trường, điều khiển tự động, và AI.  
Tạo ra một hệ sinh thái nông nghiệp thông minh, hướng tới nông nghiệp bền vững. 
# Sơ đồ khối: 
![fine](https://github.com/user-attachments/assets/c464d6de-3812-432d-a3aa-36d60c20f0df)

# Kết quả dự án: 
# Giao diện Node-RED: 
![flow-red](https://github.com/user-attachments/assets/ae685111-0cbe-4c55-a5ad-ee4d654bd77c)
# Các chế độ điều khiển , thông số hiển thị ,trạng thái: 
- Trên giao diện để điều khiển và quan sát các thông số từ một" nhà vườn" với các thông số về độ ẩm , ánh sáng
- Ở đây em có thêm quang trở để đo độ sáng để có thể cấp 1 ánh sáng thay thế vào những ngày trời không quá nắng.
- Chế độ AUTO sẽ tự động điều khiển máy bơm nước và đèn dựa trên các cảm biến đo được.
- Chế độ MANUAL hoạt động thì mọi điều khiển chỉ có thể thực hiện thủ công mà không bị ảnh hưởng bởi cảm biến. 
![pump_of_led_on](https://github.com/user-attachments/assets/b302afdc-a17c-4e0c-a779-3378e170c3a8)
# Giao diện Dashboard khu vực cửa nông trại: 
- Ở đây, em thiết kế sao cho mỗi lần nhân viên sử dụng thẻ thì dữ liệu sẽ được đẩy lên dashboard có hiện tên và ngày giờ để dễ dàng quản lí.
- Ngoài ra, em có thiết kế một bóng đèn dạng tự động khi có người đến gần để bật đèn khi trời tối. Tuy nhiên, bóng cũng có thể điều khiển thủ công
  
![gate](https://github.com/user-attachments/assets/e056d783-96bb-4bf0-9997-33f7adece032) 
# Giao diện HiveMQ: 
- Đây là giao diện HiveMQ với các tin được nhận và gửi cũng như các topic đã subcribe.
- Server hoạt động thực sự rất tốt.
  ![hivemqnew](https://github.com/user-attachments/assets/1a4a7791-45dc-499a-98f9-7d8f5b84f83d)
# Video Demo các tính năng phát triển thêm: 
-Ngoài tính năng tưới cây tự động ra có thêm 1 số tính năng khác
- Dưới đây là ảnh về tòan bộ dự án: 
![462587342_1278210319989585_1758715125430796888_n](https://github.com/user-attachments/assets/f5b1b70c-c7c1-4288-9b76-4e82b7e48837)

- Dưới đây là ảnh về các thông số khi được hiển thị trên OLED SSD1306 
![470050547_1637403043871237_7623123799336720375_n](https://github.com/user-attachments/assets/e07ff456-85e5-421d-9e22-79dcc0fe6b81)


-Khi tới cửa sẽ có bóng đèn tự động sử dụng cảm biến vật cản hồng ngoại để phát hiện người. 
https://github.com/user-attachments/assets/503c1f3f-20c8-421c-98fe-cd02e8e25bc9 

- Mô phỏng việc cửa mở tự động dùng thẻ RFID và servo trong trường hợp nhân viên là đúng người, đồng thời hiển thị lên LCD. 
https://github.com/user-attachments/assets/dc65d5bc-fa84-47f3-92f7-50e4967ddfae

- Đây là mô phỏng việc nhân viên không hợp lệ:
https://github.com/user-attachments/assets/9d65b00c-0991-4b4d-995a-6c8ca7190bed
-Mô phỏng việc dùng quang trở để cung cấp ánh sáng trong những ngày thiếu nắng trong nông trại: 
https://github.com/user-attachments/assets/1b17de9a-a663-496d-bf59-f6eacbe1f0eb
# Kết luận: 
- Việc làm việc trong dự án này giúp em rèn luyện tính độc lập và quyết đoán trong công việc do là project cá nhân.
- Học thêm về kiến thức về giao thức MQTT, Node RED, Git,..
- Rèn luyện thêm về kĩ năng code
- Tuy nhiên, do thiếu sót về kinh nghiệm nên trong dự án này không tránh khỏi nhiều thiếu sót, bản thân việc em có thể nghĩ ra thêm khá nhiều tính năng nhưng chỉ có khả năng tạm thời thực hiện 1 số các ý tưởng đó, các ý tưởng còn lại chưa phát triển vào dự án.
- Dù vậy, em mong muốn có cơ hội phát triển bản thân và cơ hội ngành nghề trong tương lai.












