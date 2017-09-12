#include "BTComms.h"

#include "Arduino.h"

static const int messageBufferLength = 20;
static unsigned char message[messageBufferLength];

/**
 * Bluetooth communications constructor
 */
BTComms::BTComms() {
	messageIndex = 0;
	messageLength = 0;
	BTstate = kLookingForStart;
}

/**
 * Code that is called from the arduino setup() function
 * This initializes things that cannot be set up from the constructor.
 */
void BTComms::setup() {
  Serial3.begin(115200);
}

/**
 * Send a message to the RCS that has 3 values (source, dest, data)
 */
void BTComms::writeMessage(unsigned char b1, unsigned char b2, unsigned char b3) {
  Serial3.write(kMessageStart);
  Serial3.write(5);
  Serial3.write(b1);
  Serial3.write(b2);
  Serial3.write(b3);
  Serial3.write(0xff - (b1 + b2 + b3 + 5));
}

/**
 * Get the length of the currently received message
 * @returns int The number of bytes in the received message
 */
int BTComms::getMessageLength() {
  return messageLength;
}

/**
 * Get a byte from the current message
 * Retrieve a byte from the currently received message. Only a single message is
 * remembered at any time, so you have to call read(), notice that there is a message,
 * and then do something with the message bytes.
 * @param index The offset (zero-based) to the byte to be returned
 * @returns unsigned char The byte that is at the specified index
 */
unsigned char BTComms::getMessageByte(unsigned index) {
  if (index >= messageLength) {
    Serial.print("request for message byte beyond end of message ");
    Serial.println(index);
    return 0;
  }
  return message[index];  
}

/**
 * Read a message from Bluetooth
 * This method reads messages from Bluetooth by looking for the message start byte, then
 * reading the message length and data.
 *
 * You should probably modify this to ignore messages with invalid checksums!
 */
bool BTComms::read() {
  while (Serial3.available()) {
    unsigned inByte = Serial3.read();
    switch (BTstate) {
      case kLookingForStart:
        if (inByte != kMessageStart)
        	break;
        BTstate = kReadingMessageLength;
        break;
      case kReadingMessageLength:
        messageLength = inByte - 1;
        if (messageLength >= messageBufferLength) {
          Serial.println("Received message length greater than buffer size");
          BTstate = kLookingForStart;
          break;
        }
        messageIndex = 0;
        BTstate = kReadMessage;
        break;
      case kReadMessage:
        message[messageIndex++] = inByte;
        if (messageIndex >= messageLength) {
          BTstate = kLookingForStart;
          return true;
        }
        break;
       default:
        Serial.println("Invalid state");
    }
  }
  return false;
}

