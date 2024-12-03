# CAN-Enabled Hydroponic Control System with Intrusion Detection

This project aims to investigate and enhance the security of hydroponic control systems that use Controller Area Network (CAN) protocols for improved data efficiency. Hydroponic systems offer significant potential for addressing global food security needs, especially as urbanization reduces arable land. By integrating CAN-enabled monitoring, these systems can achieve high efficiency in data transfer between components. However, CAN’s inherent lack of security exposes these systems to risks such as unauthorized access and data manipulation.

To address these vulnerabilities, this project developed a Centralized Monitoring ECU that functions as a lightweight Intrusion Detection System (IDS). This ECU monitors CAN traffic, validates messages against a list of authorized CAN IDs, and logs unauthorized activity in a simulated hydroponic control network. By combining centralized monitoring with a lightweight permission system, this project introduces a practical approach to enhancing security while laying the groundwork for more robust mechanisms in the future.

## Completed Sections
1. **Hardware Setup**: The current model consists of:
   - Two Arduino R4 WiFi boards serving as authorized (monitoring) and unauthorized ECUs.
   - One R4 Minima board acting as the sender (authorized) ECU.
   - Various sensors and components to simulate real hydroponic monitoring needs(highlighted for simiplicity is temperature and humidity).
2. **Implemented Intrusion Detection System**:
   - Unauthorized messages are detected by comparing their CAN IDs against a predefined list of authorized IDs and flagged as potential intrusions.
   - Authorized messages are processed and their data.
## Future Work
- **Challenge-Response Mechanism Implementation**: Develop and test a dynamic challenge-response system to strengthen the permission system:
   - A central monitoring ECU sends random challenges to authorized nodes.
   - Responses are calculated based on a shared secret or predefined algorithm, ensuring only authorized nodes can reply correctly.
   - This approach prevents an intruder ECU from simply replaying or mimicking legitimate messages.
- **Extended Testing**: Evaluate IDS effectiveness across different based on logged and flagged events.

## Conclusion
This project has successfully laid the groundwork for securing CAN-enabled hydroponic systems. The current implementation demonstrates how a Centralized Monitoring ECU can detect unauthorized messages. Future work aims to enhance this system with dynamic security mechanisms, making it more robust against real-world threats.

