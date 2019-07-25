//============================================================================
// Name        : testCAN.cpp
// Author      : HEnrique
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <array>
#include <vector>
#include <map>

enum ElementType
{
    TIME,
    ID,
    PAYLOAD,
    LENGTH,
    TYPE,
    NAME,
};
/* 						 														CAN Active	CAN Active
 * Address Frequency	Content													Messages	Messages
 * 																				(High Word)	(Low Word)
 * 	0x0A0	 10 Hz		Temperatures #1												0x0000 	0x0001
 *	0x0A1	 10 Hz		Temperatures #2												0x0000 	0x0002
 *	0x0A2	 10 Hz		Temperatures #3												0x0000 	0x0004
 *	0x0A3	100 Hz		Analog Inputs Voltages										0x0000 	0x0008
 *	0x0A4	100 Hz		Digital Input Status										0x0000 	0x0010
 *	0x0A5	100 Hz		Motor Position Information									0x0000 	0x0020
 *	0x0A6	100 Hz		Current Information											0x0000 	0x0040
 *	0x0A7	100 Hz		Voltage Information											0x0000 	0x0080
 *	0x0A8	100 Hz		Flux Information											0x0000 	0x0100
 *	0x0A9	 10 Hz		Internal Voltages											0x0000	0x0200
 *	0x0AA	100 Hz		Internal States												0x0000 	0x0400
 *	0x0AB	 10 Hz		Fault Codes													0x0000 	0x0800
 *	0x0AC	100 Hz		Torque & Timer Information									0x0000	0x1000
 *	0x0AD	100 Hz		Modulation Index & Flux Weakening Output Information		0x0000 	0x2000
 *	0x0AE	 10 Hz		Firmware Information										0x0000 	0x4000
 *	0x0AF	100 Hz		Diagnostic Data												0x0000 	0x8000
 */
enum MessageType
{
    UNDEFINED		        = 0,
    TEMPERATURES_1 	        = 0x0A0,
    TEMPERATURES_2	        = 0x0A1,
    TEMPERATURES_3 	        = 0x0A2,
    ANALOGIC_IN 	        = 0x0A3,
    DIGITAL_IN		        = 0x0A4,
    MOTOR_POSITION	        = 0x0A5,
    CURRENT_INFO	        = 0x0A6,
    VOLTAGE_INFO	        = 0x0A7,
    FLUX_INFO   	        = 0x0A8,
    INTERN_VOLTS 	        = 0x0A9,
    INTERN_STATES	        = 0x0AA,
    FAULT_CODES 	        = 0x0AB,
    TORQUE_TIMER_INFO	    = 0x0AC,
    MOD_FLUX_WEAK_OUT_INFO	= 0x0AD,
    FIRM_INFO	            = 0x0AE,
    DIAGNOSTIC_DATA	        = 0x0AF
};

enum DataType
{
    TEMPERATURE,
    VOLTAGE,
    TORQUE,
    CURRENT,
    ANGLE,
    ANG_VELOCITY,
    BOOLEAN_,
    FREQUENCY,
    POWER,
    TIME_,
    FLUX,
    P_GAIN,
    I_GAIN,
    D_GAIN,
    LPF_GAIN
};

/*"Celsius", "oC"

"volt", "V"

"newton-meter", "N.m"

"Ampere", "A"

"degrees", "o"

"rot/min", "rpm"

"bool", ""

"hertz", "Hz"

"watt", "W"

"second", "s"

"weber", "w"*/

class CANMessage : public std::map<std::string, float>
{
protected:
    MessageType m_messageType;
    std::array<int, 8> m_rawPayload;
    float m_time;
    bool m_isInitialized;
    std::map<std::string, float> m_map_float;
    std::map<std::string, bool> m_map_bool;

