#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

#define IR_LED_PIN 4  // GPIO điều khiển IR LED

IRPanasonicAc ac(IR_LED_PIN);

bool isOn = false;

void setup() {
  Serial.begin(115200);
  ac.begin();  // Không gọi setModel()

  // Cấu hình điều khiển
  ac.setMode(kPanasonicAcCool);       // Làm lạnh
  ac.setTemp(16);                     // 16 độ C
  ac.setFan(kPanasonicAcFanAuto);     // Quạt tự động
  ac.setSwingVertical(false);
  ac.setSwingHorizontal(false);
  ac.setQuiet(false);
  ac.setPowerful(false);
  // KHÔNG gọi ac.setModel(...);

  Serial.println("🌬️ IR test bật/tắt máy lạnh Panasonic mỗi 5 giây...");
}

void loop() {
  if (isOn) {
    Serial.println("🔴 Gửi lệnh TẮT...");
    ac.off();
  } else {
    Serial.println("🟢 Gửi lệnh BẬT...");
    ac.on();
  }

  // Debug mã IR gửi
  Serial.println(ac.toString());

  ac.send();      // Gửi mã IR
  isOn = !isOn;
  delay(10000);    // Đợi 5 giây
}
