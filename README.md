# MQ-135 Gas Sensor: IoT Data Collection & Prediction
---
[![Arduino](https://img.shields.io/badge/Platform-Arduino-00979C.svg)](https://www.arduino.cc/)
[![Hardware](https://img.shields.io/badge/Hardware-MQ--135-orange.svg)](https://alphatronic.lk/wp-content/uploads/2020/09/MQ-135-Gas-Sensor-Module-for-Air-Quality-1-600x600.png)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Project Overview
This repository contains an advanced Arduino-based firmware for the **MQ-135 Air Quality Sensor**. It enables real-time gas concentration monitoring, local data logging via SD card, and a dual-mode system (Training vs. Prediction) designed to bridge the gap between embedded IoT hardware and machine learning workflows.

![MQ-135 Gas Sensor Module](https://alphatronic.lk/wp-content/uploads/2020/09/MQ-135-Gas-Sensor-Module-for-Air-Quality-1-600x600.png)

## 🔬 Sensor Physics & Working Principle

The MQ-135 is a metal-oxide semiconductor (MOS) sensor. The core sensing element is a ceramic tube coated with **Tin Dioxide (SnO2)**. 

### How it works:
1.  **Oxidation**: In clean air, oxygen is adsorbed on the SnO2 surface, trapping electrons and creating a high potential barrier (high resistance).
2.  **Reduction**: When target gases (like $NH_3$, $NO_x$, Alcohol, Benzene, Smoke, $CO_2$) are present, they react with the adsorbed oxygen, releasing electrons back into the conduction band.
3.  **Conductivity Shift**: This reduction in surface oxygen density lowers the potential barrier, increasing the sensor's conductivity (and decreasing its resistance $R_s$).

---

## 📐 Mathematical Model: PPM Calculation

The concentration of gas in parts-per-million (PPM) is calculated based on the resistance ratio $R_s/R_0$, where $R_0$ is the sensor resistance in clean air.

### 1. Calculating Sensor Resistance ($R_s$)
Using a voltage divider circuit with a load resistor $R_L$:
$$R_s = R_L \cdot \left( \frac{V_c}{V_{out}} - 1 \right)$$
*Where $V_c$ is the supply voltage (5V) and $V_{out}$ is the analog output.*

### 2. Concentration Power Law
The relationship between the resistance ratio and PPM is defined by the power law:
$$PPM = a \cdot \left( \frac{R_s}{R_0} \right)^b$$
*The constants **a** and **b** are derived from the sensitivity curves in the MQ-135 datasheet for specific gases.*

---

## 🛠️ System Features

### 1. Dynamic Parameter Loading
Unlike static implementations, this firmware reads calibration data ($R_L, R_0, a, b$, etc.) directly from a `data.txt` file on an **SD Card**. This allows for field calibration without modifying the source code.

### 2. Dual-Mode Operation
A physical switch determines the operational state:
*   **Training Mode**: The system samples PPM values at set intervals and appends them to `dataset.txt` on the SD card for offline model training.
*   **Prediction Mode**: The system waits for a trigger, performs a steady-state delay to ensure sensor stabilization, and outputs a classification result.

### 3. Visual Feedback
*   **Sensor Indicator**: Fast blinks indicate active sampling or processing.
*   **SD Indicator**: Remains active during file I/O operations to prevent data corruption.

---

## 📂 Repository Structure
*   `mq-135-machine-learning/`: Contains the main Arduino firmware (`.ino`).
*   `dataset.txt` (Generated): Raw data logged for machine learning applications.
*   `data.txt` (Required): Configuration file for sensor parameters.

## 🚀 Getting Started
1.  **Hardware Wiring**: Connect the MQ-135 to `A0`, SD module to SPI pins (`53` for CS), and mode switch to pin `44`.
2.  **Configuration**: Create a `data.txt` on your SD card with the required constants.
3.  **Deploy**: Upload the `.ino` file using the Arduino IDE.

---
*Developed for research in Air Quality Monitoring and Predictive IoT.*
