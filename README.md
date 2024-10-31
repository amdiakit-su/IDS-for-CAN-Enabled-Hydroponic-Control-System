# CAN-Enabled Hydroponic Control System with Intrusion Detection

This project aims to investigate and enhance the security of hydroponic control systems that use Controller Area Network (CAN) protocols for improved data efficiency. Hydroponic systems offer significant potential for addressing global food security needs, especially as urbanization reduces arable land. By integrating CAN-enabled monitoring, these systems can achieve high efficiency in data transfer between components; however, CAN’s inherent lack of security creates potential vulnerabilities. This project introduces an Intrusion Detection System (IDS) to monitor and mitigate unauthorized access and data manipulation on CAN-enabled hydroponic systems.

## Completed Sections
1. **Hardware Setup**: The current model consists of:
   - Two Arduino 4 boards (WiFi  and Minima)
   - An ESP32 controller for monitoring traffic between the Arduinos
   - Various sensors and components to simulate real hydroponic monitoring needs.
2. **CAN Network Integration**: CAN communication has been established between the Arduino boards, with the ESP32 monitoring traffic to demonstrate easy (eavesdropping).

## Next Steps
- **Intrusion Detection Development**: Introduce the IDS to detect and log specific intrusion patterns by analyzing traffic anomalies such as unauthorized message IDs and irregular message frequencies.
- **Extended Testing**: Evaluate IDS effectiveness across different based on logged and flagged events.

