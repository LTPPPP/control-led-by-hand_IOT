// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL6a9de3M8_"
#define BLYNK_TEMPLATE_NAME "FinalProject"
#define BLYNK_AUTH_TOKEN "DmSHQm2o_g_qclPwK-7Ket6DHS4d4Dph"
#define BLYNK_FIRMWARE_VERSION "0.1.0"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>

const char *ssid = "...";
const char *password = "...";

ESP8266WebServer server(80);

// Declare pin numbers
const int ledPins[] = {D0, D7, D2, D8, D1};
const int switchPins[] = {D6, D4, D3, D5};

bool ledStates[] = {false, false, false, false, false};
bool lastSwitchStates[] = {HIGH, HIGH, HIGH, HIGH};

int d5PressCount = 0;

void handleToggleLED(int ledIndex)
{
  // sync off or on
  ledStates[ledIndex] = !ledStates[ledIndex];
  digitalWrite(ledPins[ledIndex], ledStates[ledIndex] ? HIGH : LOW);
  Blynk.virtualWrite(ledIndex + 1, ledStates[ledIndex]);
  server.send(200, "text/plain", String(ledStates[ledIndex]));
}

// Route handlers
void handleRoot()
{
  server.send(200, "text/html", R"=====(
  <!DOCTYPE html>
  <html lang="en">
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>IOT</title>
      <style>
        * {
            font-family: 'Calibri', 'Roboto', sans-serif, Helvetica;
            padding: 0;
            margin: 0;
            box-sizing: border-box;
        }
        body {
            height: 100vh;
            background-image: linear-gradient(to bottom right, #AB3035, #2E5CA7);
            background-repeat: no-repeat;
        }

        .bg-container {
            margin: 50px 0;
            animation: fade-up 1s;
        }

        .heading {
            font-size: 100px;
            width: 100%;
            padding: 10px 0;
            text-align: center;
            animation: fade-down 0.5s;
        }

        .btn-container {
            width: 100%;
            display: flex;
            justify-content: center;
            padding: 50px 0;
            animation: fade-up 0.5s;
        }

        .btn {
            user-select: none;
            font-size: 30px;
            font-weight: 600;
            color: #F5F5F5;
            background-color: #181818;
            padding: 10px 20px;
            margin: 0 45px;
            border: none;
            border-radius: 15px;
            box-shadow: 0px 2px 20px 0 rgba(0, 0, 0, 1);
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .btn:active {
            transform: scale(0.96);
        }

        .item>.btn:hover {
            opacity: 0.9;
            transform: translateY(-5px);
            box-shadow: 0 1px 20px 0 rgba(0, 0, 0, 0.05);
        }

        .btn:hover {
            opacity: 0.9;
            transform: translateY(-5px);
            box-shadow: 0 1px 20px 0 rgba(0, 0, 0, 0.05);
        }

        @keyframes fade-down {
            0% {
                opacity: 0;
                transform: translateY(-30px) scale(0.9);
            }

            100% {
                opacity: 1;
                transform: translateY(0px) scale(1);
            }
        }

        @keyframes fade-up {
            0% {
                opacity: 0;
                transform: translateY(30px) scale(0.9);
            }

            100% {
                opacity: 1;
                transform: translateY(0px) scale(1);
            }
        }

        .item>.btn:active {
            transform: scale(0.96)
        }

        .handMade-container {
            display: none;
            justify-content: center;
            padding: 50px 0;
        }

        .camera-container {
            display: none;
            justify-content: center;
        }

        .camera {
            border-radius: 10px;
            color: aliceblue;
            background-color: #181818;
            padding: 20px;
            margin: 10px;
        }
      </style>
  </head>
  <body>
      <h1 class="heading">Group 5</h1>

      <div class="btn-container">
          <button class="btn handMade-btn">
              HandMade
          </button>

          <button class="btn automatic-btn">
              Automatic
          </button>
      </div>

      <div class="handMade-container">
          <div class="item">
              <button class="btn" onclick="toggleLED(0)">Led 1</button>
          </div>

          <div class="item">
              <button class="btn" onclick="toggleLED(1)">Led 2</button>
          </div>

          <div class="item">
              <button class="btn" onclick="toggleLED(2)">Led 3</button>
          </div>

          <div class="item">
              <button class="btn" onclick="toggleLED(3)">Led 4</button>
          </div>

          <div class="item">
              <button class="btn" onclick="toggleLED(4)">Led 5</button>
          </div>
      </div>

      <div class="camera-container">
          <div class="camera">Camera here</div>
      </div>

      <script>
        let handMadeBTN = document.querySelector('.handMade-btn');
        let handMadeContainer = document.querySelector('.handMade-container');
        let camerabtn = document.querySelector('.automatic-btn');
        let camerContainer = document.querySelector('.camera-container');

        handMadeBTN.onclick = function () {
            if (handMadeContainer.style.display === 'none' || handMadeContainer.style.display === '') {
                handMadeContainer.style.display = 'flex';
                camerContainer.style.display = 'none';
            } else {
                handMadeContainer.style.display = 'none';
            }
        }

        camerabtn.onclick = function () {
            if (camerContainer.style.display === 'none' || camerContainer.style.display === '') {
                camerContainer.style.display = 'flex';
                handMadeContainer.style.display = 'none';
            } else {
                camerContainer.style.display = 'none';
            }
        }

        function toggleLED(ledIndex) {
            fetch(`/toggleLED?index=${ledIndex}`)
                .then(response => response.text())
                .then(state => {
                    const btn = document.querySelector(`.handMade-container .item:nth-child(${parseInt(ledIndex) + 1}) .btn`);
                    btn.style.backgroundColor = state === '1' ? '#FFE177' : '#181818';
                    btn.style.color = state === '1' ? '#181818' : '#F5F5F5';
                    fetch('/updateWebInterface');
                });
        }

        const socket = new WebSocket('ws://localhost:8765');

        socket.onopen = () => {
            console.log('WebSocket connection established');
        };

        socket.onmessage = (event) => {
            const frame = event.data;
            const cameraContainer = document.querySelector('.camera-container');
            const img = document.createElement('img');
            img.src = `data:image/jpeg;base64,${frame}`;
            cameraContainer.innerHTML = '';
            cameraContainer.appendChild(img);
        };

        socket.onerror = (error) => {
            console.error('WebSocket error:', error);
        };
    </script>
  </body>
  </html>
  )=====");
}

void handleToggleLEDRequest()
{
  if (server.hasArg("index"))
  {
    int index = server.arg("index").toInt();
    if (index >= 0 && index < 5)
    {
      handleToggleLED(index);
    }
  }
  else
  {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup()
{
  // Initialize LED pins
  for (int i = 0; i < 5; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
    pinMode(switchPins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < 5; i++)
  {
    Blynk.virtualWrite(i + 1, ledStates[i]);
  }

  pinMode(D0, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D4, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);

  // Start serial communication for debugging
  Serial.begin(115200);

  // Connect to WiFi
  Serial.print("Connecting to the Network");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP Address of network: ");
  Serial.println(WiFi.localIP());
  +
      // Set up server routes
      server.on("/", handleRoot);
  server.on("/toggleLED", handleToggleLEDRequest);
  server.on("/fingerstatus", HTTP_GET, handleFingerStatus);

  // Start the server
  server.begin();
  Serial.println("Server started");
  Serial.print("Copy and paste the following URL: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  // Start Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  server.on("/updateWebInterface", updateWebInterface);
}

void loop()
{
  server.handleClient(); // Handle web server requests
  Blynk.run();           // Handle Blynk

  for (int i = 0; i < 4; i++)
  {
    bool switchState = digitalRead(switchPins[i]);
    if (switchState == LOW && lastSwitchStates[i] == HIGH)
    {
      if (i == 3)
      { // D5 pin index in switchPins array
        d5PressCount++;
        switch (d5PressCount)
        {
        case 1:
          digitalWrite(ledPins[3], HIGH); // Turn on LED 4
          break;
        case 2:
          digitalWrite(ledPins[3], LOW); // Turn off LED 4
          break;
        case 3:
          digitalWrite(ledPins[4], HIGH); // Turn on LED 5
          break;
        case 4:
          digitalWrite(ledPins[4], LOW); // Turn off LED 5
          break;
        case 5:
          digitalWrite(ledPins[3], HIGH); // Turn on LED 4
          digitalWrite(ledPins[4], HIGH); // Turn on LED 5
          break;
        case 6:
          digitalWrite(ledPins[3], LOW); // Turn off LED 4
          digitalWrite(ledPins[4], LOW); // Turn off LED 5
          d5PressCount = 0;
          break;
        }
      }
      else
      {
        ledStates[i] = !ledStates[i];
        digitalWrite(ledPins[i], ledStates[i] ? HIGH : LOW);
      }
      delay(50);
    }
    lastSwitchStates[i] = switchState;
  }

  check_button();
  delay(100);
}

BLYNK_WRITE(V1)
{
  int p = param.asInt();
  Serial.print("V1 triggered: ");
  Serial.println(p);
  digitalWrite(ledPins[0], p);
  updateWebInterface();
}

BLYNK_WRITE(V2)
{
  int p = param.asInt();
  Serial.print("V2 triggered: ");
  Serial.println(p);
  digitalWrite(ledPins[1], p);
  updateWebInterface();
}

BLYNK_WRITE(V3)
{
  int p = param.asInt();
  Serial.print("V3 triggered: ");
  Serial.println(p);
  digitalWrite(ledPins[2], p);
  updateWebInterface();
}

BLYNK_WRITE(V4)
{
  int p = param.asInt();
  Serial.print("V4 triggered: ");
  Serial.println(p);
  d5PressCount++;
  switch (d5PressCount)
  {
  case 1:
    digitalWrite(ledPins[3], HIGH); // Turn on LED 4
    break;
  case 2:
    digitalWrite(ledPins[3], LOW); // Turn off LED 4
    break;
  case 3:
    digitalWrite(ledPins[4], HIGH); // Turn on LED 5
    break;
  case 4:
    digitalWrite(ledPins[4], LOW); // Turn off LED 5
    break;
  case 5:
    digitalWrite(ledPins[3], HIGH); // Turn off LED 4
    digitalWrite(ledPins[4], HIGH); // Turn off LED 5
    break;
  case 6:
    digitalWrite(ledPins[3], LOW); // Turn off LED 4
    digitalWrite(ledPins[4], LOW); // Turn off LED 5
    d5PressCount = 0;              // Reset the count
    break;
  }
  updateWebInterface();
}

void check_button()
{
  for (int i = 0; i < 4; i++)
  {
    bool switchState = digitalRead(switchPins[i]);
    if (switchState == LOW && lastSwitchStates[i] == HIGH)
    {
      if (i == 3)
      { // D5 pin index in switchPins array
        d5PressCount++;
        switch (d5PressCount)
        {
        case 1:
          digitalWrite(ledPins[3], HIGH); // Turn on LED 4
          break;
        case 2:
          digitalWrite(ledPins[3], LOW); // Turn off LED 4
          break;
        case 3:
          digitalWrite(ledPins[4], HIGH); // Turn on LED 5
          break;
        case 4:
          digitalWrite(ledPins[4], LOW); // Turn off LED 5
          break;
        case 5:
          digitalWrite(ledPins[3], HIGH); // Turn on LED 4
          digitalWrite(ledPins[4], HIGH); // Turn on LED 5
          break;
        case 6:
          digitalWrite(ledPins[3], LOW); // Turn off LED 4
          digitalWrite(ledPins[4], LOW); // Turn off LED 5
          d5PressCount = 0;
          break;
        }
      }
      else
      {
        ledStates[i] = !ledStates[i];
        digitalWrite(ledPins[i], ledStates[i] ? HIGH : LOW);
      }
      updateWebInterface(); // Sync with web interface
      delay(50);
    }
    lastSwitchStates[i] = switchState;
  }
}

void updateWebInterface()
{
  String ledStatesString = "";
  for (int i = 0; i < 5; i++)
  {
    ledStatesString += ledStates[i] ? '1' : '0';
  }
  server.send(200, "text/plain", ledStatesString);
}

void handleFingerStatus()
{
  String state = server.arg("state");
  Serial.println("Received state: " + state);

  // Parse the state and control LEDs
  for (int i = 0; i < 5; i++)
  {
    if (state[i] == '1')
    {
      digitalWrite(ledPins[i], HIGH); // Turn on the LED
    }
    else
    {
      digitalWrite(ledPins[i], LOW); // Turn off the LED
    }
  }

  server.send(200, "text/plain", "State received: " + state);
}