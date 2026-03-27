#include <WiFi.h>
#include <WebServer.h>

// -------- WIFI CONFIG --------
const char* ssid = "1+";
const char* password = "12344321";

WebServer server(80);
// -------- Motor Pins --------
#define ENA 4
#define IN1 1
#define IN2 2

#define ENB 5
#define IN3 9
#define IN4 10

// -------- Variables --------
int speedPWM_A = 0;
int speedPWM_B = 0;

// -------- Motor Control --------
void setMotor(int leftSpeed, int rightSpeed) {
  // LEFT MOTOR
  if (leftSpeed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (leftSpeed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    leftSpeed = -leftSpeed;
  }

  // RIGHT MOTOR
  if (rightSpeed > 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else if (rightSpeed < 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    rightSpeed = -rightSpeed;
  }

  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}

// -------- HTML UI --------
String webpage = R"====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { background:#111; color:white; text-align:center; }
.joystick {
  width:150px; height:150px;
  background:#333; border-radius:50%;
  position:relative; margin:20px auto;
}
.stick {
  width:50px; height:50px;
  background:red; border-radius:50%;
  position:absolute; top:50px; left:50px;
}
.container { display:flex; justify-content:space-around; }
</style>
</head>

<body>
<h2>ESP32 Joystick Control</h2>

<div class="container">
  <div>
    <p>Forward / Back</p>
    <div class="joystick" id="joyY">
      <div class="stick" id="stickY"></div>
    </div>
  </div>

  <div>
    <p>Left / Right</p>
    <div class="joystick" id="joyX">
      <div class="stick" id="stickX"></div>
    </div>
  </div>
</div>

<p id="status">PWM: 0 , 0</p>

<script>

let yVal = 0;
let xVal = 0;

function setupJoystick(joyId, stickId, isY) {
  let joy = document.getElementById(joyId);
  let stick = document.getElementById(stickId);

  joy.addEventListener("touchmove", function(e) {
    e.preventDefault();

    let rect = joy.getBoundingClientRect();
    let x = e.touches[0].clientX - rect.left - 75;
    let y = e.touches[0].clientY - rect.top - 75;

    x = Math.max(-50, Math.min(50, x));
    y = Math.max(-50, Math.min(50, y));

    stick.style.left = (x + 50) + "px";
    stick.style.top = (y + 50) + "px";

    if (isY) {
      yVal = -y;
    } else {
      xVal = x;
    }

    sendCommand();
  });

  joy.addEventListener("touchend", function() {
    stick.style.left = "50px";
    stick.style.top = "50px";

    if (isY) yVal = 0;
    else xVal = 0;

    sendCommand();
  });
}

function sendCommand() {
  let left = yVal + xVal;
  let right = yVal - xVal;

  left = Math.max(-100, Math.min(100, left));
  right = Math.max(-100, Math.min(100, right));

  document.getElementById("status").innerHTML =
    "PWM: " + left + " , " + right;

  fetch(`/move?l=${left}&r=${right}`);
}

setupJoystick("joyY", "stickY", true);
setupJoystick("joyX", "stickX", false);

</script>
</body>
</html>
)====";

// -------- HTTP Handlers --------
void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleMove() {
  int l = server.arg("l").toInt();
  int r = server.arg("r").toInt();

  int pwmL = map(l, -100, 100, -255, 255);
  int pwmR = map(r, -100, 100, -255, 255);

  setMotor(pwmL, pwmR);

  server.send(200, "text/plain", "OK");
}

// -------- Setup --------
void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  WiFi.softAP(ssid, password);
  Serial.println("AP Started");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/move", handleMove);

  server.begin();
}

// -------- Loop --------
void loop() {
  server.handleClient();
}