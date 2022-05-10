#include "MIDIUSB.h"

int sensitivity = 2;                                                                                          // Мъртва зона за потенциометъра
int MUXpins[] = {2, 3, 4};                                                                                    // Пиновете, които превключват мултиплексор 1
int MUXpins2[] = {5, 6, 7};                                                                                   // Пиновете, които превключват мултиплексор 2
int potsCount = 8;                                                                                            // Броя на потенциометри за всеки мултиплексор
int pots[8];                                                                                                  // Сегашните стойности на потенциометрите
int pots2[8];                                                                                                 // Сегашните стойности на потенциометрите
int potsLastVal[] = {0, 0, 0, 0, 0, 0, 0, 0};                                                                 // Последните стойности на потенциометрите
int potsLastVal2[] = {0, 0, 0, 0, 0, 0, 0, 0};                                                                // Последните стойности на потенциометрите
int multiplexerInputPin = A0;                                                                                 // Входен пин на аналогово-цифрофия преобразувател на микроконтролера
int multiplexerInputPin2 = A1;                                                                                // Входен пин на аналогово-цифрофия преобразувател на микроконтролера
int MUXtable[][3] = {{0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}}; // Стойностите, с които се превключва мултиплексора
int potParameters[] = {4, 5, 6, 7, 8, 9, 10, 11};                                                             // MIDI параметри, които се променят
int potParameters2[] = {12, 13, 14, 15, 16, 17, 18};                                                          // MIDI параметри, които се променят

// Функцията, която изпраща MIDI съобщението към компютъра
// channel - MIDI каналът, който се използва
// control - Съответния MIDI параметър, който се променя
// value - Самата стойност на MIDI параметъра
void controlChange(byte channel, byte control, byte value)
{
    midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
    MidiUSB.sendMIDI(event);
}

void setup()
{
    // Определя съответните пинове на мултиплексора като output пинове
    for (int i = 0; i < potsCount; i++)
    {
        pinMode(MUXpins[i], OUTPUT);
        pinMode(MUXpins2[i], OUTPUT);
    }

    // Определя съответните пинове на мултиплексора като входяхщи пинове
    // Тяхната стойност не може да бъде зададена, а само да се чете
    pinMode(multiplexerInputPin, INPUT);
    pinMode(multiplexerInputPin2, INPUT);
}

void loop()
{
    // На всяка итерация на цикъла се превключва входа на мултиплексора
    // Чете се стойноста (напрежението) на потенциометъра подадена на този вход
    // И се записва в масива, като ако стойността на съответния потенциометър
    // е различна от предишната, то се изпраща съобщение към компютъра

    for (int i = 0; i < 8; i++)
    {
        digitalWrite(MUXpins[0], MUXtable[i][0]);
        digitalWrite(MUXpins[1], MUXtable[i][1]);
        digitalWrite(MUXpins[2], MUXtable[i][2]);

        // Числото се превърща от 10 в 7-битово число
        pots[i] = analogRead(multiplexerInputPin) >> 3;

        if (potsLastVal[i] > pots[i] + sensitivity || potsLastVal[i] < pots[i] - sensitivity)
        {
            controlChange(0x01, potParameters[i], pots[i]);
            potsLastVal[i] = pots[i];
        }

        // Подсигурява, че всички чакащи MIDI съобщения са изпратени
        MidiUSB.flush();
    }

    for (int i = 0; i < 7; i++)
    {
        digitalWrite(MUXpins2[0], MUXtable[i][0]);
        digitalWrite(MUXpins2[1], MUXtable[i][1]);
        digitalWrite(MUXpins2[2], MUXtable[i][2]);

        pots2[i] = analogRead(multiplexerInputPin2) >> 3;

        if (potsLastVal2[i] > pots2[i] + sensitivity || potsLastVal2[i] < pots2[i] - sensitivity)
        {
            controlChange(0x01, potParameters2[i], pots2[i]);
            potsLastVal2[i] = pots2[i];
        }

        MidiUSB.flush();
    }
}