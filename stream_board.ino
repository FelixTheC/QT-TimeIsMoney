const char* start_UUID = "d3b3ecc2-ced7-461a-ac96-04f6d99d9d34";
const char* end_UUID = "2e11f26e-d155-42d7-be96-d8dec1e6c69e";
const int start_btn = 7;
const int stop_btn = 0;
volatile int start = -1;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  while (!Serial) {}  // wait for serial port to connect. Needed for native USB port only
  
  attachInterrupt(digitalPinToInterrupt(start_btn), start_btn_write, RISING);
  attachInterrupt(digitalPinToInterrupt(stop_btn), stop_btn_write, RISING);
}

void loop() 
{
}

void start_btn_write()
{
  Serial.println(start_UUID);
}

void stop_btn_write()
{
  Serial.println(end_UUID);
}
