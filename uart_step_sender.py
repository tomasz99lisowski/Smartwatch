import argparse
import sys
import time

try:
    import serial
except Exception:
    serial = None


def build_packet(user_id: int, steps: int) -> str:
    # Match the dashboard parser format: {'User': 1, 'Steps': 123}
    return f"{{'User': {user_id}, 'Steps': {steps}}}\n"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Send step packets over UART for dashboard testing."
    )
    parser.add_argument("--port", default="COM5", help="Serial port (default: COM5)")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument("--steps", type=int, required=True, help="Starting step value to send")
    parser.add_argument("--user", type=int, choices=[1, 2], help="Send only for one user (1 or 2)")
    parser.add_argument(
        "--count",
        type=int,
        default=1,
        help="How many packets to send (default: 1)",
    )
    parser.add_argument(
        "--interval",
        type=float,
        default=1.0,
        help="Seconds between packets (default: 1.0)",
    )
    parser.add_argument(
        "--increment",
        type=int,
        default=0,
        help="Increase steps by this amount each packet (default: 0)",
    )
    parser.add_argument(
        "--start-user",
        type=int,
        choices=[1, 2],
        default=1,
        help="When --user is omitted, alternate users starting from this value (default: 1)",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    if serial is None:
        print("pyserial is not installed. Install with: pip install pyserial")
        return 1

    if args.count < 1:
        print("--count must be >= 1")
        return 1

    try:
        ser = serial.Serial(args.port, args.baud, timeout=1)
    except Exception as exc:
        print(f"Could not open {args.port}: {exc}")
        return 1

    print(f"Connected to {args.port} @ {args.baud}")

    try:
        for i in range(args.count):
            user_id = args.user if args.user is not None else ((args.start_user - 1 + i) % 2) + 1
            steps = args.steps + (i * args.increment)

            packet = build_packet(user_id, steps)
            ser.write(packet.encode("utf-8"))
            print(f"Sent: {packet.strip()}")

            if i < args.count - 1:
                time.sleep(args.interval)
    finally:
        ser.close()

    print("Done.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
