import requests

def control_motor(state, speed, duration):
    try:
        url = f"http://192.168.1.2/motor?state={state}&speed={speed}&time={duration}"
        requests.get(url, timeout=1)
        print(f"🚗 Motor: {state}, speed={speed}, time={duration}")
    except Exception as e:
        print("⚠️ Motor control error:", e)

def control_servo(servo, angle_from, angle_to):
    try:
        url = f"http://192.168.1.2/servo?servo={servo}&from={angle_from}&to={angle_to}"
        requests.get(url, timeout=1)
        print(f"🦾 Servo: {servo}, from {angle_from} to {angle_to}")
    except Exception as e:
        print("⚠️ Servo error:", e)

def show_emoji(emoji_id):
    try:
        url = f"http://192.168.1.2/emoji?value={emoji_id}"
        requests.get(url, timeout=1)
        print(f"🙂 Emoji ID: {emoji_id}")
    except Exception as e:
        print("⚠️ Emoji error:", e)