    int calculateFromBytes(int byteIndex) const
    {
        int result;

        if(byteIndex < 7)
        {
            result =  m_rawPayload.at(byteIndex) + m_rawPayload.at(byteIndex + 1) * 16 * 16;
        }
        else
        {
            std::cout << "there is no next byte!";
            exit(0);
        }

        return result;
    }

public:
    CANMessage(MessageType messageType, std::array<int, 8> payload, float time = 0)
            : m_messageType(messageType), m_rawPayload(payload), m_time(time), m_isInitialized(true)
    {
    }

    CANMessage(MessageType messageType, float time = 0)
            : m_messageType(messageType), m_time(time), m_isInitialized(false)
    {
        m_rawPayload = {0,0,0,0,0,0,0,0};
    }

    CANMessage()
            : m_messageType(MessageType::UNDEFINED), m_rawPayload({0,0,0,0,0,0,0,0}), m_time(0.0), m_isInitialized(false)
    {
    }

    MessageType getMessageType() const
    {
        return m_messageType;
    }

    std::array<int, 8> getRawPayload() const
    {
        return m_rawPayload;
    }

    int getByte(int byteIndex) const
    {
        if(byteIndex >= 0 && byteIndex < 8 && m_isInitialized == true)
        {
            return m_rawPayload.at(byteIndex);
        }

        return -1;
    }

    bool getIsInitialized() const
    {
        return m_isInitialized;
    }

    static std::string MessageTypeToString(MessageType messageType)
    {
        std::string result;

        switch(messageType)
        {
            case MessageType::TEMPERATURES_1:
                result = "Temperatures #1";
                break;
            case MessageType::TEMPERATURES_2:
                result = "Temperatures #2";
                break;
            case MessageType::TEMPERATURES_3:
                result = "Temperatures #3";
                break;
            case MessageType::ANALOGIC_IN:
                result = "Analog Inputs Voltages\t";
                break;
            case MessageType::DIGITAL_IN:
                result = "Digital Input Status";
                break;
            case MessageType::MOTOR_POSITION:
                result = "Motor Position Information";
                break;
            case MessageType::CURRENT_INFO:
                result = "Current Information";
                break;
            case MessageType::VOLTAGE_INFO:
                result = "Voltage Information";
                break;
            case MessageType::FLUX_INFO:
                result = "Flux Information";
                break;
            case MessageType::INTERN_VOLTS:
                result = "Internal Voltages";
                break;
            case MessageType::INTERN_STATES:
                result = "Internal States";
                break;
            case MessageType::FAULT_CODES:
                result = "Fault Codes";
                break;
            case MessageType::TORQUE_TIMER_INFO:
                result = "Torque & Timer Information";
                break;
            case MessageType::MOD_FLUX_WEAK_OUT_INFO:
                result = "Modulation Index & Flux Weakening Output Information";
                break;
            case MessageType::FIRM_INFO:
                result = "Firmware Information";
                break;
            case MessageType::DIAGNOSTIC_DATA:
                result = "Diagnostic Data";
                break;
            case MessageType::UNDEFINED:
                result = "Undefined";
                break;
        }

        return result;
    }
};

class CurrentInfoMessage: public CANMessage
{
private:
    int m_phaseACurrent;
    int m_phaseBCurrent;
    int m_phaseCCurrent;
    int m_dcBusCurrent;

    int calculateFromBytes(int byteIndex) const
    {
        int result;

        if(byteIndex < 7)
        {
            result =  m_rawPayload.at(byteIndex) + m_rawPayload.at(byteIndex + 1) * 16 * 16;
        }
        else
        {
            std::cout << "there is no next byte!";
            exit(0);
        }

        return result;
    }

public:
    CurrentInfoMessage(std::array<int, 8> payload, float time = 0)
            : CANMessage(MessageType::CURRENT_INFO, payload, time),
            m_phaseACurrent(calculateFromBytes(0)/10),
            m_phaseBCurrent(calculateFromBytes(2)/10),
            m_phaseCCurrent(calculateFromBytes(4)/10),
            m_dcBusCurrent(calculateFromBytes(6)/10)
    {
        if(m_messageType != MessageType::CURRENT_INFO)
        {
            throw "Unwanted message type: " + m_messageType;
        }
    }

