from yolo_detector import YoloDetector
from planner import robot_planner
from llm_interface import call_llm
import re
from motor_control import show_emoji
conversation_history = [
    {"role": "system", "content": (
        "You are Shadow, a friendly and witty robot assistant. "
        "Always respond in English, with natural language and humor. "
        "IMPORTANT: At the END of EVERY reply, you MUST write this on a new line:\n"
        "'Emotion: [number]'\n"
        "Where [number] is ONE of the following:\n"
        "1: Happy\n2: Sad\n3: Angry\n4: Laugh\n5: Cringe\n6: Normal"
        "ALWAYS include it. DO NOT skip it. DO NOT forget. NO excuses.\n"
        "Even if you are unsure, pick the closest matching emotion from the list."
    )}
]



yolo_thread = YoloDetector()
yolo_thread.start()

listening_mode = False

try:
    while True:
        print("\n Type 'exit' to quit | Say 'Shadow' to begin ")
        user_input = input(" You: ").strip()

        if user_input.lower() == "exit":
            break
        
        elif listening_mode:    
            goal = user_input.strip()
            current_objects = yolo_thread.get_detected_objects()
            print(f"Detected: {current_objects}")
            robot_planner(goal, current_objects, conversation_history, yolo_thread)
            listening_mode=False
            continue

        elif user_input.lower() in ["shadow", "hey shadow", "shadow buddy", "hey there shadow"]:
            print("Shadow: Coming right away...")
            listening_mode=True

        else:
            conversation_history.append({"role": "user", "content": user_input})
            response = call_llm(conversation_history, title="Chat")
            if response:
                print(f"\n Shadow: {response}")

                match = re.search(r"Emotion:\s*(\d)",response)
                print (match)
                if match:
                    emoji_id = int(match.group(1))
                    print(emoji_id)
                    show_emoji(emoji_id)

                conversation_history.append({"role": "assistant", "content": response})
finally:
    print("\n Stopping...")
    yolo_thread.stop()
    yolo_thread.join()
    print("✅ Shutdown complete.")
