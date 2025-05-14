import time
import ast
from motor_control import control_motor, control_servo, show_emoji
from llm_interface import call_llm

def execute_robot_plan(plan, yolo_thread):
    for command in plan:
        print(f"\n▶️ Executing: {command}")
        try:
            if command.startswith("find_object("):
                obj = eval(command[12:-1])
                show_emoji(4)
                while obj not in yolo_thread.get_detected_objects():
                    control_motor("left", 150, 0.5)
                    time.sleep(0.5)
                print(f"✅ Found {obj}!")

            elif command.startswith("go_to("):
                obj = eval(command[6:-1])
                while obj in yolo_thread.get_detected_objects():
                    control_motor("forward", 150, 1)
                    time.sleep(1)

            elif command == "pick_up":
                control_servo(1, 0, 90)
                time.sleep(1)
                control_servo(2, 0, 90)
                show_emoji(1)

            elif command == "let_down":
                control_servo(2, 90, 0)
                time.sleep(1)
                control_servo(1, 90, 0)
                show_emoji(2)

            elif command.startswith("change_emotion("):
                emoji_id = int(command[15:-1])
                show_emoji(emoji_id)

            elif command.startswith("speak("):
                text = eval(command[6:-1])
                print(f"🗣️ {text}")

        except Exception as e:
            print("❌ Execution error:", e)

def robot_planner(goal, detected_list, conversation_history, yolo_thread):
    prompt = f"""
    Your name is Shadow, a friendly robot. Generate a list of high-level actions to complete the goal.

    Goal: {goal}
    Detected objects: {detected_list}

    Available actions:
    - find_object(object_name)
    - go_to(object_name)
    - pick_up
    - let_down
    - change_emotion(ID) [1:Happy, 2:Sad, 3:Angry, 4:Laugh, 5:Cringe]
    - speak(text)

    Only output a valid Python list of commands. Do not include any explanation, markdown, or extra text. Just output the list like:
    ["find_object('coffee')", "go_to('coffee')", "pick_up", "let_down"]
    """

    conversation_history.append({"role": "user", "content": prompt})
    response = call_llm(conversation_history, title="RobotPlanner")

    if response:
        print("\n📩 LLM Response:")
        print(response)
        try:
            plan = ast.literal_eval(response)  # safer than eval
            print("\n📋 Action Plan:")
            for i, action in enumerate(plan, 1):
                print(f"{i}. {action}")
            execute_robot_plan(plan, yolo_thread)
            conversation_history.append({"role": "assistant", "content": response})
        except Exception as e:
            print("❌ Failed to parse or execute plan:", e)