    CurrentInfoMessage(MessageType messageType, float time = 0)
            : CANMessage(MessageType::CURRENT_INFO, time), m_phaseACurrent(0), m_phaseBCurrent(0), m_phaseCCurrent(0), m_dcBusCurrent(0)
    {
        if(m_messageType != MessageType::CURRENT_INFO)
        {
            throw "Unwanted message type: " + m_messageType;
        }
    }

    CurrentInfoMessage()
            : CANMessage(MessageType::CURRENT_INFO), m_phaseACurrent(0), m_phaseBCurrent(0), m_phaseCCurrent(0), m_dcBusCurrent(0)
    {
    }

    int getPhaseACurrent() const
    {
        int phaseA = this->calculateFromBytes(0);
        phaseA = phaseA / 10;
        return phaseA;
    }
};

int strhex_to_int(const std::string& strhex)
{
    /*
    for(char& c : str) {
        do_things_with(c);
    }
    */

    int result = 0;
    int auxInt;

    for(int charIndex = strhex.length()-1; charIndex >= 0; charIndex--)
    {
        switch(strhex.at(charIndex))
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                auxInt = strhex.at(charIndex) - '0';
                break;
            case 'A': case 'a':
                auxInt = 10;
                break;
            case 'B': case 'b':
                auxInt = 11;
                break;
            case 'C': case 'c':
                auxInt = 12;
                break;
            case 'D': case 'd':
                auxInt = 13;
                break;
            case 'E': case 'e':
                auxInt = 14;
                break;
            case 'F': case 'f':
                auxInt = 15;
                break;
            default:
                std::cout << "NOT A HEX! (unwanted: " << strhex.at(charIndex) << ")" << std::endl;
                return 0;
                break;
        }

        result += auxInt * pow(16, strhex.length() - 1 - charIndex);
    }

    return result;
}

int main()
{
    std::ifstream canFile("../payload_good.txt");

    if (!canFile)
    {
        // Print an error and exit
        std::cerr << "Uh oh, payload_good.txt could not be opened for reading!" << std::endl;
        exit(1);
    }

    std::string line;

    bool isFirstLine = true;

    int counter = 0;

    std::stringstream ssLine;

    std::string lineElement;

    int lineElementCounter;

    int auxID;

    std::vector<CANMessage> messagesList;

    while(canFile && counter < 20)
    {

        getline(canFile, line);

        if(isFirstLine)
        {
            isFirstLine = false;
            std::cout << line << std::endl;
        }
        else
        {
            counter++;

            std::cout 	<< "linha " << std::setw(3) << counter << " (" << std::setw(5) << line.length() << ")"
                         << std::endl << line << std::endl;

            ssLine.str(line);
            ssLine.clear();

            lineElementCounter = 0;

            while(getline(ssLine, lineElement, '\t'))
            {
                if(lineElementCounter == 1)
                {
                    auxID = std::stoi(lineElement);
                    std::cout << lineElement << "(" << std::showbase << std::hex << std::uppercase << auxID << std::dec << ") ";

                    switch(auxID)
                    {
                        case MessageType::TEMPERATURES_1:
                            std::cout << "Temperatures #1";
                            break;
                        case MessageType::TEMPERATURES_2:
                            std::cout << "Temperatures #2";
                            break;
                        case MessageType::TEMPERATURES_3:
                            std::cout << "Temperatures #3";
                            break;
                        case MessageType::ANALOGIC_IN:
                            std::cout << "Analog Inputs Voltages";
                            break;
                        case MessageType::DIGITAL_IN:
                            std::cout << "Digital Input Status";
                            break;
                        case MessageType::MOTOR_POSITION:
                            std::cout << "Motor Position Information";
                            break;
                        case MessageType::CURRENT_INFO:
                            std::cout << "Current Information";
                            messagesList.push_back(CurrentInfoMessage());
                            break;
                        default:
                            std::cout << "-> unkown <-";
                            break;
                    }
                }
                lineElementCounter++;
            }

            std::cout << std::endl;
        }
    }

    canFile.close();

}
