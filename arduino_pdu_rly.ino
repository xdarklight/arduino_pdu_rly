/* SPDX-License-Identifier: MIT */

static const char relayPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
static unsigned char numRelayPins = sizeof(relayPins) / sizeof(relayPins[0]);

static int cmd = -1;

enum relayState {
  OFF,
  ON
};

void updateRelayState(unsigned char port, relayState state)
{
    if (port >= numRelayPins)
        return;

    if (state == ON)
        digitalWrite(relayPins[port], LOW);
    else
        digitalWrite(relayPins[port], HIGH);
}

bool isRelayOn(unsigned char port)
{
    if (port >= numRelayPins)
        return false;

    if (digitalRead(relayPins[port]) == LOW)
        return true;
    else
        return false;
}

void errorBlink()
{
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
}

void process(int cmd, int arg)
{
    char buf[8];
    int i;

    switch (cmd) {
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
            /* 1-byte response: 0xff input is powered, 0x0 = input is not powered */
            if (cmd - 0x18 >= numRelayPins)
                Serial.print(0x0);
            else
                Serial.print(0xff);
            break;

        case 0x19:
            /* 1-byte response, bit-mask of inputs which are powered */
            buf[0] = 0;
            for (i = 0; i < 8; i++) {
                if (i < numRelayPins)
                    buf[0] |= 1 << i;
            }
            Serial.write(buf, 1);
            break;

        case 0x1a:
            /* 8-byte response, 1 byte per input: 0xff input is powered, 0x0 = input is not powered */
            for (i = 0; i < 8; i++) {
                if (i >= numRelayPins)
                    buf[i] = 0x0;
                else
                    buf[i] = 0xff;
            }
            Serial.write(buf, 8);
            break;

        case 0x38:
            /* 8-byte response: unique identifier (TODO: this is not unique yet) */
            buf[0] = 'A';
            buf[1] = 'R';
            buf[2] = 'D';
            buf[3] = 'U';
            buf[4] = '-';
            buf[5] = 'R';
            buf[6] = 'L';
            buf[7] = 'Y';
            Serial.write(buf, 8);
            break;

        case 0x5a:
            /* 2-byte response: module ID (2 relays = 33 DEC, 8 relays = 12 DEC) and software version */
            if (numRelayPins == 8)
                buf[0] = 12;
            else
                buf[0] = 33;

            buf[1] = 0x1;
            Serial.write(buf, 2);
            break;

        case 0x5b:
            /* 1-byte reponse: each bit represents one relay state */
            buf[0] = 0;
            for (i = 0; i < numRelayPins; i++) {
                if (isRelayOn(i))
                    buf[0] |= 1 << i;
            }

            Serial.write(buf, 1);
            break;

        case 0x5c:
            /* no response, the bit-mask from the next byte decides which relays are on/off */
            for (i = 0; i < numRelayPins; i++)
                updateRelayState(i, (arg & (1 << i)) ? ON : OFF);
            break;

        case 0x64:
            /* no response, turn on all relays */
            for (i = 0; i < numRelayPins; i++)
                updateRelayState(i, ON);
            break;

        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
            /* no response, turn on the relay based on the command (0x65 = 0, 0x66 = 1, ...) */
            updateRelayState(cmd - 0x65, ON);
            break;

        case 0x6e:
            /* no response, turn off all relays */
            for (i = 0; i < numRelayPins; i++)
                updateRelayState(i, OFF);
            break;

        case 0x6f:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
            /* no response, turn off the relay based on the command (0x6f = 0, 0x70 = 1, ...) */
            updateRelayState(cmd - 0x6f, OFF);
            break;

        default:
            /* unsupported command */
            errorBlink();
            break;
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    for (int i = 0; i < numRelayPins; i++) {
        /*
         * first disable the relay to make sure we're not leaking
         * current/voltage during power-on.
         * NOTE: we cannot check the status here because
         * the pin is still in INPUT mode (set by the bootloader
         * and we always read 0 here).
         */
        updateRelayState(i, OFF);

        /* only then configure the port as OUTPUT */
        pinMode(relayPins[i], OUTPUT);
    }

    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(9600);
}

void loop()
{
    bool hasError;
    int i, cmd, arg;

    i = 0;
    arg = 0;
    hasError = false;

    while (Serial.available()) {
        if (i == 0)
            cmd = Serial.read();
        else if (i == 1)
            arg = Serial.read();

        i++;
    }

    if (i == 0)
        /* no data */
        return;
    else if (i > 1 && cmd != 0x5c)
        /* too much data */
        errorBlink();
    else if (i != 2 && cmd == 0x5c)
        /* missing argument for cmd 0x5c */
        errorBlink();
    else
        process(cmd, arg);
}
