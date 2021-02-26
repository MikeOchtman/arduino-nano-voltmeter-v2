// implements a voltmeter using a single analog pin
// and a SSD1306 128x32 OLED screen to display

#include <U8g2lib.h>
#include <Wire.h>

const unsigned char OLED_WIDTH {
  128
};
const unsigned char OLED_HEIGHT {32};

const int voltMeterPin { A7};

const double RANGE_MAX {
  10.0
};  //maximum voltage to read
const double RANGE_MIN {
  0.0
}; // minimum voltage to read
const int ADC_MAX {
  1024
};  // maximum counts from ADC
const int ADC_MIN {
  0
};  // minimum counts from ADC
const unsigned char CHART_BUFFER_SIZE {
  100
}; // number of readings to display
const unsigned char TRACE_HEIGHT {
  22
}; //vertical number of pixels set aside for the time trace


const unsigned char CHART_OFFSET_X {OLED_WIDTH - CHART_BUFFER_SIZE};
const unsigned char CHART_OFFSET_Y {OLED_HEIGHT - TRACE_HEIGHT};

// A circular buffer with scaled position of a voltage
// point from 0 to TRACE_HEIGHT with 0 = RANGE_MIN V and
//  TRACE_HEIGHT = RANGE_MAX V
char readings[CHART_BUFFER_SIZE];
unsigned char head = 0;
char str_message[20];
char str_voltage[8];
double voltage;
char scale_hi[6];
char scale_lo[6];

// display on a OLED using the u8g2 library
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0,
    /* clock=*/ SCL,
    /* data=*/ SDA,
    /* reset=*/ U8X8_PIN_NONE);



void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < CHART_BUFFER_SIZE; ++i) {
    readings[i] = 0;
  }
  Serial.begin(115200);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "Volt Meter 2.0");
  u8g2.sendBuffer();
  delay(1000);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.sendBuffer();
  Serial.println("Voltmeter v2.0");
  dtostrf(RANGE_MAX, 3, 1, scale_hi);
  dtostrf(RANGE_MIN, 3, 1, scale_lo);

}

void loop() {
  // put your main code here, to run repeatedly:


  voltage = (double)analogRead(voltMeterPin) / (double)ADC_MAX * (RANGE_MAX - RANGE_MIN);
  dtostrf(voltage, 4, 3, str_voltage); // convert double to string
  sprintf(str_message, "%s Volts", str_voltage);
  Serial.println(str_message);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.drawStr(60, 10, str_message);

  // scale the latest voltage and add it to the buffer at position head
  // then plot the graph
  u8g2.setFont(u8g2_font_4x6_tf);
  u8g2.drawStr(3, OLED_HEIGHT - TRACE_HEIGHT + 6, scale_hi);
  u8g2.drawStr(3, OLED_HEIGHT, scale_lo);
  u8g2.drawStr(CHART_OFFSET_X-8,OLED_HEIGHT-(TRACE_HEIGHT/2)+4, "V");
  u8g2.drawLine(CHART_OFFSET_X - 1, OLED_HEIGHT - TRACE_HEIGHT, CHART_OFFSET_X - 1, OLED_HEIGHT-1);

  readings[head] = (voltage/(RANGE_MAX-RANGE_MIN))*TRACE_HEIGHT;

  unsigned char runner = head;
  for (int index = 0; index < CHART_BUFFER_SIZE; ++index) {
    u8g2.drawPixel(CHART_OFFSET_X + CHART_BUFFER_SIZE - index, OLED_HEIGHT - readings[runner] - 1);
    if (runner > 0) {
      runner--;
    } else {
      runner = CHART_BUFFER_SIZE - 1;
    }
  }
  u8g2.sendBuffer();
  
  head++;
  if (CHART_BUFFER_SIZE == head) {
    head = 0;
  }
  



}
