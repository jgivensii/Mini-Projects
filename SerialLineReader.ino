class SerialLineReader {
private:
String inBuffer;
void dispatch(String line) {
line.remove(line.length()-1);
if (line == "DEVICE") {
Serial.println(FS(DEVICE_ID));
return;
}
else if (line == "SERVICE") {
Serial.println(FS(SERVICE_ID));
return;
}
else if (line == "VERSION") {
Serial.println(FS(VERSION_ID));
return;
}
else if (line == "REPORT") {
doReport();
}
else if (line == "RUN" ) {
RUNNING = 1;
}
if (line == "STOP" ) {
RUNNING = 0;
}
//else if (line == "KEY") {
//Serial.println("REPORT, TIME(s), DELTA_TIME,",
//"(s),N_SAMPLES,AVERAGE,RMS,ERROR,INTEGRATED_AVERAGE,",
//"INTEGRATED_RMS,INTEGRATED_ERROR");
//}
//else if (line == "HELP") {
//Serial.println("REPORT / RUN / STOP / KEY / DEVICE /",
//"SERVICE / VERSION") }
else {
Serial.println(line);
}
}
public:
SerialLineReader() {
inBuffer.reserve(64);
}
void consumeBytes() {
while (Serial.available() > 0) {
char inChar = Serial.read();
String line;
inBuffer += inChar;
if (inChar == '\n') {
dispatch(inBuffer);
inBuffer = "";
}
}
}
};
//----------------------------------------------------------------------------
void serialEventRun(void) {
Timer1.detachInterrupt();
serialEvent();
Timer1.attachInterrupt(reportRadiometerValues);
}
SerialLineReader *serialLineReader;
void serialEvent() {
serialLineReader->consumeBytes();
}
void serialSetup() {
// start serial port at 9600 bps and wait for port to open:
Serial.begin(115200);
while (!Serial) {
; // wait for serial port to connect. Needed for native USB port only
}
serialLineReader = new(SerialLineReader);
}
void timerSetup() {
Timer1.initialize(PERIOD);
Timer1.attachInterrupt(reportRadiometerValues);
}
void fastADC(){
// Fast ADC
ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear prescaler bits
// uncomment as required
// ADCSRA |= bit (ADPS0); // 2
// ADCSRA |= bit (ADPS1); fastN = 8000; // 4
// ADCSRA |= bit (ADPS0) | bit (ADPS1); // 8
ADCSRA |= bit (ADPS2); // 16
// ADCSRA |= bit (ADPS0) | bit (ADPS2); // 32
// ADCSRA |= bit (ADPS1) | bit (ADPS2); // 64
// ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2); // 128
}
volatile float M0, M1;
volatile float S0, S1;
volatile float a0, a1;
volatile long count, theCount, t0, t1, dt;
volatile float analog0, analog1;
volatile float tmpM0, tmpM1;
volatile float reportA0Value, reportA1Value;
volatile float rmsA0Value, rmsA1Value;
volatile float errA0Value, errA1Value;
void reportRadiometerValues() {
reportA0Value, reportA1Value;
rmsA0Value, rmsA1Value;
errA0Value, errA1Value;
// compute the averages, rms’s, and errors
reportA0Value = abs(a0 / count);
reportA1Value =abs(a1 / count);
rmsA0Value = sqrt(S0/(count-2));
rmsA1Value = sqrt(S1/(count-2));
errA0Value = rmsA0Value / sqrt(count-1);
errA1Value = rmsA1Value / sqrt(count-1);
theCount = count*fastN;
t1 = millis();
dt = t1 - t0;
resetRadiometer();
}
void doReport() {
Timer1.detachInterrupt();
// send results to serial port
Serial.print("REPORT");
Serial.print(DELIMITER);
Serial.print(millis()/1000.0f,4);
Serial.print(DELIMITER);
Serial.print(dt/1000.0f,4);
Serial.print(DELIMITER);
Serial.print(theCount);
Serial.print(DELIMITER);
Serial.print(reportA0Value,4);
Serial.print(DELIMITER);
Serial.print(rmsA0Value,4);
Serial.print(DELIMITER);
Serial.print(errA0Value,4);
Serial.print(DELIMITER);
Serial.print(reportA1Value,4);
Serial.print(DELIMITER);
Serial.print(rmsA1Value,4);
Serial.print(DELIMITER);
Serial.println(errA1Value,4);
Timer1.setPeriod(PERIOD);
Timer1.attachInterrupt(reportRadiometerValues);
}
void resetRadiometer() {
M0 = 0.0; M1 = 0.0;
S0 = 0.0; S1 = 0.0;
a0 = 0.0; a1 = 0.0;
count = 0;
t0 = millis();
}
void setup() {
fastADC();
serialSetup();
timerSetup();
resetRadiometer();
RUNNING = 1;
}
// the loop routine runs over and over again forever:
void loop() {
// read the photodiode’s inputs on analog pins 0 & 1
// analog pin 0 is the amplifier raw
// analog pin 1 is the amplifier averaged
Timer1.detachInterrupt();
count++;
//analog0 = (float) analogRead(0);
analog0 = 0.0;
for (int i=0;i<fastN;i++) {
analog0=analog0+analogRead(0);
}
analog0 = analog0/float(fastN);
analog1 = 0.0;
for (int i=0;i<fastN;i++) {
analog1=analog1+analogRead(1);
}
analog1 = analog1/float(fastN);
//analog1 = (float) analogRead(1);
a0 = a0 + analog0;
a1 = a1 + analog1;
tmpM0 = M0; tmpM1 = M1;
M0+= (analog0 - tmpM0) / count;
M1+= (analog1 - tmpM1) / count;
S0+= (analog0 - tmpM0) * (analog0 - M0);
S1+= (analog1 - tmpM1) * (analog1 - M1);
if ( RUNNING == 1 && count == 1 ) {
doReport();
}
Timer1.attachInterrupt(reportRadiometerValues);
}
