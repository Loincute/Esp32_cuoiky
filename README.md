# HỆ THỐNG CHỐNG TRỘM XE MÁY ESP32 – MQTT – NODE-RED

## 1. Vấn đề thực tế
### Nhu cầu thực tế

Tình trạng trộm cắp xe máy tại các khu vực dân cư, chung cư, bãi gửi xe ngày càng tinh vi. Những hình thức phổ biến:
- Nâng xe để trộm bộ vành hoặc dắt xe đi.
- Nghiêng xe để phá khóa hoặc lấy trộm đồ.
- Tác động mạnh vào xe khi gửi ở nơi công cộng.
  
Các hệ thống chống trộm truyền thống (khóa cơ, còi đơn thuần) không đủ thông minh để phát hiện chính xác hành vi trộm.

Vì vậy cần một giải pháp IoT thông minh:
- Phát hiện nghiêng – nâng xe dựa vào cảm biến gia tốc/gyro.
- Báo về điện thoại ngay lập tức qua MQTT.
- Ghi log lịch sử, hiển thị biểu đồ, điều khiển từ xa.

### Lý do thực hiện
Dự án được thực hiện nhằm tạo ra một thiết bị chống trộm xe giá rẻ, nhỏ gọn, có thể lắp được cho mọi loại xe:
- Sử dụng ESP32, dễ lập trình, giá thành thấp.
- SW-420 đo độ rung xe.
- MPU6050 đo nghiêng – rung – nâng xe siêu nhạy.
- MQTT + Node-RED giúp hiển thị trực quan và cảnh báo real-time.logy accessible to small- and medium-sized parking facilities.

## 2. Nội dung chính

### 2.1. Phần cứng sử dụng

| Thiết bị                          | Chức năng                                      |
|-----------------------------------|------------------------------------------------|
| ESP32                             | Vi điều khiển chính                            |
| MPU6050 (Accel + Gyro)            | Phát hiện nghiêng xe, nâng xe, rung động       |
| Buzzer                            | Báo động khi phát hiện trộm                    |
| LED (trạng thái hệ thống)         | ON/OFF – báo trạng thái chống trộm             |
| Nguồn máy tính                    | Cấp nguồn cho ESP32 & MPU6050                  |

### 2.2. Phần mềm sử dụng
- Node-RED Dashboard
    + Hiển thị trạng thái chống trộm: ON/OFF
    + Giá trị Accel & Gyro hiện thực
    + Lịch sử cảnh báo qua email qua Node-RED
- MQTT Broker: MQTTX
- PlatformIO để lập trình ESP32
### 2.3. Nguyên lý hoạt động
#### 1. Phát hiện nghiêng xe
ESP32 lấy dữ liệu từ MPU6050:
- Gia tốc trục X/Y/Z
- Gyro trục X/Y/Z
Khi góc nghiêng vượt ngưỡng cảnh báo:
- TiltThreshold = 0.7G → Nghiêng trái/phải mạnh
- LiftThreshold = 0.6G → Bị nâng bánh lên
  
→ Hệ thống kích hoạt báo động, gửi MQTT đến Dashboard.
#### 2. Kích hoạt báo động
Điều kiện báo động:
- Nghiêng xe quá mức
- Nâng đầu xe
- Xe bị rung mạnh (gyro tăng đột ngột)
Khi báo động:
- Buzzer kêu 150ms ON / 150ms OFF
- Dashboard hiển thị ALARM = 1
- Sau 120 giây tự tắt (nếu không reset)
#### 3. Điều khiển từ xa
Dashboard có nút:
Chống trộm ON
Chống trộm OFF
Cho phép bật/tắt còi qua MQTT
#### 4. Ghi log & biểu đồ
Node-RED lưu:
Accel X/Y/Z
Gyro X/Y/Z
Lịch sử thời điểm báo động
Mức độ nghiêng
## 3. Sơ đồ khối
[MPU6050] → (I2C) → [ESP32] → MQTT → [Node-RED Dashboard]
                                ↓
                             Buzzer
                                ↓
                               LED
## 4. Sơ đồ chân ESP32
| Thiết bị    | Chân   | ESP32 |
| ----------- | ------ | ----- |
| **MPU6050** | VCC    | 3.3V  |
|             | GND    | GND   |
|             | SCL    | 22    |
|             | SDA    | 21    |
| **Buzzer**  | Signal | 27    |
| **LED**     | Signal | 26    |
## 5. Kết quả
