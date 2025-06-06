#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==== CẤU HÌNH CHÂN ====
#define IR_LED_PIN 4       // Gửi IR
#define DHTPIN 26          // Cảm biến DHT11
#define DHTTYPE DHT11
#define SOIL_PIN 34        // Cảm biến độ ẩm đất (chân analog)
#define RELAY_PIN 25       // Relay điều khiển tưới

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

IRPanasonicAc ac(IR_LED_PIN);
DHT dht(DHTPIN, DHTTYPE);

// ==== NGƯỠNG BẬT TẮT ====
const float TEMP_ON = 31.0;
const float TEMP_OFF = 27.0;
const int SOIL_DRY = 30;   // Dưới 30% là khô → tưới
const int SOIL_WET = 60;   // Trên 60% là ẩm → ngừng

// ==== TRẠNG THÁI ====
bool airconOn = false;
bool isWatering = false;

void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(1000); // Đợi cảm biến khởi động
  ac.begin(); // KHÔNG GỌI setModel
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Mặc định máy lạnh ở chế độ làm lạnh 16 độ
  ac.setMode(kPanasonicAcCool);
  ac.setTemp(16);
  ac.setFan(kPanasonicAcFanAuto);
  ac.setSwingVertical(false);
  ac.setSwingHorizontal(false);
  ac.setQuiet(false);
  ac.setPowerful(false);

  // Màn hình OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED không khởi động được!");
    for (;;);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int soilADC = analogRead(SOIL_PIN);

  // Chuyển đổi độ ẩm đất sang %
  int soilPercent = map(soilADC, 0, 285, 0, 100);  // 0 khô = 0%, 285 ướt = 100%
  soilPercent = constrain(soilPercent, 0, 100);

  // Nếu lỗi cảm biến
  if (isnan(t) || isnan(h)) {
    Serial.println("❌ Lỗi cảm biến DHT11");
    return;
  }

  // ===== ĐIỀU KHIỂN MÁY LẠNH =====
  if (!airconOn && t > TEMP_ON) {
    ac.on();
    ac.send();
    airconOn = true;
    Serial.println("🟢 BẬT máy lạnh (Nhiệt độ cao)");
  } else if (airconOn && t < TEMP_OFF) {
    ac.off();
    ac.send();
    airconOn = false;
    Serial.println("🔴 TẮT máy lạnh (Nhiệt độ thấp)");
  }

  // ===== TƯỚI CÂY =====
  if (!isWatering && soilPercent < SOIL_DRY) {
    digitalWrite(RELAY_PIN, HIGH);
    isWatering = true;
    Serial.println("💦 BẬT tưới cây (Đất khô)");
  } else if (isWatering && soilPercent > SOIL_WET) {
    digitalWrite(RELAY_PIN, LOW);
    isWatering = false;
    Serial.println("💤 TẮT tưới cây (Đất ẩm)");
  }

  // ===== SERIAL MONITOR =====
  Serial.print("🌡 Nhiet do: ");
  Serial.print(t);
  Serial.print("°C | 💧 Do am: ");
  Serial.print(h);
  Serial.print("% | 🪴 ADC: ");
  Serial.print(soilADC);
  Serial.print(" | ");
  Serial.print(soilPercent);
  Serial.println("%");

  // ===== OLED =====
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.print("🌡 Nhiet do: ");
  display.print(t, 1);
  display.println(" C");

  display.print("💧 Do am: ");
  display.print(h, 1);
  display.println(" %");

  display.print("🪴 Dat: ");
  display.print(soilPercent);
  display.println(" %");

  display.print("❄️ May lanh: ");
  display.println(airconOn ? "BAT" : "TAT");

  display.print("💦 Tuoi: ");
  display.println(isWatering ? "CO" : "KHONG");

  display.display();

  delay(5000);
}
