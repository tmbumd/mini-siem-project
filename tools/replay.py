import argparse, socket, time

def send_udp(host, port, line):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.sendto(line.encode(), (host, port))
    s.close()

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=5514)
    ap.add_argument("--paths", required=True, help="comma-separated list of log files")
    ap.add_argument("--speed", type=float, default=1.0, help="1.0 = realtime, 0 = as fast as possible")
    args = ap.parse_args()

    files = [p for p in args.paths.split(",") if p]
    for p in files:
        with open(p, "r") as f:
            for line in f:
                line = line.strip()
                if not line: continue
                send_udp(args.host, args.port, line)
                if args.speed > 0:
                    time.sleep(0.02/args.speed)
    print("Replay complete.")

if __name__ == "__main__":
    main()
