/* Arduino IDE 2.3.2 with "ESP32 Dev Module" for Firebeetle ESP32-D!
--> Mousetrap for ESP32 FireBeetle */

#include "Mail.h"
#include "Volt.h"
#include <Arduino.h>
#include <WiFi.h>

float voltBatt;
float voltBoard;
const int LED_PIN = 2;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);
  Serial.println("");
  Serial.println("---------------------------------------------");

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_15,0);

  Serial.print("Connecting to AP");
  WiFi.begin(SSID, PASSWORD);

  unsigned long startTime = millis();
  unsigned long waitTime = 5000;
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < waitTime){
    Serial.print(".");
    delay(100);
    }

  if (WiFi.status() != WL_CONNECTED){  // If WiFi not connected within waitTime
    Serial.println("");
    Serial.println("Could not connect to WiFi --> going to sleep until the mouse wakes me up");
    esp_deep_sleep_start();
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.print("Waiting for NTP server time reading");

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  unsigned long ntpStart = millis();
  unsigned long ntpTimeout = 10000;
  while (time(nullptr) < ESP_MAIL_CLIENT_VALID_TS && millis() - ntpStart < ntpTimeout)
  {
      delay(100);
  }

  if (time(nullptr) < ESP_MAIL_CLIENT_VALID_TS){
    Serial.println("");
    Serial.println("NTP Timeout - could not get time from server");
  } else {
    Serial.println(" OK");
  }

  Serial.println("------------------");
  voltBatt = ReadVoltage(36)*2;
  Serial.println(voltBatt,3);
  voltBoard = ReadVoltage(34);
  Serial.println(voltBoard,3);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);
  /* Declare the session config data */
  ESP_Mail_Session session;
  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";
  /* Declare the message class */
  SMTP_Message message;
  /* Set the message headers */
  message.sender.name = "ESP-Mousetrap";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Mouse was trapped";
  message.addRecipient("Recipient", RECIPIENT_EMAIL);
  /*Send HTML message*/
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Ahoj, I caught a mouse!</h1><p>- Sent from Mousetrap</p></div>";
  htmlMsg += "Battery voltage = ";
  htmlMsg += String(voltBatt,3);
  htmlMsg += " Volt";
  htmlMsg += "<p>Board voltage = ";
  htmlMsg += String(voltBoard,3);
  htmlMsg += " Volt";
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  /* Send email, retry once after 5 seconds on failure */
  bool mailSent = false;
  for (int attempt = 1; attempt <= 2 && !mailSent; attempt++){
    if (!smtp.connect(&session)){
      Serial.println("SMTP connect failed (attempt " + String(attempt) + ")");
    } else {
      if (MailClient.sendMail(&smtp, &message)){
        mailSent = true;
      } else {
        Serial.println("Error sending email (attempt " + String(attempt) + "): " + smtp.errorReason());
      }
    }
    if (!mailSent && attempt < 2){
      Serial.println("Waiting 5 seconds before retrying...");
      delay(5000);
    }
  }

  WiFi.disconnect(true);  // Disconnect WiFi so the router releases the connection immediately

  // Save power --> 13.7 µA instead of 1,400.0 µA
  //adc_power_off();  // no longer works
  //adc_power_release();  // suggested alternative, also doesn't work
  //adc_power_acquire(); // also no longer works

  // Deep sleep mode until RESET pin is connected to a LOW signal (for example pushbutton or magnetic reed switch)
  Serial.println("");
  Serial.println("I'm awake, but I'm going into deep sleep mode until a mouse wakes me up!");
  Serial.println("");
  Serial.println("");
  Serial.flush();

  digitalWrite(LED_PIN, LOW);
  esp_deep_sleep_start();
}

void loop() {
  // Never reached, as setup() always ends in deep sleep
}
