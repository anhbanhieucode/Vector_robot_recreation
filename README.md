# Vector Robot Recreation: Project "Shadow"

![Python](https://img.shields.io/badge/Python-3.8%2B-blue?logo=python&logoColor=white)
![C++](https://img.shields.io/badge/Firmware-Arduino%2FESP32-00979D?logo=arduino&logoColor=white)
![AI](https://img.shields.io/badge/AI-YOLO%20%7C%20LLM-orange)
![Status](https://img.shields.io/badge/Status-Development-yellow)

**A DIY open-source recreation of the Anki Vector robot, powered by modern AI.**

This project aims to replicate the capabilities of the Vector robot using off-the-shelf hardware (ESP32-CAM) and a powerful Python backend. It features **"Shadow,"** a witty AI assistant capable of real-time object detection, depth estimation, and natural language conversation with emotional context. The LLM is taken from Ollama 3 API.

---

## Table of Contents
- [Overview](#-overview)
- [System Architecture](#-system-architecture)
- [Key Features](#-key-features)
- [Hardware Requirements](#-hardware-requirements)
- [Project Structure](#-project-structure)
- [Installation & Setup](#-installation--setup)
- [Usage](#-usage)

---

## Overview

Unlike the original Vector which relied on closed-source internal processing, "Shadow" uses a **hybrid architecture**:
1.  **The Body (ESP32):** Handles low-level hardware control (camera streaming, motors, sensors).
2.  **The Brain (Python/PC):** Processes complex AI tasks including Object Detection (YOLO), Depth Estimation, and the LLM-based personality engine.

The goal is to create a smart desktop companion that is fully hackable and extensible.

---

## Key Features

* ** LLM-Powered Personality:**
    * Integrated with Large Language Models to create "Shadow"—a friendly, witty assistant.
    * **Emotion Engine:** The AI analyzes the conversation and outputs emotion tags (`Happy`, `Sad`, `Angry`, `Cringe`, etc.) to trigger physical reactions.
* ** Advanced Vision:**
    * **YOLO Object Detection:** Real-time identification of objects in the robot's field of view.
    * **Depth Estimation & Optical Flow:** Allows the robot to understand 3D space and movement.
* ** Hybrid Control System:**
    * Offloads heavy computation to a host PC while maintaining low-latency control via the ESP32.

---

## Project Structure

```plaintext
Vector_robot_recreation/
├── AI_test_v5/               # Firmware (ESP32-CAM)
│   ├── AI_test_v5.ino        # Main Arduino sketch for the robot hardware
│   ├── camera_index.h        # HTML/Web interface data
│   └── camera_pins.h         # Pin definitions for the ESP32-CAM
│
├── robot_code/               # The "Brain" (Python Host)
│   ├── main.py               # Entry point: Handles the loop, user input, and threads
│   ├── llm_interface.py      # Communicates with the LLM API
│   ├── yolo_detector.py      # Computer Vision thread (Object Detection)
│   ├── depth_estimation.py   # Monocular depth estimation logic
│   ├── optical_flow.py       # Motion tracking
│   ├── motor_control.py      # Sends movement commands to the ESP32
│   ├── planner.py            # Robot behavior planning
│   └── config.py             # Configuration settings (API keys, ports)
│
└── README.md
```
## Hardware Requirements

**Microcontroller:** ESP32-CAM (or similar ESP32 based board).
**Camera:** OV2640 (standard with ESP32-CAM).
**Motors:** DC Motors or Servos for wheels and head movement.
**Drivers:** L298N or similar motor driver.
**Host Machine:** A PC or Raspberry Pi 4/5 running Python.

## Installation & Setup

**1. Firmware Setup (ESP32)**
* Open AI_test_v5/AI_test_v5.ino in the Arduino IDE.

* Install the ESP32 Board Manager and required libraries.

* Select "AI Thinker ESP32-CAM" as the board.

* Upload the code to your ESP32.

* Note: Check the Serial Monitor to get the IP address of the robot.

**2. Software Setup (Python Brain)**
* Navigate to the robot code directory:

```bash

cd robot_code
```
Install the dependencies (create a virtual env recommended):

```bash

pip install opencv-python ultralytics numpy requests
```
(Note: You may need additional libraries depending on your specific LLM provider)

Configure the connection:

Open config.py (or check main.py) and update the IP Address to match your ESP32's IP.

Add your LLM API Key (OpenAI/Groq/Local) if required.

## Usage
To start the robot's brain:

```bash

python main.py
```
## Interaction:

The terminal will prompt: Type 'exit' to quit | Say 'Shadow' to begin.

Type your message to chat with Shadow.

The system will process your text, generate a witty response, and perform actions based on the detected emotion.

## Example Interaction:

You: "Hello Shadow, can you see me?" Shadow: "I see a human looking at a screen. Looking good! [Emotion: Happy]" (Robot moves head up and down)


## License
MIT License

Author: [anhbanhieucode]
